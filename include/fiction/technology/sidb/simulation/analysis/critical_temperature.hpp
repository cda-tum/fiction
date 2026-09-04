/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Temperature above which an SiDB layout stops behaving as intended.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp"
#include "fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp"
#include "fiction/technology/sidb/simulation/analysis/energy_distribution.hpp"
#include "fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp"
#include "fiction/technology/sidb/simulation/engine.hpp"
#include "fiction/technology/sidb/simulation/engines/clustercomplete.hpp"
#include "fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp"
#include "fiction/technology/sidb/simulation/engines/quickexact.hpp"
#include "fiction/technology/sidb/simulation/engines/quicksim.hpp"
#include "fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp"
#include "fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp"
#include "fiction/technology/sidb/simulation/logic/is_operational.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::analysis
{

/**
 * This struct stores the parameters for the *Critical Temperature* algorithm.
 */
struct critical_temperature_params
{
    /**
     * The parameters used to determine if a layout is `operational` or `non-operational`.
     */
    sidb::simulation::logic::is_operational_params operational_params{};
    /**
     * Probability threshold for ground state population. The temperature at which the simulation finds the ground state
     * to be populated with a probability of less than the given percentage, is determined to be the critical
     * temperature. For gate-based simulation, this is the probability of erroneous calculations of the gate.
     */
    double confidence_level{0.99};
    /**
     * Maximum simulation temperature beyond which no simulation will be conducted (~ 126 °C by default) (unit: K).
     */
    double max_temperature{400};
    /**
     * Number of iteration steps for the *QuickSim* algorithm (only applicable if engine == QUICKSIM).
     */
    uint64_t iteration_steps{80};
    /**
     * Alpha parameter for the *QuickSim* algorithm (only applicable if engine == QUICKSIM).
     */
    double alpha{0.7};
};

/**
 * This struct stores the result of the temperature simulation.
 */
struct critical_temperature_stats
{
    /**
     * The total runtime of the critical temperature computation.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * Name of the algorithm used to compute the physically valid charge distributions.
     */
    std::string algorithm_name{};
    /**
     * Number of physically valid charge configurations.
     */
    uint64_t num_valid_lyt{};
    /**
     * Energy difference between the ground state and the first (erroneous) excited state (unit: meV).
     */
    double energy_between_ground_state_and_first_erroneous = std::numeric_limits<double>::infinity();
    /**
     * Prints the simulation results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        if (num_valid_lyt != 0)
        {
            out << fmt::format("'# of physically valid charge configurations': {} | Energy between ground state and "
                               "first erroneous in meV: {}\n",
                               num_valid_lyt, energy_between_ground_state_and_first_erroneous);
        }
        else
        {
            out << "no state found | if two-state simulation was used, try re-running with three states\n";
        }

        out << "_____________________________________________________\n";
    }
};

namespace detail
{

class critical_temperature_impl
{
  public:
    /**
     * Initializes the algorithm for a layout.
     *
     * @param source_layout The layout to simulate.
     * @param ps Parameters for the critical temperature algorithm.
     * @param st Statistics of the process.
     */
    critical_temperature_impl(layout source_layout, const critical_temperature_params& ps,
                              critical_temperature_stats& st) :
            sidb_layout{std::move(source_layout)},
            params{ps},
            stats{st},
            bii(logic::bdl_input_iterator{sidb_layout, params.operational_params.input_bdl_iterator_params}),
            critical_temperature{ps.max_temperature}
    {
        stats.sim_params     = params.operational_params.sim_params;
        stats.algorithm_name = sidb::simulation::engine_name(params.operational_params.sim_engine);
    }

    /**
     * Constructor to initialize the algorithm with a pre-generated input configuration.
     *
     * None of the BDL detection results depend on the simulation parameters, so a caller that simulates the same
     * layout under many parameter settings can determine them once and pass them to every call. The layouts and
     * the detection results are not copied, are only read, and must outlive this object; the same ones may be shared by
     * concurrently running instances.
     *
     * @param input_pattern_lyts One layout per input pattern, indexed by input pattern, as generated by
     * `generate_bdl_input_pattern_layouts`.
     * @param ps Parameters for the critical temperature algorithm.
     * @param st Statistics of the process.
     * @param output_pairs Output BDL pairs of the layout.
     * @param input_wires BDL input wires of the layout.
     * @param output_wires BDL output wires of the layout.
     */
    critical_temperature_impl(const std::vector<layout>& input_pattern_lyts, const critical_temperature_params& ps,
                              critical_temperature_stats&                       st,
                              const std::vector<logic::bdl_pair<lattice_site>>& output_pairs,
                              const std::vector<logic::bdl_wire>&               input_wires,
                              const std::vector<logic::bdl_wire>&               output_wires) :
            // a shallow copy, so that the `is_empty()`, `num_pos()` and `num_cells()` guards keep working
            sidb_layout{input_pattern_lyts.front()},
            params{ps},
            stats{st},
            // the input pattern layouts make the iterator redundant; this is the same no-op instantiation that
            // `is_operational_impl` uses on its pre-generated-layouts path
            bii(logic::bdl_input_iterator{layout{}}),
            critical_temperature{ps.max_temperature},
            input_pattern_layouts{&input_pattern_lyts},
            pre_detected_output_bdl_pairs{&output_pairs},
            pre_detected_input_bdl_wires{&input_wires},
            pre_detected_output_bdl_wires{&output_wires}
    {
        stats.sim_params     = params.operational_params.sim_params;
        stats.algorithm_name = sidb::simulation::engine_name(params.operational_params.sim_engine);
    }

    /**
     * *Gate-based Critical Temperature* Simulation of a SiDB layout for a given Boolean function.
     *
     * @tparam TT Type of the truth table.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     */
    template <typename TT>
    void gate_based_simulation(const std::vector<TT>& spec) noexcept
    {
        const mockturtle::stopwatch stop{stats.time_total};
        if (sidb_layout.is_empty())
        {
            critical_temperature = 0.0;
            return;
        }

        assert((input_pattern_layouts != nullptr || sidb_layout.num_pis() > 0) && "gate needs input cells");
        assert(sidb_layout.num_pos() > 0 && "gate needs output cells");

        if (sidb_layout.num_cells() > 1)
        {
            // the BDL detection results are taken from the caller when it supplied them, and determined here
            // otherwise. None of them depend on the simulation parameters
            const auto detected_output_bdl_pairs =
                pre_detected_output_bdl_pairs != nullptr ?
                    std::vector<logic::bdl_pair<lattice_site>>{} :
                    sidb::simulation::logic::detect_bdl_pairs(
                        sidb_layout, sidb::sidb_technology::cell_type::OUTPUT,
                        params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

            const auto& output_bdl_pairs =
                pre_detected_output_bdl_pairs != nullptr ? *pre_detected_output_bdl_pairs : detected_output_bdl_pairs;

            auto detected_input_bdl_wires  = std::vector<logic::bdl_wire>{};
            auto detected_output_bdl_wires = std::vector<logic::bdl_wire>{};

            if (params.operational_params.op_condition ==
                    sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS &&
                pre_detected_input_bdl_wires == nullptr)
            {
                detected_input_bdl_wires = sidb::simulation::logic::detect_bdl_wires(
                    sidb_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                    sidb::simulation::logic::bdl_wire_selection::INPUT);
                detected_output_bdl_wires = sidb::simulation::logic::detect_bdl_wires(
                    sidb_layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                    sidb::simulation::logic::bdl_wire_selection::OUTPUT);
            }

            const auto& input_bdl_wires =
                pre_detected_input_bdl_wires != nullptr ? *pre_detected_input_bdl_wires : detected_input_bdl_wires;
            const auto& output_bdl_wires =
                pre_detected_output_bdl_wires != nullptr ? *pre_detected_output_bdl_wires : detected_output_bdl_wires;

            // number of different input combinations
            for (auto i = 0u; i < spec.front().num_bits(); ++i)
            {
                const auto& lyt_with_input_pattern = layout_with_input_pattern(i);

                // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
                if (can_positive_charges_occur(lyt_with_input_pattern, params.operational_params.sim_params))
                {
                    critical_temperature = 0.0;
                    return;
                }

                // performs physical simulation of a given SiDB layout at a given input combination
                const auto sim_result = physical_simulation_of_layout(lyt_with_input_pattern);

                if (sim_result.charge_distributions.empty())
                {
                    critical_temperature = 0.0;
                    return;
                }
                stats.num_valid_lyt = sim_result.charge_distributions.size();
                // The energy distribution of the physically valid charge configurations for the given layout is
                // determined.
                const auto distribution = calculate_energy_distribution(sim_result.charge_distributions);

                energy_and_state_type energy_state_type{};

                if (params.operational_params.op_condition ==
                    sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS)
                {
                    energy_state_type = calculate_energy_and_state_type_with_kinks_rejected(
                        lyt_with_input_pattern, distribution, sim_result.charge_distributions, spec, i, input_bdl_wires,
                        output_bdl_wires);
                }
                else
                {
                    // A label that indicates whether the state still fulfills the logic.
                    energy_state_type = calculate_energy_and_state_type_with_kinks_accepted(
                        distribution, sim_result.charge_distributions, output_bdl_pairs, spec, i);
                }

                const auto min_energy = energy_state_type.cbegin()->first;

                auto ground_state_is_transparent = is_ground_state_transparent(energy_state_type, min_energy);

                if (ground_state_is_transparent)
                {
                    this->determine_critical_temperature(energy_state_type);
                }

                else
                {
                    critical_temperature = 0.0;  // If no ground state fulfills the logic, the Critical
                                                 // Temperature is zero. May be worth it to change µ_.
                }
            }
        }
    }

    /**
     * *Gate-based Critical Temperature* Simulation of a SiDB layout for a given Boolean function.
     */
    void non_gate_based_simulation() noexcept
    {
        const mockturtle::stopwatch stop{stats.time_total};
        result                      simulation_results{};

        if (params.operational_params.sim_engine == engine::QUICKEXACT)
        {
            const sidb::simulation::engines::quickexact_params qe_params{
                .sim_params = params.operational_params.sim_params,
                .base_number_detection =
                    sidb::simulation::engines::quickexact_params::automatic_base_number_detection::OFF};

            // All physically valid charge configurations are determined for the given layout (`QuickExact`
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = sidb::simulation::engines::quickexact(sidb_layout, qe_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        else if (params.operational_params.sim_engine == engine::CLUSTERCOMPLETE)
        {
            const sidb::simulation::engines::clustercomplete_params cc_params{.sim_params =
                                                                                  params.operational_params.sim_params};

            // All physically valid charge configurations are determined for the given layout (`ClusterComplete`
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = sidb::simulation::engines::clustercomplete(sidb_layout, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED
        else if (params.operational_params.sim_engine == engine::QUICKSIM)
        {
            const sidb::simulation::engines::quicksim_params qs_params{.sim_params =
                                                                           params.operational_params.sim_params,
                                                                       .iteration_steps = params.iteration_steps,
                                                                       .alpha           = params.alpha};

            // All physically valid charge configurations are determined for the given layout (probabilistic ground
            // state simulation is used).
            if (const auto result = sidb::simulation::engines::quicksim(sidb_layout, qs_params); result.has_value())
            {
                simulation_results = result.value();
            }
            else
            {
                return;
            }
        }
        else
        {
            assert(false && "unsupported simulation engine");
        }

        // The number of physically valid charge configurations is stored.
        stats.num_valid_lyt = simulation_results.charge_distributions.size();

        const auto distribution = calculate_energy_distribution(simulation_results.charge_distributions);

        // if there is more than one metastable state
        if (distribution.size() > 1)
        {
            const auto ground_state        = distribution.get_nth_state(0);
            const auto first_excited_state = distribution.get_nth_state(1);

            if (!ground_state.has_value() || !first_excited_state.has_value())
            {
                return;
            }

            const auto ground_state_energy        = ground_state->electrostatic_potential_energy;
            const auto first_excited_state_energy = first_excited_state->electrostatic_potential_energy;

            // The energy difference between the first excited and the ground state in meV.
            stats.energy_between_ground_state_and_first_erroneous =
                std::min(stats.energy_between_ground_state_and_first_erroneous,
                         (first_excited_state_energy - ground_state_energy) * 1000);
        }

        std::vector<double> temp_values{};  // unit: K

        // Calculate the number of iterations as an integer
        const auto num_iterations = static_cast<uint64_t>(std::round(params.max_temperature * 100));
        // Reserve space for the vector
        temp_values.reserve(num_iterations);
        for (uint64_t i = 1; i <= num_iterations; i++)
        {
            temp_values.emplace_back(static_cast<double>(i) / 100.0);
        }

        // This function determines the critical temperature for a given confidence level.
        for (const auto& temp : temp_values)
        {
            // If the occupation probability of excited states exceeds the given threshold.
            if (occupation_probability_non_gate_based(distribution, temp) > (1 - params.confidence_level) &&
                (temp < critical_temperature))
            {
                // The current temperature is stored as the critical temperature.
                critical_temperature = temp;

                break;
            }

            if (std::abs(temp - params.max_temperature) < 0.001 && (temp < critical_temperature))
            {
                // Maximal temperature is stored as the Critical Temperature.
                critical_temperature = params.max_temperature;
            }
        }
    }
    /**
     * Returns the critical temperature.
     *
     * @return The critical temperature (unit: K).
     */
    [[nodiscard]] double get_critical_temperature() const noexcept
    {
        return critical_temperature;
    }

  private:
    /**
     * The energy difference between the ground state and the first erroneous state is determined. Additionally, the
     * state type of the ground state is determined and returned.
     *
     * @param est All energies of all physically valid charge distributions with the corresponding
     * state type (i.e. transparent, erroneous).
     * @param min_energy Minimal energy of all physically valid charge distributions of a given layout (unit: eV).
     * @return State type (i.e. transparent, erroneous) of the ground state is returned.
     */
    [[nodiscard]] bool is_ground_state_transparent(const energy_and_state_type& est,
                                                   const double                 min_energy) const noexcept
    {
        bool ground_state_is_transparent = false;

        for (const auto& [energy, state_type] : est)
        {
            // Check if there is at least one ground state that satisfies the logic (transparent). Round the energy
            // value of the given valid_layout to six decimal places to overcome possible rounding errors and for
            // comparability with the min_energy.
            if (std::abs(fiction::utils::math::round_to_n_decimal_places(energy, 6) -
                         fiction::utils::math::round_to_n_decimal_places(min_energy, 6)) <
                    fiction::utils::math::ERROR_MARGIN &&
                state_type == state_type::ACCEPTED)
            {
                ground_state_is_transparent = true;
            }

            if ((state_type == state_type::REJECTED) && (energy > min_energy) && ground_state_is_transparent &&
                (((energy - min_energy) * 1000) < stats.energy_between_ground_state_and_first_erroneous))
            {
                // The energy difference is stored in meV.
                stats.energy_between_ground_state_and_first_erroneous = (energy - min_energy) * 1000;
                break;
            }
        }
        return ground_state_is_transparent;
    };
    /**
     * The *Critical Temperature* is determined.
     *
     * @param energy_state_type All energies of all physically valid charge distributions with the corresponding
     * state type (i.e. transparent, erroneous).
     */
    void determine_critical_temperature(const energy_and_state_type& energy_state_type) noexcept
    {
        // Vector with temperature values from 0.01 to max_temperature * 100 K in 0.01 K steps is generated.
        std::vector<double> temp_values{};
        temp_values.reserve(static_cast<uint64_t>(params.max_temperature * 100));

        for (uint64_t i = 1; i <= static_cast<uint64_t>(params.max_temperature * 100); i++)
        {
            temp_values.emplace_back(static_cast<double>(i) / 100.0);
        }
        // This function determines the Critical Temperature for a given confidence level.
        for (const auto& temp : temp_values)
        {
            // If the occupation probability of erroneous states exceeds the given threshold...
            if (occupation_probability_gate_based(energy_state_type, temp) > (1 - params.confidence_level) &&
                (temp < critical_temperature))
            {
                // The current temperature is stored as Critical Temperature.
                critical_temperature = temp;
                break;
            }
            if (std::abs(temp - params.max_temperature) < 0.001 && (temp < critical_temperature))
            {
                // Maximal temperature is stored as Critical Temperature.
                critical_temperature = params.max_temperature;
            }
        }
    }

    /**
     * SiDB cell-level layout.
     */
    layout sidb_layout;
    /**
     * Parameters for the critical_temperature algorithm.
     */
    const critical_temperature_params& params;
    /**
     * Statistics.
     */
    critical_temperature_stats& stats;
    /**
     * Iterator that iterates over all possible input states.
     */
    logic::bdl_input_iterator bii;
    /**
     * Critical temperature [K].
     */
    double critical_temperature;
    /**
     * Pre-generated layouts, one per input pattern, or `nullptr` if the BDL input iterator is used instead. Not owned
     * by this object and only ever read.
     */
    const std::vector<layout>* input_pattern_layouts{nullptr};
    /**
     * Pre-detected output BDL pairs, or `nullptr` if they are to be detected here. Not owned by this object.
     */
    const std::vector<logic::bdl_pair<lattice_site>>* pre_detected_output_bdl_pairs{nullptr};
    /**
     * Pre-detected input BDL wires, or `nullptr` if they are to be detected here. Not owned by this object.
     */
    const std::vector<logic::bdl_wire>* pre_detected_input_bdl_wires{nullptr};
    /**
     * Pre-detected output BDL wires, or `nullptr` if they are to be detected here. Not owned by this object.
     */
    const std::vector<logic::bdl_wire>* pre_detected_output_bdl_wires{nullptr};

    /**
     * Returns the layout with the given input pattern applied.
     *
     * Reads from the pre-generated input pattern layouts if they were supplied, and drives the BDL input iterator to
     * the requested pattern otherwise.
     *
     * @param input_pattern The input pattern to apply.
     * @return The layout with `input_pattern` applied.
     */
    [[nodiscard]] const layout& layout_with_input_pattern(const uint64_t input_pattern) noexcept
    {
        if (input_pattern_layouts != nullptr)
        {
            assert(input_pattern < input_pattern_layouts->size() && "input pattern out of range");

            return (*input_pattern_layouts)[input_pattern];
        }

        bii = input_pattern;

        return *bii;
    }
    /**
     * This function conducts physical simulation of the given layout (gate layout with certain input
     * combination). The simulation results are stored in the `sim_result_100` variable.
     *
     * @param lyt_with_input_pattern The SiDB layout with a given input combination applied.
     * @return Simulation results.
     */
    [[nodiscard]] result physical_simulation_of_layout(const layout& lyt_with_input_pattern) noexcept
    {
        if (params.operational_params.sim_engine == engine::EXGS)
        {
            // perform exhaustive ground state simulation
            return sidb::simulation::engines::exhaustive_ground_state_simulation(lyt_with_input_pattern,
                                                                                 params.operational_params.sim_params);
        }
        if (params.operational_params.sim_engine == engine::QUICKEXACT)
        {
            // perform QuickExact exact simulation
            const sidb::simulation::engines::quickexact_params qe_params{
                .sim_params = params.operational_params.sim_params,
                .base_number_detection =
                    fiction::sidb::simulation::engines::quickexact_params::automatic_base_number_detection::OFF};
            return sidb::simulation::engines::quickexact(lyt_with_input_pattern, qe_params);
        }
#if (FICTION_ALGLIB_ENABLED)
        if (params.operational_params.sim_engine == engine::CLUSTERCOMPLETE)
        {
            // perform ClusterComplete exact simulation
            const sidb::simulation::engines::clustercomplete_params cc_params{.sim_params =
                                                                                  params.operational_params.sim_params};
            return sidb::simulation::engines::clustercomplete(lyt_with_input_pattern, cc_params);
        }
#endif  // FICTION_ALGLIB_ENABLED
        if (params.operational_params.sim_engine == engine::QUICKSIM)
        {
            assert(params.operational_params.sim_params.base == 2 && "QuickSim does not support base-3 simulation");

            const sidb::simulation::engines::quicksim_params qs_params{.sim_params =
                                                                           params.operational_params.sim_params,
                                                                       .iteration_steps = params.iteration_steps,
                                                                       .alpha           = params.alpha};

            if (const auto result = sidb::simulation::engines::quicksim(lyt_with_input_pattern, qs_params))
            {
                return result.value();
            }
            return result{};  // return empty result if no valid charge distribution was
                              // found
        }

        assert(false && "unsupported simulation engine");

        return result{};
    }
};

}  // namespace detail

/**
 * This algorithm performs temperature-aware SiDB simulation as proposed in \"Temperature Behavior of Silicon Dangling
 * Bond Logic\" by J. Drewniok, M. Walter, and R. Wille in IEEE NANO 2023
 * (https://ieeexplore.ieee.org/document/10231259). It comes in two flavors: gate-based and non-gate based.
 *
 * For *Gate-based Critical Temperature* Simulation, the Critical Temperature is defined as follows:
 * The temperature at which the erroneous charge distributions are populated by more than \f$1 - \eta\f$, where
 * \f$\eta \in [0,1]\f$.
 *
 * @tparam TT Type of the truth table.
 * @param lyt The layout to simulate.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 * @return The critical temperature (unit: K).
 */
template <typename TT>
double critical_temperature_gate_based(const layout& lyt, const std::vector<TT>& spec,
                                       const critical_temperature_params& params = {},
                                       critical_temperature_stats*        pst    = nullptr)
{

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.end());

    critical_temperature_stats st{};

    detail::critical_temperature_impl p{lyt, params, st};

    p.gate_based_simulation(spec);

    if (pst != nullptr)
    {
        *pst = st;
    }

    return p.get_critical_temperature();
}
/**
 * *Gate-based Critical Temperature* simulation of an SiDB layout from its pre-generated input pattern layouts.
 *
 * This overload takes one layout per input pattern together with the BDL detection results, instead of deriving all of
 * them from the layout. Since neither the input configuration nor the BDL pairs and wires depend on the simulation
 * parameters, a caller that simulates the same layout under many parameter settings can determine them once with
 * `generate_bdl_input_pattern_layouts`, `detect_bdl_pairs` and `detect_bdl_wires` and pass them to every call, instead
 * of re-deriving them each time.
 *
 * The layouts and the detection results are only read and may be shared by concurrent calls.
 *
 * @tparam TT Type of the truth table.
 * @param input_pattern_layouts One layout per input pattern, indexed by input pattern, as generated by
 * `generate_bdl_input_pattern_layouts`.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Simulation and physical parameters.
 * @param output_bdl_pairs Output BDL pairs of the layout.
 * @param input_bdl_wires BDL input wires of the layout.
 * @param output_bdl_wires BDL output wires of the layout.
 * @param pst Statistics.
 * @return The critical temperature (unit: K).
 * @throws std::invalid_argument if `spec` is empty, if the number of input pattern layouts does not match the number
 * of input combinations of `spec`, or if the number of output BDL pairs does not match the number of truth tables.
 */
template <typename TT>
double critical_temperature_gate_based(const std::vector<layout>& input_pattern_layouts, const std::vector<TT>& spec,
                                       const critical_temperature_params&                params,
                                       const std::vector<logic::bdl_pair<lattice_site>>& output_bdl_pairs,
                                       const std::vector<logic::bdl_wire>&               input_bdl_wires,
                                       const std::vector<logic::bdl_wire>&               output_bdl_wires,
                                       critical_temperature_stats*                       pst = nullptr)
{

    // unlike the other overload, this one indexes caller-supplied containers, so a wrong size is an out-of-bounds
    // read rather than a wrong answer. It is also reachable from `pyfiction` with arbitrary lists, so the checks
    // have to survive `NDEBUG`
    if (spec.empty())
    {
        throw std::invalid_argument("spec is empty");
    }

    if (input_pattern_layouts.size() != spec.front().num_bits())
    {
        throw std::invalid_argument(
            fmt::format("expected {} input pattern layouts for a {}-input specification, but got {}",
                        spec.front().num_bits(), spec.front().num_vars(), input_pattern_layouts.size()));
    }

    if (output_bdl_pairs.size() != spec.size())
    {
        throw std::invalid_argument(fmt::format("expected {} output BDL pairs, one per truth table, but got {}",
                                                spec.size(), output_bdl_pairs.size()));
    }

    // all elements in tts must have the same number of variables
    assert(std::ranges::adjacent_find(spec, [](const auto& a, const auto& b)
                                      { return a.num_vars() != b.num_vars(); }) == spec.end());

    critical_temperature_stats st{};

    detail::critical_temperature_impl p{input_pattern_layouts, params,          st,
                                        output_bdl_pairs,      input_bdl_wires, output_bdl_wires};

    p.gate_based_simulation(spec);

    if (pst != nullptr)
    {
        *pst = st;
    }

    return p.get_critical_temperature();
}
/**
 * For *Non-gate-based Critical Temperature* simulation, the Critical Temperature is defined as follows: The temperature
 * at which the excited charge distributions are populated by more than \f$1 - \eta\f$, where \f$\eta \in [0,1]\f$
 * is the confidence level for the presence of a working gate.
 *
 * @param lyt The layout to simulate.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 * @return The critical temperature (unit: K)
 */
inline double critical_temperature_non_gate_based(const layout& lyt, const critical_temperature_params& params = {},
                                                  critical_temperature_stats* pst = nullptr)
{

    critical_temperature_stats st{};

    detail::critical_temperature_impl p{lyt, params, st};

    p.non_gate_based_simulation();

    if (pst != nullptr)
    {
        *pst = st;
    }

    return p.get_critical_temperature();
}

}  // namespace fiction::sidb::simulation::analysis
