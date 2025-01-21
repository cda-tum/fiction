//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp"
#include "fiction/algorithms/simulation/sidb/clustercomplete.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math_utils.hpp"

#include <fmt/format.h>
#include <mockturtle/utils/stopwatch.hpp>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct stores the parameters for the *Critical Temperature*` algorithm.
 */
struct critical_temperature_params
{
    /**
     * The parameters used to determine if a layout is `operational` or `non-operational`.
     */
    is_operational_params operational_params{};
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
    sidb_simulation_parameters simulation_parameters{};
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

template <typename Lyt>
class critical_temperature_impl
{
  public:
    critical_temperature_impl(const Lyt& lyt, const critical_temperature_params& ps, critical_temperature_stats& st) :
            layout{lyt},
            params{ps},
            stats{st},
            bii(bdl_input_iterator<Lyt>{layout, params.operational_params.input_bdl_iterator_params}),
            critical_temperature{ps.max_temperature}
    {
        stats.simulation_parameters = params.operational_params.simulation_parameters;
        stats.algorithm_name        = sidb_simulation_engine_name(params.operational_params.sim_engine);
    }

    /**
     * *Gate-based Critical Temperature* Simulation of a SiDB layout for a given Boolean function.
     *

     * tparam TT Type of the truth table.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     */
    template <typename TT>
    void gate_based_simulation(const std::vector<TT>& spec) noexcept
    {
        mockturtle::stopwatch stop{stats.time_total};
        if (layout.is_empty())
        {
            critical_temperature = 0.0;
            return;
        }

        assert(layout.num_pis() > 0 && "gate needs input cells");
        assert(layout.num_pos() > 0 && "gate needs output cells");

        if (layout.num_cells() > 1)
        {
            const auto output_bdl_pairs =
                detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT,
                                 params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params);

            auto input_bdl_wires  = std::vector<bdl_wire<Lyt>>{};
            auto output_bdl_wires = std::vector<bdl_wire<Lyt>>{};

            if (params.operational_params.op_condition == is_operational_params::operational_condition::REJECT_KINKS)
            {
                input_bdl_wires =
                    detect_bdl_wires(layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                     bdl_wire_selection::INPUT);
                output_bdl_wires =
                    detect_bdl_wires(layout, params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                                     bdl_wire_selection::OUTPUT);
            }

            // number of different input combinations
            for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
            {
                // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
                if (can_positive_charges_occur(*bii, params.operational_params.simulation_parameters))
                {
                    critical_temperature = 0.0;
                    return;
                }

                // performs physical simulation of a given SiDB layout at a given input combination
                const auto sim_result = physical_simulation_of_bdl_iterator(bii);

                if (sim_result.charge_distributions.empty())
                {
                    critical_temperature = 0.0;
                    return;
                }
                stats.num_valid_lyt = sim_result.charge_distributions.size();
                // The energy distribution of the physically valid charge configurations for the given layout is
                // determined.
                const auto distribution = energy_distribution(sim_result.charge_distributions);

                sidb_energy_and_state_type energy_state_type{};

                if (params.operational_params.op_condition ==
                    is_operational_params::operational_condition::REJECT_KINKS)
                {
                    energy_state_type = calculate_energy_and_state_type_with_kinks_rejected<Lyt>(
                        distribution, sim_result.charge_distributions, spec, i, input_bdl_wires, output_bdl_wires);
                }
                else
                {
                    // A label that indicates whether the state still fulfills the logic.
                    energy_state_type = calculate_energy_and_state_type_with_kinks_accepted<Lyt>(
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
        mockturtle::stopwatch       stop{stats.time_total};
        sidb_simulation_result<Lyt> simulation_results{};

        if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            const quickexact_params<cell<Lyt>> qe_params{
                params.operational_params.simulation_parameters,
                quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};

            // All physically valid charge configurations are determined for the given layout (`QuickExact` simulation
            // is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = quickexact(layout, qe_params);
        }
        else if (params.operational_params.sim_engine == sidb_simulation_engine::CLUSTERCOMPLETE)
        {
#if (FICTION_ALGLIB_ENABLED)
            const clustercomplete_params<cell<Lyt>> cc_params{params.operational_params.simulation_parameters};

            // All physically valid charge configurations are determined for the given layout (`ClusterComplete`
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = clustercomplete(layout, cc_params);
#else   // FICTION_ALGLIB_ENABLED
            assert(false && "ALGLIB must be enabled if ClusterComplete is to be used");
#endif  // FICTION_ALGLIB_ENABLED
        }
        else if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKSIM)
        {
            const quicksim_params qs_params{params.operational_params.simulation_parameters, params.iteration_steps,
                                            params.alpha};

            // All physically valid charge configurations are determined for the given layout (probabilistic ground
            // state simulation is used).
            simulation_results = quicksim(layout, qs_params);
        }
        else
        {
            assert(false && "unsupported simulation engine");
        }

        // The number of physically valid charge configurations is stored.
        stats.num_valid_lyt = simulation_results.charge_distributions.size();

        const auto distribution = energy_distribution(simulation_results.charge_distributions);

        // if there is more than one metastable state
        if (distribution.size() > 1)
        {
            const auto ground_state_energy        = distribution.cbegin()->first;
            const auto first_excited_state_energy = std::next(distribution.cbegin())->first;

            // The energy difference between the first excited and the ground state in meV.
            if (stats.energy_between_ground_state_and_first_erroneous >
                (first_excited_state_energy - ground_state_energy) * 1000)
            {
                stats.energy_between_ground_state_and_first_erroneous =
                    (first_excited_state_energy - ground_state_energy) * 1000;
            }
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
     * @param energy_and_state_type All energies of all physically valid charge distributions with the corresponding
     * state type (i.e. transparent, erroneous).
     * @param min_energy Minimal energy of all physically valid charge distributions of a given layout (unit: eV).
     * @return State type (i.e. transparent, erroneous) of the ground state is returned.
     */
    bool is_ground_state_transparent(const sidb_energy_and_state_type& energy_and_state_type,
                                     const double                      min_energy) noexcept
    {
        bool ground_state_is_transparent = false;

        for (const auto& [energy, state_type] : energy_and_state_type)
        {
            // Check if there is at least one ground state that satisfies the logic (transparent). Round the energy
            // value of the given valid_layout to six decimal places to overcome possible rounding errors and for
            // comparability with the min_energy.
            if (std::abs(round_to_n_decimal_places(energy, 6) - round_to_n_decimal_places(min_energy, 6)) <
                    physical_constants::POP_STABILITY_ERR &&
                state_type)
            {
                ground_state_is_transparent = true;
            }

            if (!state_type && (energy > min_energy) && ground_state_is_transparent &&
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
     * @param energy_state_type All energies of all physically valid charge distributions with the corresponding state
     * type (i.e. transparent, erroneous).
     */
    void determine_critical_temperature(const sidb_energy_and_state_type& energy_state_type) noexcept
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
    Lyt layout;
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
    bdl_input_iterator<Lyt> bii;
    /**
     * Critical temperature [K].
     */
    double critical_temperature;
    /**
     * This function conducts physical simulation of the given layout (gate layout with certain input combination). The
     * simulation results are stored in the `sim_result_100` variable.
     *
     * @param bdl_iterator A reference to a BDL input iterator representing the gate layout at a given input
     * combination. The simulation is performed based on the configuration represented by the iterator.
     * @return Simulation results.
     */
    [[nodiscard]] sidb_simulation_result<Lyt>
    physical_simulation_of_bdl_iterator(const bdl_input_iterator<Lyt>& bdl_iterator) noexcept
    {

        if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKEXACT)
        {
            // perform exact simulation
            const quickexact_params<cell<Lyt>> qe_params{
                params.operational_params.simulation_parameters,
                fiction::quickexact_params<cell<Lyt>>::automatic_base_number_detection::OFF};
            return quickexact(*bdl_iterator, qe_params);
        }
        if (params.operational_params.sim_engine == sidb_simulation_engine::CLUSTERCOMPLETE)
        {
#if (FICTION_ALGLIB_ENABLED)
            // perform ClusterComplete simulation -- base 3 simulation is allowed
            const clustercomplete_params<cell<Lyt>> cc_params{params.operational_params.simulation_parameters};
            return clustercomplete(*bdl_iterator, cc_params);
#else   // FICTION_ALGLIB_ENABLED
            assert(false && "ALGLIB must be enabled if ClusterComplete is to be used");
#endif  // FICTION_ALGLIB_ENABLED
        }
        if (params.operational_params.sim_engine == sidb_simulation_engine::QUICKSIM)
        {
            assert(params.operational_params.simulation_parameters.base == 2 &&
                   "QuickSim does not support base-3 simulation");

            const quicksim_params qs_params{params.operational_params.simulation_parameters, params.iteration_steps,
                                            params.alpha};
            return quicksim(*bdl_iterator, qs_params);
        }

        if (params.operational_params.sim_engine == sidb_simulation_engine::EXGS)
        {
            return exhaustive_ground_state_simulation(*bdl_iterator, params.operational_params.simulation_parameters);
        }

        assert(false && "unsupported simulation engine");

        return sidb_simulation_result<Lyt>{};
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
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT Type of the truth table.
 * @param lyt The layout to simulate.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 * @return The critical temperature (unit: K).
 */
template <typename Lyt, typename TT>
double critical_temperature_gate_based(const Lyt& lyt, const std::vector<TT>& spec,
                                       const critical_temperature_params& params = {},
                                       critical_temperature_stats*        pst    = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    critical_temperature_stats st{};

    detail::critical_temperature_impl<Lyt> p{lyt, params, st};

    p.gate_based_simulation(spec);

    if (pst)
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
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 * @return The critical temperature (unit: K)
 */
template <typename Lyt>
double critical_temperature_non_gate_based(const Lyt& lyt, const critical_temperature_params& params = {},
                                           critical_temperature_stats* pst = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    critical_temperature_stats st{};

    detail::critical_temperature_impl<Lyt> p{lyt, params, st};

    p.non_gate_based_simulation();

    if (pst)
    {
        *pst = st;
    }

    return p.get_critical_temperature();
}

}  // namespace fiction

#endif  // FICTION_CRITICAL_TEMPERATURE_HPP
