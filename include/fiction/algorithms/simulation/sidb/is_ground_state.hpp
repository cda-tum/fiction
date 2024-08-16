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
#include <cassert>

namespace fiction
{

/**
 * This function checks if the elstrostatic ground state of an SiDB layout is found by a heuristic for the physical
 * simulation (e.g., *QuickSim* or *SimAnneal*).
 *
 * @tparam Lyt SiDB cell-level layout type.
 * @param heuristic_results Simulation results obtained from a heuristic physical simulation.
 * @param exact_results Simulation results obtained from an exact physical simulation.
 * @return Returns `true` if the ground state is contained in the simulation result provided by the heuristic physical
 * simulation. `false` otherwise.
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


    assert(ground_state_charge_distributions_heuristic.size() <= ground_state_charge_distributions_exact.size() &&
           "The heuristic results must be less equal than the exact results.");

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
