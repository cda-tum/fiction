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

#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <unordered_set>

namespace fiction::sidb::simulation
{

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
