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
 * @brief Enumerates all ways of distributing k entities over n positions.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/utils/execution_timeout.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <chrono>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <vector>

#include <combinations.h>

namespace fiction::utils::math
{

/**
 * This function generates all possible combinations of distributing k entities
 * onto n positions. Each combination is represented as a vector of indices,
 * where each index indicates the position of an entity.
 *
 * @param k The number of entities to distribute.
 * @param n The number of positions available for distribution.
 * @param deadline Shared execution deadline; unlimited by default.
 * @return A vector of vectors representing all possible combinations of
 *         distributing k entities on n positions.
 * @throws std::length_error if the number of combinations exceeds the vector's capacity.
 * @throws timeout_error if the deadline is reached during enumeration.
 */
[[nodiscard]] inline std::vector<std::vector<std::size_t>>
determine_all_combinations_of_distributing_k_entities_on_n_positions(
    const std::size_t k, const std::size_t n,
    const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::time_point::max())
{
    check_deadline(deadline);
    // Handle a special case
    if (k > n)
    {
        return {};
    }

    std::vector<std::vector<std::size_t>> all_combinations{};

    if (k == 0)
    {
        return all_combinations;
    }

    const auto number_of_combinations = binomial_coefficient(n, k);
    if (number_of_combinations > all_combinations.max_size())
    {
        throw std::length_error{"number of combinations exceeds vector capacity"};
    }
    // A bounded search grows on demand instead of allocating the full search space before its deadline checks.
    if (deadline == std::chrono::steady_clock::time_point::max())
    {
        all_combinations.reserve(static_cast<std::size_t>(number_of_combinations));
    }

    std::vector<std::size_t> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 0);

    combinations::for_each_combination(
        numbers.begin(), numbers.begin() + static_cast<std::vector<std::size_t>::difference_type>(k), numbers.end(),
        [&k, &all_combinations, deadline](const auto begin, const auto end)
        {
            check_deadline(deadline);
            std::vector<std::size_t> combination{};
            combination.reserve(k);

            for (auto it = begin; it != end; ++it)
            {
                combination.push_back(*it);
            }

            all_combinations.push_back(combination);

            return false;  // keep looping
        });

    check_deadline(deadline);
    return all_combinations;
}

}  // namespace fiction::utils::math
