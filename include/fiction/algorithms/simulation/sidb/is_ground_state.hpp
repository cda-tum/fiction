//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_IS_GROUND_STATE_HPP
#define FICTION_IS_GROUND_STATE_HPP

#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math_utils.hpp"

#include <cmath>

namespace fiction
{

/**
 * This function checks if the ground state is found by the heuristic.
 *
 * @tparam Lyt Cell-level layout type.
 * @param heuristic_results All found physically valid charge distribution surfaces obtained by a heuristic algorithm.
 * @param exhaustive_results All valid charge distribution surfaces determined by ExGS.
 * @return Returns `true` if the relative difference between the lowest energies of the two sets is less than
 * \f$0.00001\f$, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool is_ground_state(const sidb_simulation_result<Lyt>& heuristic_results,
                                   const sidb_simulation_result<Lyt>& exhaustive_results) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (exhaustive_results.charge_distributions.empty())
    {
        return false;
    }

    const auto ground_state_charge_distributions_exhaustive =
        determine_groundstate_from_simulation_results(exhaustive_results);

    const auto ground_state_charge_distributions_heuristic =
        determine_groundstate_from_simulation_results(heuristic_results);

    if (ground_state_charge_distributions_exhaustive.size() != ground_state_charge_distributions_heuristic.size())
    {
        return false;
    }

    for (const auto& cds_exhaustive : ground_state_charge_distributions_exhaustive)
    {
        for (const auto& cds_heuristic : ground_state_charge_distributions_exhaustive)
        {
            if (cds_exhaustive.get_charge_index_and_base().first != cds_heuristic.get_charge_index_and_base().first)
            {
                return false;
            }
        }
    }

    return true;
}

}  // namespace fiction

#endif  // FICTION_IS_GROUND_STATE_HPP
