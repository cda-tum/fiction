//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/algorithms/iter/bdl_input_iterator.hpp"
#include "fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/physical_constants.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/hash.hpp"
#include "fiction/utils/math_utils.hpp"
#include "fiction/utils/truth_table_utils.hpp"

#include <fmt/format.h>
#include <kitty/bit_operations.hpp>
#include <kitty/dynamic_truth_table.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
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
     * An enumeration of modes to use for the *Critical Temperature* Simulation.
     */
    enum class critical_temperature_mode
    {
        /**
         * The *Critical Temperature* is determined by considering the gate logic of the given layout. In this mode, it
         * is distinguished between excited charge distributions that produce the correct output (with respect to a
         * truth table) and those that do not.
         */
        GATE_BASED_SIMULATION,
        /**
         * The *Critical Temperature* is determined by ignoring the gate logic of the given layout. This mode does not
         * distinguish between excited charge distributions that produce the correct output (with respect to a truth
         * table) and those that do not.
         */
        NON_GATE_BASED_SIMULATION
    };

    /**
     * An enumeration of simulation modes (exact vs. approximate) to use for the *Critical Temperature* Simulation.
     */
    enum class simulation_engine
    {
        /**
         * This simulation engine computes *Critical Temperature* values with 100 % accuracy.
         */
        EXACT,
        /**
         * This simulation engine quickly calculates the *Critical Temperature*. However, there may be deviations from
         * the exact *Critical Temperature*. This mode is recommended for larger layouts (> 40 SiDBs).
         */
        APPROXIMATE
    };
    /**
     * All Parameters for physical SiDB simulations.
     */
    quicksim_params simulation_params{};
    /**
     * Simulation mode to determine the *Critical Temperature*.
     */
    simulation_engine engine = simulation_engine::EXACT;
    /**
     * Probability that the ground state is less populated due to temperature. For gate-based simulation, this is the
     * probability of erroneous calculations of the gate.
     */
    double confidence_level{0.99};
    /**
     * Simulation stops at max_temperature (~ 126 °C by default) (unit: K).
     */
    double max_temperature{400};
    /**
     * Parameters for the BDL pair detection algorithms.
     */
    detect_bdl_pairs_params bdl_params{};
};

/**
 * This struct stores the result of the temperature simulation.
 *
 * @paramt Lyt SiDB cell-level layout type.
 */
template <typename Lyt>
struct critical_temperature_stats
{
    /**
     * Name of the algorithm used to compute the physically valid charge distributions.
     */
    std::string algorithm_name{};
    /**
     * *Critical Temperature* of the given layout (unit: K).
     */
    double critical_temperature{0};
    /**
     * Number of physically valid charge configurations.
     */
    uint64_t num_valid_lyt{};
    /**
     * Energy difference between the ground state and the first (erroneous) excited state (unit: eV).
     */
    double energy_between_ground_state_and_first_erroneous = std::numeric_limits<double>::infinity();
    /**
     * Prints the simulation results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("Critical Temperature  = {:.2f} K\n", critical_temperature);

        if (num_valid_lyt != 0)
        {
            out << fmt::format("'# of physically valid charge configurations': {} | Energy between ground state and "
                               "first erroneous: {}\n",
                               num_valid_lyt, energy_between_ground_state_and_first_erroneous);
        }
        else
        {
            out << "no state found | if two-state simulation was used, try re-running with three states\n";
        }

        out << "_____________________________________________________" << std::endl;
    }
};

namespace detail
{

template <typename Lyt>
class critical_temperature_impl
{
  public:
    critical_temperature_impl(const Lyt& lyt, const critical_temperature_params& ps,
                              critical_temperature_stats<Lyt>& st) :
            layout{lyt},
            params{ps},
            stats{st},
            bii(bdl_input_iterator<Lyt>{layout, params.bdl_params})

    {
        stats.critical_temperature = params.max_temperature;
    }

    /**
     * *Gate-based Critical Temperature* Simulation of a SiDB layout for a given Boolean function.
     *
     * @tparam TT The type of the truth table specifying the gate behavior.
     * @param spec Expected Boolean function of the layout given as a multi-output truth table.
     * @return `true` if the simulation succeeds and *Gate-based Critical Temperature* is determined, `false` otherwise.
     */
    template <typename TT>
    [[nodiscard]] bool gate_based_simulation(const std::vector<TT>& spec) noexcept
    {
        if (layout.is_empty())
        {
            stats.critical_temperature = 0.0;
            return true;
        }

        else if (layout.num_cells() > 1)
        {
            const auto output_bdl_pairs =
                detect_bdl_pairs(layout, sidb_technology::cell_type::OUTPUT, params.bdl_params);

            // number of different input combinations
            for (auto i = 0u; i < spec.front().num_bits(); ++i, ++bii)
            {
                // if positively charged SiDBs can occur, the SiDB layout is considered as non-operational
                if (can_positive_charges_occur(*bii, params.simulation_params.phys_params))
                {
                    stats.critical_temperature = 0.0;
                    return true;
                }

                // performs physical simulation of a given SiDB layout at a given input combination
                const auto sim_result = physical_simulation_of_layout(bii);
                if (sim_result.charge_distributions.empty())
                {
                    stats.critical_temperature = 0.0;
                    return true;
                }
                stats.num_valid_lyt = sim_result.charge_distributions.size();
                // The energy distribution of the physically valid charge configurations for the given layout is
                // determined.
                const auto distribution = energy_distribution(sim_result.charge_distributions);

                // A label that indicates whether the state still fulfills the logic.
                sidb_energy_and_state_type energy_state_type{};
                energy_state_type = calculate_energy_and_state_type(distribution, sim_result.charge_distributions,
                                                                    output_bdl_pairs, spec, i);

                const auto min_energy = energy_state_type.cbegin()->first;

                auto ground_state_is_transparent =
                    energy_between_ground_state_and_first_erroneous(energy_state_type, min_energy);

                if (ground_state_is_transparent)
                {
                    this->determine_critical_temperature(energy_state_type);
                }

                else
                {
                    stats.critical_temperature = 0.0;  // If no ground state fulfills the logic, the Critical
                                                       // Temperature is zero. May be worth it to change µ_.
                }
            }
        }

        return true;
    }

    /**
     * *Gate-based Critical Temperature* Simulation of a SiDB layout for a given Boolean function.
     *
     * @return `true` if the simulation succeeds and the *Critical Temperature* is determined, `false` otherwise.
     */
    bool non_gate_based_simulation() noexcept
    {
        sidb_simulation_result<Lyt> simulation_results{};
        if (params.engine == critical_temperature_params::simulation_engine::EXACT)
        {
            stats.algorithm_name = "QuickExact";
            // All physically valid charge configurations are determined for the given layout (`QuickExact` simulation
            // is used to provide 100 % accuracy for the Critical Temperature).
            const quickexact_params<Lyt> qe_params{params.simulation_params.phys_params,
                                                   quickexact_params<Lyt>::automatic_base_number_detection::OFF};
            simulation_results = quickexact(layout, qe_params);
        }
        else
        {
            stats.algorithm_name = "QuickSim";
            // All physically valid charge configurations are determined for the given layout (exhaustive ground state
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = quicksim(layout, params.simulation_params);
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
        temp_values.reserve(static_cast<uint64_t>(params.max_temperature * 100));

        for (uint64_t i = 1; i <= static_cast<uint64_t>(params.max_temperature * 100); i++)
        {
            temp_values.emplace_back(static_cast<double>(i) / 100.0);
        }

        // This function determines the critical temperature for a given confidence level.
        for (const auto& temp : temp_values)
        {
            // If the occupation probability of excited states exceeds the given threshold.
            if (occupation_probability_non_gate_based(distribution, temp) > (1 - params.confidence_level) &&
                temp < stats.critical_temperature)
            {
                // The current temperature is stored as the critical temperature.
                stats.critical_temperature = temp;

                break;
            }

            if (std::abs(temp - params.max_temperature) < 0.001)
            {
                // Maximal temperature is stored as the Critical Temperature.
                stats.critical_temperature = params.max_temperature;
            }
        }

        return true;
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
    bool energy_between_ground_state_and_first_erroneous(const sidb_energy_and_state_type& energy_and_state_type,
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
            if (occupation_probability_gate_based(energy_state_type, temp) > (1 - params.confidence_level))
            {
                // The current temperature is stored as Critical Temperature.
                stats.critical_temperature = temp;
                break;
            }
            if (std::abs(temp - params.max_temperature) < 0.001)
            {
                // Maximal temperature is stored as Critical Temperature.
                stats.critical_temperature = params.max_temperature;
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
    critical_temperature_stats<Lyt>& stats;
    /**
     * Iterator that iterates over all possible input states.
     */
    bdl_input_iterator<Lyt> bii;
    /**
     * This function conducts physical simulation of the given layout (gate layout with certain input combination). The
     * simulation results are stored in the `sim_result` variable.
     *
     * @param bdl_iterator A reference to a BDL input iterator representing the gate layout at a given input
     * combination. The simulation is performed based on the configuration represented by the iterator.
     * @return Simulation results.
     */
    [[nodiscard]] sidb_simulation_result<Lyt>
    physical_simulation_of_layout(const bdl_input_iterator<Lyt>& bdl_iterator) noexcept
    {
        assert(params.simulation_params.phys_params.base == 2 && "base number is set to 3");
        if (params.engine == critical_temperature_params::simulation_engine::EXACT)
        {
            // perform exact simulation
            const quickexact_params<Lyt> quickexact_params{
                params.simulation_params.phys_params,
                fiction::quickexact_params<Lyt>::automatic_base_number_detection::OFF};
            return quickexact(*bdl_iterator, quickexact_params);
        }

        if (params.engine == critical_temperature_params::simulation_engine::APPROXIMATE)
        {
            return quicksim(*bdl_iterator, params.simulation_params);
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
 * The temperature at which the erroneous charge distributions are populated by more than \f$ 1 - \eta \f$, where \f$
 * \eta \in [0,1] \f$.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @tparam TT The type of the truth table specifying the gate behavior.
 * @param lyt The layout to simulate.
 * @param spec Expected Boolean function of the layout given as a multi-output truth table.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 */
template <typename Lyt, typename TT>
bool critical_temperature_gate_based(const Lyt& lyt, const std::vector<TT>& spec,
                                     const critical_temperature_params& params = {},
                                     critical_temperature_stats<Lyt>*   pst    = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    assert(!spec.empty());
    // all elements in tts must have the same number of variables
    assert(std::adjacent_find(spec.begin(), spec.end(),
                              [](const auto& a, const auto& b) { return a.num_vars() != b.num_vars(); }) == spec.end());

    critical_temperature_stats<Lyt> st{};

    detail::critical_temperature_impl<Lyt> p{lyt, params, st};

    const auto result = p.gate_based_simulation(spec);

    if (pst)
    {
        *pst = st;
    }

    return result;
}

/**
 * For *Non-gate-based Critical Temperature* simulation, the Critical Temperature is defined as follows: The temperature
 * at which the excited charge distributions are populated by more than \f$ 1 - \eta \f$, where \f$ \eta \in [0,1] \f$
 * is the confidence level for the presence of a working gate.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 */
template <typename Lyt>
bool critical_temperature_non_gate_based(const Lyt& lyt, const critical_temperature_params& params = {},
                                         critical_temperature_stats<Lyt>* pst = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    critical_temperature_stats<Lyt> st{};

    detail::critical_temperature_impl<Lyt> p{lyt, params, st};

    const auto result = p.non_gate_based_simulation();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_CRITICAL_TEMPERATURE_HPP
