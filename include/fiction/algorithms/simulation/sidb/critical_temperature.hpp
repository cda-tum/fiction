//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp"
#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
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
 * An enumeration of modes to use for the Critical Temperature Simulation.
 */
enum class critical_temperature_mode
{
    /**
     * The Critical Temperature is determined by considering the gate logic of the given layout. In this mode, it is
     * distinguished between excited charge distributions that produce the correct output (with respect to a truth
     * table) and those that do not.
     */
    GATE_BASED_SIMULATION,
    /**
     * The Critical Temperature is determined by ignoring the gate logic of the given layout. This mode does not
     * distinguish between excited charge distributions that produce the correct output (with respect to a truth table)
     * and those that do not.
     */
    NON_GATE_BASED_SIMULATION
};

/**
 * An enumeration of simulation modes (exact vs. approximate) to use for the Critical Temperature Simulation.
 */
enum class simulation_engine
{
    /**
     * This simulation engine computes Critical Temperature values with 100 % accuracy.
     */
    EXACT,
    /**
     * This simulation engine quickly calculates the Critical Temperature. However, there may be deviations from the
     * exact Critical Temperature. This mode is recommended for larger layouts (> 40 SiDBs).
     */
    APPROXIMATE
};

/**
 * This struct stores the parameters for the `critical_temperature` algorithm.
 */
struct critical_temperature_params
{
    /**
     * Simulation mode to determine the Critical Temperature.
     */
    simulation_engine engine = simulation_engine::EXACT;
    /**
     * Mode to determine the Critical Temperature.
     */
    critical_temperature_mode temperature_mode = critical_temperature_mode::GATE_BASED_SIMULATION;
    /**
     * All Parameters for physical SiDB simulations.
     */
    quicksim_params simulation_params{};
    /**
     * Probability that the ground state is less populated due to temperature. For gate-based simulation, this is the
     * probability of erroneous calculations of the gate.
     */
    double confidence_level{0.99};
    /**
     * Simulation stops at max_temperature (room temperature ~300 K).
     */
    uint64_t max_temperature{400};
    /**
     * Truth table of the given gate (if layout is simulated in `gate-based` mode).
     */
    tt truth_table{};
    /**
     * Input bit (e.g. 0 -> 00, 1 -> 01, ...).
     */
    uint64_t input_bit{};
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
     * Critical Temperature of the given layout.
     */
    double critical_temperature{};
    /**
     * Number of physically valid charge configurations.
     */
    uint64_t num_valid_lyt{};
    /**
     * Energy difference between the ground state and the first (erroneous) excited state.
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
    critical_temperature_impl(Lyt& lyt, const critical_temperature_params& params,
                              critical_temperature_stats<Lyt>& st) :
            layout{lyt},
            parameter{params},
            temperature_stats{st}
    {}

    bool gate_based_simulation()
    {
        if (layout.is_empty())
        {
            return true;
        }

        sidb_simulation_result<Lyt> simulation_results{};
        if (parameter.engine == simulation_engine::EXACT)
        {
            temperature_stats.algorithm_name = "exgs";
            // All physically valid charge configurations are determined for the given layout (exhaustive ground state
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            const quickexact_params<Lyt> params{parameter.simulation_params.phys_params};
            simulation_results = quickexact<Lyt>(layout, params);
        }
        else
        {
            temperature_stats.algorithm_name = "quicksim";
            // All physically valid charge configurations are determined for the given layout (exhaustive ground state
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = quicksim(layout, parameter.simulation_params);
        }

        // The number of physically valid charge configurations is stored.
        temperature_stats.num_valid_lyt = simulation_results.charge_distributions.size();

        // If the layout consists of only one SiDB, the maximum temperature is returned as the Critical Temperature.
        if (layout.num_cells() == 1u)
        {
            temperature_stats.critical_temperature = static_cast<double>(parameter.max_temperature);
        }

        else if (layout.num_cells() > 1)
        {
            // All cells of the given layout are collected.
            std::vector<typename Lyt::cell> all_cells{};
            all_cells.reserve(layout.num_cells());

            layout.foreach_cell([&all_cells](const auto& c) { all_cells.push_back(c); });

            // The goal is to sort the cells from left to right and top to bottom.
            std::sort(all_cells.begin(), all_cells.end());

            // The energy distribution of the physically valid charge configurations for the given layout is determined.
            const auto distribution = energy_distribution(simulation_results.charge_distributions);

            std::vector<int64_t> output_bits_index{};
            std::vector<bool>    output_bits{};

            if (parameter.truth_table.num_vars() ==
                3)  // truth table of (double wire, cx, etc.) consists of three variables.
            {
                if (parameter.truth_table.num_bits() == 8)  // number of bits of truth table.
                {
                    output_bits_index = {-4, -3};           // double wire, cx, etc.
                    // Truth table entries for given inputs are collected.
                    output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit * 2 + 1) != 0u);
                    output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit * 2) != 0u);
                }
            }

            else if (parameter.truth_table.num_vars() == 1 && parameter.truth_table.num_bits() == 2)
            {
                output_bits_index = {-2};  // Wire, inverter, etc. -2 due to placed perturber.
                // Truth table entry for given input is collected.
                output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit) != 0u);
            }

            else if (parameter.truth_table.num_vars() == 2)
            {
                if (parameter.truth_table.num_bits() == 4 &&
                    parameter.truth_table != create_fan_out_tt())  // and, or, nand, etc.
                {
                    output_bits_index = {-2};                      // One output SiDB. -2 due to placed perturber.
                    // Truth table entry for given inputs is collected.
                    output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit) != 0u);
                }
                else
                {
                    output_bits_index = {-4, -3};  // fo2.
                    // Truth table entries for given input is collected.
                    output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit * 2 + 1) != 0u);
                    output_bits.push_back(kitty::get_bit(parameter.truth_table, parameter.input_bit * 2) != 0u);
                }
            }

            // Output cell(s) is/are collected.
            std::vector<typename Lyt::cell> output_cells;
            output_cells.reserve(output_bits_index.size());

            // Collect the SiDB cells where the output bit is read out.
            for (const auto& output_bit : output_bits_index)
            {
                output_cells.push_back(
                    all_cells[static_cast<uint64_t>(static_cast<int64_t>(layout.num_cells()) + output_bit)]);
            }

            // A label that indicates whether the state still fulfills the logic.
            sidb_energy_and_state_type energy_state_type{};
            energy_state_type = calculate_energy_and_state_type(distribution, simulation_results.charge_distributions,
                                                                output_cells, output_bits);

            const auto min_energy = energy_state_type.cbegin()->first;

            auto ground_state_is_transparent =
                energy_between_ground_state_and_first_erroneous(energy_state_type, min_energy);

            if (ground_state_is_transparent)
            {
                this->determine_critical_temperature(energy_state_type);
            }

            else
            {
                temperature_stats.critical_temperature = 0.0;  // If no ground state fulfills the logic, the Critical
                                                               // Temperature is zero. May be worth it to change µ_.
            }
        }

        return true;
    }

    bool non_gate_based_simulation()
    {
        sidb_simulation_result<Lyt> simulation_results{};
        if (parameter.engine == simulation_engine::EXACT)
        {
            temperature_stats.algorithm_name = "exgs";
            // All physically valid charge configurations are determined for the given layout (exhaustive ground state
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            const quickexact_params<Lyt> params{parameter.simulation_params.phys_params};
            simulation_results = quickexact<Lyt>(layout, params);
        }
        else
        {
            temperature_stats.algorithm_name = "quicksim";
            // All physically valid charge configurations are determined for the given layout (exhaustive ground state
            // simulation is used to provide 100 % accuracy for the Critical Temperature).
            simulation_results = quicksim(layout, parameter.simulation_params);
        }

        // The number of physically valid charge configurations is stored.
        temperature_stats.num_valid_lyt = simulation_results.charge_distributions.size();

        const auto distribution = energy_distribution(simulation_results.charge_distributions);

        // if there is more than one metastable state
        if (temperature_stats.num_valid_lyt > 1)
        {
            const auto ground_state_energy        = distribution.cbegin()->first;
            const auto first_excited_state_energy = std::next(distribution.cbegin())->first;

            // The energy difference between the first excited and the ground state in meV.
            temperature_stats.energy_between_ground_state_and_first_erroneous =
                (first_excited_state_energy - ground_state_energy) * 1000;
        }

        std::vector<double> temp_values{};
        temp_values.reserve(parameter.max_temperature * 100);

        for (uint64_t i = 1; i <= parameter.max_temperature * 100; i++)
        {
            temp_values.push_back(static_cast<double>(i) / 100.0);
        }

        // This function determines the Critical Temperature (CT) for a given confidence level.
        for (const auto& temp : temp_values)
        {
            // If the occupation probability of excited states exceeds the given threshold.
            if (occupation_probability_non_gate_based(distribution, temp) > (1 - parameter.confidence_level))
            {
                // The current temperature is stored as the Critical Temperature.
                temperature_stats.critical_temperature = temp;

                break;
            }

            if (std::abs(temp - static_cast<double>(parameter.max_temperature)) < 0.001)
            {
                // Maximal temperature is stored as the Critical Temperature.
                temperature_stats.critical_temperature = static_cast<double>(parameter.max_temperature);
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
     * @param min_energy Minimal energy of all physically valid charge distributions of a given layout.
     * @return State type (i.e. transparent, erroneous) of the ground state is returned.
     */
    bool energy_between_ground_state_and_first_erroneous(const sidb_energy_and_state_type& energy_and_state_type,
                                                         const double                      min_energy)
    {
        bool ground_state_is_transparent = false;
        for (const auto& [energy, state_type] : energy_and_state_type)
        {
            // Check if there is at least one ground state that satisfies the logic (transparent). Round the energy
            // value of the given valid_layout to six decimal places to overcome possible rounding errors and for
            // comparability with the min_energy.
            if ((round_to_n_decimal_places(energy, 6) == round_to_n_decimal_places(min_energy, 6)) && state_type)
            {
                ground_state_is_transparent = true;
            }

            if (!state_type && (energy > min_energy) && ground_state_is_transparent)
            {
                // The energy difference is stored in meV.
                temperature_stats.energy_between_ground_state_and_first_erroneous = (energy - min_energy) * 1000;
                break;
            }
        }
        return ground_state_is_transparent;
    };
    /**
     * The Critical Temperature is determined.
     *
     * @param energy_state_type All energies of all physically valid charge distributions with the corresponding state
     * type (i.e. transparent, erroneous).
     */
    void determine_critical_temperature(const sidb_energy_and_state_type& energy_state_type)
    {
        // Vector with temperature values from 0.01 to max_temperature * 100 K in 0.01 K steps is generated.
        std::vector<double> temp_values{};
        temp_values.reserve(parameter.max_temperature * 100);

        for (uint64_t i = 1; i <= parameter.max_temperature * 100; i++)
        {
            temp_values.push_back(static_cast<double>(i) / 100.0);
        }
        // This function determines the Critical Temperature for a given confidence level.
        for (const auto& temp : temp_values)
        {
            // If the occupation probability of erroneous states exceeds the given threshold...
            if (occupation_probability_gate_based(energy_state_type, temp) > (1 - parameter.confidence_level))
            {
                // The current temperature is stored as Critical Temperature.
                temperature_stats.critical_temperature = temp;

                break;
            }

            if (std::abs(temp - static_cast<double>(parameter.max_temperature)) < 0.001)
            {
                // Maximal temperature is stored as Critical Temperature.
                temperature_stats.critical_temperature = static_cast<double>(parameter.max_temperature);
            }
        }
    }

    /**
     * SiDB cell-level layout.
     */
    Lyt& layout{};
    /**
     * Parameters for the `critical_temperature` algorithm.
     */
    const critical_temperature_params& parameter;
    /**
     * Statistics.
     */
    critical_temperature_stats<Lyt>& temperature_stats;
};

}  // namespace detail

/**
 *
 * This algorithm performs temperature-aware SiDB simulation. It comes in two flavors:
 * gate-based and non-gate based, which can be specified using the `critical_temperature_mode` parameter.
 *
 * For gate-based simulation, the Critical Temperature is defined as follows: The temperature at which the excited
 * charge distributions are populated by more than \f$ 1 - \eta \f$, where \f$ \eta \in [0,1] \f$.
 *
 * For non-gate-based simulation, the Critical Temperature is defined as follows: The temperature at which the
 * erroneous charge distributions are populated by more than \f$ 1 - \eta \f$, where \f$ \eta \in [0,1] \f$ is the
 * confidence level for the presence of a working gate.
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param lyt The layout to simulate.
 * @param params Simulation and physical parameters.
 * @param pst Statistics.
 */

template <typename Lyt>
bool critical_temperature(Lyt& lyt, const critical_temperature_params& params = {},
                          critical_temperature_stats<Lyt>* pst = nullptr)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    critical_temperature_stats<Lyt> st{};

    detail::critical_temperature_impl<Lyt> p{lyt, params, st};

    bool result = false;

    if (params.temperature_mode == critical_temperature_mode::GATE_BASED_SIMULATION)
    {
        result = p.gate_based_simulation();
    }
    else
    {
        result = p.non_gate_based_simulation();
    }

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_CRITICAL_TEMPERATURE_HPP
