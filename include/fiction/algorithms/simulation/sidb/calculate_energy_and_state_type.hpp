//
// Created by Jan Drewniok on 20.04.23.
//

#ifndef FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
#define FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP

#include "fiction/algorithms/simulation/sidb/energy_distribution.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/utils/math_utils.hpp"

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace fiction
{
/**
 *  Types of charge distributions.
 */
enum class sidb_charge_distribution_type
{
    ERRONEOUS,   // The output SiDB (SiDBs) of the charge distribution does NOT match the truth table entry.
    TRANSPARENT  // The output SiDB (SiDBs) of the charge distribution matches the truth table entry.
};
/**
 *  Data type to collect electrostatic potential energies of charge distributions with corresponding state types (i.e.,
 * transparent, erroneous).
 */
using sidb_energy_and_state_type = std::vector<std::pair<double, sidb_charge_distribution_type>>;

/**
 * This function takes in an SiDB energy distribution. For each charge distribution, the state type is determined (i.e.
 * erroneous, transparent).
 *
 * @tparam Lyt SiDB cell-level layout type (representing a gate).
 * @param energy_distribution Energy distribution.
 * @param output_cells SiDBs in the layout from which the output is read.
 * @param output_bits Truth table entry for a given input (e.g. 0 for AND (00 as input) or 1 for a wire (input 1)).
 * @return sidb_energy_and_state_type Electrostatic potential energy of all charge distributions with state type.
 */
template <typename Lyt>
[[nodiscard]] sidb_energy_and_state_type
calculate_energy_and_state_type(const sidb_energy_distribution&                      energy_distribution,
                                const std::vector<charge_distribution_surface<Lyt>>& valid_lyts,
                                const std::vector<typename Lyt::cell>&               output_cells,
                                const std::vector<bool>&                             output_bits) noexcept

{
    assert(!output_cells.empty() && "No output cell provided.");
    assert(!output_bits.empty() && "No output bits provided.");

    sidb_energy_and_state_type energy_transparent_erroneous{};

    for (const auto& [energy, occurrence] : energy_distribution)
    {
        for (const auto& valid_layout : valid_lyts)
        {
            if (round_to_n_decimal_places(valid_layout.get_system_energy(), 6) == round_to_n_decimal_places(energy, 6))
            {
                // collect the charge state of the output SiDBs.
                std::vector<sidb_charge_state> charge_states(output_cells.size());
                std::transform(output_cells.begin(), output_cells.end(), charge_states.begin(),
                               [&](const auto& cell) { return valid_layout.get_charge_state(cell); });

                // Convert the charge states of the output SiDBs to bits (-1 -> 1, 0 -> 0).
                std::vector<bool> charge(charge_states.size());
                std::transform(charge_states.begin(), charge_states.end(), charge.begin(),
                               [](const auto& state) { return static_cast<bool>(-charge_state_to_sign(state)); });

                sidb_charge_distribution_type state_type = sidb_charge_distribution_type::ERRONEOUS;

                if (charge == output_bits)
                {
                    state_type =
                        sidb_charge_distribution_type::TRANSPARENT;  // The output represents the correct output. Hence,
                                                                     // state is called sidb_charge_distribution_type.
                }
                energy_transparent_erroneous.emplace_back(energy, state_type);
            }
        }
    }
    return energy_transparent_erroneous;
}

}  // namespace fiction

#endif  // FICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
