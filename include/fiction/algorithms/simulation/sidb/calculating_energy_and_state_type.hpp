//
// Created by Jan Drewniok on 20.04.23.
//

#ifndef FICTION_CALCULATING_ENERGY_AND_STATE_TYPE_HPP
#define FICTION_CALCULATING_ENERGY_AND_STATE_TYPE_HPP

#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/utils/math_utils.hpp"

#include <cstdint>
#include <map>

namespace fiction
{

using energy_and_state_type = std::vector<std::pair<double, bool>>;

/**
 * This function takes in a map of energy (of a physically valid charge distribution) and the corresponding degeneracy.
 * For each charge distribution, it is determined if the wanted logic is still fulfilled, which is then returned. Thus,
 * information about which charge distributions affect the accuracy of a gate when it is populated.
 *
 * @tparam Lyt Cell-level layout type.
 * @param energy_distribution The energy distribution of all physically valid charge layouts (can be calculated with
 * energy_distribution.hpp).
 * @param output_cells Cells where the output is located.
 * @param output_bits input bits of the given gate (e.g. "00",... for AND or "0",... for a wire)
 * @return energy_and_state_type A vector of pairs where the first entry is the energy of a charge distribution layout
 * and the second entry indicating whether the corresponding charge distribution satisfies the logic.
 */
template <typename Lyt>
[[nodiscard]] energy_and_state_type
calculating_energy_and_state_type(const std::map<double, uint64_t>&                    energy_distribution,
                                  const std::vector<charge_distribution_surface<Lyt>>& valid_lyts,
                                  const std::vector<typename Lyt::cell>&               output_cells,
                                  const std::string&                                   output_bits) noexcept

{
    energy_and_state_type energy_transparent_erroneous{};

    for (const auto& [energy, occurrence] : energy_distribution)
    {
        for (const auto& valid_layout : valid_lyts)
        {
            if (round_to_n_decimal_places(valid_layout.get_system_energy(), 6) == round_to_n_decimal_places(energy, 6))
            {
                std::stringstream charge{};  // The output is collected as a string. For example: "10",
                                             // "1", etc. (depending on the number of outputs).

                for (const auto& cell : output_cells)
                {
                    charge << std::to_string(-charge_state_to_sign(valid_layout.get_charge_state(cell)));
                }

                bool transparent = false;

                if (charge.str() == output_bits)
                {
                    transparent = true;  // The output represents the correct output. Hence, state is called
                                         // transparent.
                }
                energy_transparent_erroneous.emplace_back(energy, transparent);
            }
        }
    }

    return energy_transparent_erroneous;
}

}  // namespace fiction

#endif  // FICTION_CALCULATING_ENERGY_AND_STATE_TYPE_HPP
