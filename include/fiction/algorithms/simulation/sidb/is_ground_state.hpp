//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_IS_GROUND_STATE_HPP
#define FICTION_IS_GROUND_STATE_HPP

#include "fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/traits.hpp"

#include <cstdint>
#include <unordered_set>

namespace fiction
{

/**
 * This function checks if the ground state is found by the heuristic.
 *
 * @tparam Lyt Cell-level layout type.
 * @param heuristic_results All found physically valid charge distribution surfaces obtained by a heuristic algorithm.
 * @param exact_results All valid charge distribution surfaces determined by ExGS.
 * @return Returns `true` if the relative difference between the lowest energies of the two sets is less than
 * \f$0.00001\f$, `false` otherwise.
 */
template <typename Lyt>
[[nodiscard]] bool is_ground_state(const sidb_simulation_result<Lyt>& heuristic_results,
                                   const sidb_simulation_result<Lyt>& exact_results) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (exact_results.charge_distributions.empty())
    {
        return false;
    }

    const auto ground_state_charge_distributions_exact = determine_groundstate_from_simulation_results(exact_results);

    const auto ground_state_charge_distributions_heuristic =
        determine_groundstate_from_simulation_results(heuristic_results);

    if (ground_state_charge_distributions_exact.size() != ground_state_charge_distributions_heuristic.size())
    {
        return false;
    }

    std::unordered_set<uint64_t> indices_ground_state_heuristic{};

    // Collect all charge indices of the ground states simulated by the heuristic.
    for (const auto& cds : ground_state_charge_distributions_heuristic)
    {
        indices_ground_state_heuristic.insert(cds.get_charge_index_and_base().first);
    }

    // Check if the heuristic has found all ground states.
    for (const auto& cds : ground_state_charge_distributions_exact)
    {
        if (indices_ground_state_heuristic.find(cds.get_charge_index_and_base().first) ==
            indices_ground_state_heuristic.cend())
        {
            return false;
        }
    }

    return true;
}

}  // namespace fiction

#endif  // FICTION_IS_GROUND_STATE_HPP
