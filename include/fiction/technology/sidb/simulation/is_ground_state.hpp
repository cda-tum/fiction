/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Checks a heuristic simulation result against an exact one.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"

#include <cassert>
#include <cstdint>
#include <unordered_set>

namespace fiction::sidb::simulation
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
[[nodiscard]] bool is_ground_state(const sidb::simulation::legacy_result<Lyt>& heuristic_results,
                                   const sidb::simulation::legacy_result<Lyt>& exact_results) noexcept
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");

    if (exact_results.charge_distributions.empty())
    {
        return false;
    }

    const auto ground_state_charge_distributions_exact = exact_results.groundstates();

    const auto ground_state_charge_distributions_heuristic = heuristic_results.groundstates();

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

/**
 * Whether a heuristic simulation found every ground state an exact simulation found: the two results have the same
 * number of ground states and every exact ground state appears among the heuristic ones.
 *
 * @param heuristic_results Result of a heuristic simulation.
 * @param exact_results Result of an exact simulation of the same layout.
 * @return `true` iff the heuristic result contains every ground state of the exact one.
 */
[[nodiscard]] inline bool is_ground_state(const result& heuristic_results, const result& exact_results) noexcept
{
    if (exact_results.charge_distributions.empty())
    {
        return false;
    }

    const auto exact     = exact_results.groundstates();
    const auto heuristic = heuristic_results.groundstates();

    assert(heuristic.size() <= exact.size() && "The heuristic results must be less equal than the exact results.");

    if (exact.size() != heuristic.size())
    {
        return false;
    }

    return std::ranges::all_of(
        exact, [&heuristic](const auto& e)
        { return std::ranges::any_of(heuristic, [&e](const auto& h) { return h.same_charge_states(e); }); });
}

}  // namespace fiction::sidb::simulation
