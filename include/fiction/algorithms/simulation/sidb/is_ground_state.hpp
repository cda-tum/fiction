//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_IS_GROUND_STATE_HPP
#define FICTION_IS_GROUND_STATE_HPP

#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/quickexact.hpp"
#include "fiction/algorithms/simulation/sidb/quicksim.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"

#include <cmath>

namespace fiction
{

/**
 * This function checks if the ground state is found by the *QuickSim* algorithm.
 *
 * @tparam Lyt Cell-level layout type.
 * @param quicksim_results All found physically valid charge distribution surfaces obtained by the *QuickSim* algorithm.
 * @param exhaustive_results All valid charge distribution surfaces determined by ExGS.
 * @return Returns `true` if the relative difference between the lowest energies of the two sets is less than \f$
 * 0.00001 \f$, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool is_ground_state(const sidb_simulation_result<Lyt>& quicksim_results,
                                   const sidb_simulation_result<Lyt>& exhaustive_results) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    if (exhaustive_results.charge_distributions.empty())
    {
        return false;
    }

    const auto min_energy_exact  = minimum_energy(exhaustive_results.charge_distributions);
    const auto min_energy_new_ap = minimum_energy(quicksim_results.charge_distributions);

    return std::abs(min_energy_exact - min_energy_new_ap) / min_energy_exact < physical_constants::POP_STABILITY_ERR;
}

}  // namespace fiction

#endif  // FICTION_IS_GROUND_STATE_HPP
