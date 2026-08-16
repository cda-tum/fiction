//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_COMBINATION_UTILS_HPP
#define FICTION_COMBINATION_UTILS_HPP

#include "fiction/utils/math_utils.hpp"

#include <cstddef>
#include <numeric>
#include <vector>

#include <combinations.h>

namespace fiction
{

/**
 * This function generates all possible combinations of distributing k entities
 * onto n positions. Each combination is represented as a vector of indices,
 * where each index indicates the position of an entity.
 *
 * @param k The number of entities to distribute.
 * @param n The number of positions available for distribution.
 * @return A vector of vectors representing all possible combinations of
 *         distributing k entities on n positions.
 */
[[nodiscard]] inline std::vector<std::vector<std::size_t>>
determine_all_combinations_of_distributing_k_entities_on_n_positions(const std::size_t k, const std::size_t n) noexcept
{
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

    all_combinations.reserve(binomial_coefficient(n, k));

    std::vector<std::size_t> numbers(n);
    std::iota(numbers.begin(), numbers.end(), 0);

    combinations::for_each_combination(
        numbers.begin(), numbers.begin() + static_cast<std::vector<std::size_t>::difference_type>(k), numbers.end(),
        [&k, &all_combinations](const auto begin, const auto end)
        {
            std::vector<std::size_t> combination{};
            combination.reserve(k);

            for (auto it = begin; it != end; ++it)
            {
                combination.push_back(*it);
            }

            all_combinations.push_back(combination);

            return false;  // keep looping
        });

    return all_combinations;
}

}  // namespace fiction

#endif  // FICTION_COMBINATION_UTILS_HPP
