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
 * @brief Compares two SiDB simulation results for equivalence.
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <set>

namespace fiction::sidb::simulation
{

/**
 * Whether two simulation results of the same layout are equivalent: the same number of distinct charge distributions,
 * and, sorted by charge index, pairwise the same charge states and energies within `ERROR_MARGIN`.
 *
 * @param result1 First result.
 * @param result2 Second result.
 * @return `true` iff both results hold the same charge distributions.
 */
[[nodiscard]] inline bool check_simulation_results_for_equivalence(result result1, result result2)
{
    if (result1.charge_distributions.size() != result2.charge_distributions.size())
    {
        return false;
    }

    if (!result1.charge_distributions.empty() &&
        result1.charge_distributions.front().size() != result2.charge_distributions.front().size())
    {
        return false;
    }

    const auto index = [](const charge_distribution& cd) { return cd.charge_index(3); };

    for (auto* res : {&result1, &result2})
    {
        std::set<uint64_t> unique{};

        for (const auto& cd : res->charge_distributions)
        {
            unique.insert(index(cd));
        }

        if (unique.size() != res->charge_distributions.size())
        {
            return false;
        }

        std::ranges::sort(res->charge_distributions,
                          [&index](const auto& lhs, const auto& rhs) { return index(lhs) < index(rhs); });
    }

    for (std::size_t i = 0; i < result1.charge_distributions.size(); ++i)
    {
        const auto& cd1 = result1.charge_distributions[i];
        const auto& cd2 = result2.charge_distributions[i];

        if (std::abs(cd1.energy() - cd2.energy()) > fiction::utils::math::ERROR_MARGIN || !cd1.same_charge_states(cd2))
        {
            return false;
        }
    }

    return true;
}

}  // namespace fiction::sidb::simulation
