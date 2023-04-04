//
// Created by Jan Drewniok on 06.02.23.
//

#ifndef FICTION_CRITICAL_TEMPERATURE_HPP
#define FICTION_CRITICAL_TEMPERATURE_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/occupation_function.hpp"
#include "fiction/algorithms/simulation/sidb/sort_function.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/gate_logic_map.hpp"
#include "fiction/utils/hash.hpp"

#include <fmt/format.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <ostream>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * This struct stores the result of the temperature simulation.
 *
 * @paramt Lyt Cell-level layout type.
 */
template <typename Lyt>
struct critical_temperature_stats
{
    /**
     * Critical temperature of the given layout.
     */
    double critical_temperature{};
    /**
     * Number of physically valid charge configurations.
     */
    uint64_t num_valid_lyt{};
    /**
     * Energy difference between the ground state and the first (erroneous) excited state.
     */
    double e_min_ground_state_error = std::numeric_limits<double>::max();
    /**
     * Prints the simulation results to the given output stream.
     *
     * @param out Output stream.
     */
    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("critical temperature  = {:.2f} K\n", critical_temperature);

        if (num_valid_lyt != 0)
        {
            out << fmt::format("'#valid': {} | E_min_g,(err)exc.: {}\n", num_valid_lyt, e_min_ground_state_error);
        }
        else
        {
            out << "no state found | if two-state simulation was used, try re-running with three states\n";
        }

        out << "_____________________________________________________" << std::endl;
    }
};

/**
 * Computes the critical temperature of a given SiDB layout. If a gate is simulated, the temperature that results in a
 * population of erroneous excited states with a probability greater than 1 − η, where η is the confidence level for the
 * presence of a working gate, is called the *Critical Temperature (CT)* of the gate. In the case of an arbitrary layout
 * (flag is set to false), temperatures above CT lead to a population of ground states smaller than η.
 *
 * @tparam Lyt Cell-level layout type.
 * @param lyt The layout to simulate.
 * @param erroneous_excited Flag to indicate that the critical temperature is determined for a logic gate. `True` is
 * used (recommended) for gates. `False` is required for arbitrary layouts with no underlying logic.
 * @param ps Physical parameters. They are material-specific and may vary from experiment to experiment.
 * @param pst Statistics. They store the simulation results.
 * @param confidence_level Confidence level for the presence of a working gate.
 * @param max_temperature The maximal critical temperature is set to 400 K by default.
 * @param gate Gate (e.g. or, and, nor, ...), must be available in gate_logic_map.
 * @param input_bits Input configuration of the given gate (e.g. `00` or `01` etc. for 2-input gate).
 * @return The Critical Temperature is returned. 0 can only be returned if logic is considered (flag is set): It occurs
 * when either no charge distribution satisfies logic, or at least not the ground state as it should be. Changing the
 * physical parameter µ_ might help.
 */
template <typename Lyt>
void critical_temperature(const Lyt& lyt, const bool erroneous_excited = true,
                          const sidb_simulation_parameters& params = sidb_simulation_parameters{},
                          critical_temperature_stats<Lyt>* pst = nullptr, const double confidence_level = 0.99,
                          const uint64_t max_temperature = 400, const std::string& gate = " ",
                          const std::string& input_bits = " ")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    critical_temperature_stats<Lyt> ct_stats{};

    if (lyt.is_empty())
    {
        return;
    }

    // All physically valid charge configurations are determined for the given layout (exhaustive ground state
    // simulation is used to provide 100 % accuracy for the critical temperature).
    exgs_stats<Lyt> stats_exhaustive{};
    exhaustive_ground_state_simulation(lyt, params, &stats_exhaustive);

    // The number of physically valid charge configurations is stored.
    ct_stats.num_valid_lyt = stats_exhaustive.valid_lyts.size();

    // If the layout consists of only one SiDB, the maximum temperature is returned as the critical temperature.
    if (lyt.num_cells() == 1u)
    {
        ct_stats.critical_temperature = static_cast<double>(max_temperature);
    }

    else if (lyt.num_cells() > 1)
    {
        // Vector with temperature values from 0.01 to 400 K in 0.01 K steps is generated.
        std::vector<double> temp_values{};
        temp_values.reserve(max_temperature * 100);

        for (uint64_t i = 1; i <= max_temperature * 100; i++)
        {
            temp_values.push_back(static_cast<double>(i) / 100.0);
        }

        // All cells of the given layout are collected.
        std::vector<typename Lyt::cell> all_cells{};
        all_cells.reserve(lyt.num_cells());

        lyt.foreach_cell([&all_cells](const auto& c) { all_cells.push_back(c); });

        // The cells are sorted according to local_sort_sidb_cells.
        // The goal is to sort the cells from left to right and top to bottom.
        std::sort(all_cells.begin(), all_cells.end(), local_sort_sidb_cells<Lyt>);

        // The indices of the output cells are collected as a vector. Depending on the specific gate, there is/are
        // one or two outputs.

        // The energy distribution of the physically valid charge configurations for the given layout is determined.
        const auto distribution = energy_distribution(stats_exhaustive.valid_lyts);

        // This scope is only executed if logic is considered, i.e. flag is set to true.
        if (erroneous_excited)
        {
            std::vector<int64_t> output_bits_index{};

            if (gate_logic::GATES_IN_OUT_NUMBER.at(gate).at("out") == "2")
            {
                output_bits_index = {-4, -3};  // double wire, cx, fo2, etc.
            }
            else
            {
                output_bits_index = {-2};  // and, or, xnor, nor, nand, etc.
            }

            // Output cell(s) is/are collected.
            std::vector<typename Lyt::cell> output_cells;
            output_cells.reserve(output_bits_index.size());

            std::transform(output_bits_index.begin(), output_bits_index.end(), std::back_inserter(output_cells),
                           [&all_cells](const auto& index) { return *(all_cells.end() + index); });

            // A label that indicates whether the state still calculates the correct result is added.
            std::vector<std::pair<double, bool>> energy_transparent_erroneous{};

            for (const auto& [energy, occurrence] : distribution)
            {
                for (const auto& layout : stats_exhaustive.valid_lyts)
                {
                    if ((std::round(layout.get_system_energy() * 1'000'000)) / 1'000'000 ==
                        (std::round(energy * 1'000'000)) / 1'000'000)
                    {
                        std::stringstream charge{};  // The output is collected as a string. For example: "10",
                                                     // "1", etc. (depending on the number of outputs).

                        for (const auto& cell : output_cells)
                        {
                            charge << std::to_string(-charge_state_to_sign(layout.get_charge_state(cell)));
                        }

                        bool transparent = false;

                        if (charge.str() == gate_logic::GATE_TRUTH_TABLE.at(gate).at(input_bits))
                        {
                            transparent = true;  // The output represents the correct output. Hence, state is called
                                                 // transparent.
                        }

                        energy_transparent_erroneous.emplace_back(energy, transparent);
                    }
                }
            }

            const auto min_energy = energy_transparent_erroneous.begin()->first;

            bool ground_state_is_transparent = false;

            // The energy difference between the ground state and the first erroneous state is determined.
            for (const auto& [energy, trans_error] : energy_transparent_erroneous)
            {
                // Check if at least one ground state exists which fulfills the logic (transparent).
                if ((energy == min_energy) && trans_error)
                {
                    ground_state_is_transparent = true;
                }

                if (!trans_error && (energy > min_energy) && ground_state_is_transparent)
                {
                    // The energy difference is stored in meV.
                    ct_stats.e_min_ground_state_error = (energy - min_energy) * 1000;

                    break;
                }
            }

            if (ground_state_is_transparent)
            {
                // This function determines the critical temperature (CT) for a given confidence level.
                for (const auto& temp : temp_values)
                {
                    // if the occupation probability of erroneous states exceeds the given threshold...
                    if (occupation_probability(energy_transparent_erroneous, temp, erroneous_excited) >
                        (1 - confidence_level))
                    {
                        // ...the current temperature is stored as critical temperature
                        ct_stats.critical_temperature = temp;

                        break;
                    }

                    if (std::abs(temp - static_cast<double>(max_temperature)) < 0.001)
                    {
                        // Maximal temperature is stored as critical temperature.
                        ct_stats.critical_temperature = static_cast<double>(max_temperature);
                    }
                }
            }
            else
            {
                ct_stats.critical_temperature = 0.0;  // If no ground state fulfills the logic, the critical temperature
                                                      // is zero. May be worth it to change µ_.
            }
        }
        else  // This scope is executed if an arbitrary SiDB layout is simulated with no underlying logic (flag has
              // to be set to `false`). The population of the first excited states is of interest.
        {
            std::vector<std::pair<double, bool>> energy_transparent_erroneous{};

            // if there is more than one metastable state
            if (distribution.size() > 1)
            {
                const auto ground_state_energy        = distribution.cbegin()->first;
                const auto first_excited_state_energy = std::next(distribution.cbegin())->first;

                // The energy difference between the first excited and the ground state in meV.
                ct_stats.e_min_ground_state_error = (first_excited_state_energy - ground_state_energy) * 1000;
            }

            for (const auto& [energy, occurrence] : distribution)
            {
                for (const auto& layout : stats_exhaustive.valid_lyts)
                {
                    if ((std::round(layout.get_system_energy() * 1'000'000)) / 1'000'000 ==
                        (std::round(energy * 1'000'000)) / 1'000'000)
                    {
                        bool transparent = false;
                        energy_transparent_erroneous.emplace_back(energy, transparent);
                    }
                }
            }

            // This function determines the critical temperature (CT) for a given confidence level
            for (const auto& temp : temp_values)
            {
                // if the occupation probability of excited states exceeds the given threshold...
                if (occupation_probability(energy_transparent_erroneous, temp, erroneous_excited) >
                    (1 - confidence_level))
                {
                    // ...the current temperature is stored as the critical temperature
                    ct_stats.critical_temperature = temp;

                    break;
                }

                if (std::abs(temp - static_cast<double>(max_temperature)) < 0.001)
                {
                    // maximal temperature is stored as the critical temperature
                    ct_stats.critical_temperature = static_cast<double>(max_temperature);
                }
            }
        }
    }

    if (pst)
    {
        *pst = ct_stats;
    }
}

}  // namespace fiction

#endif  // FICTION_CRITICAL_TEMPERATURE_HPP
