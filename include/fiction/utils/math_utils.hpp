//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_MATH_UTILS_HPP
#define FICTION_MATH_UTILS_HPP

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <type_traits>
#include <vector>

#include <combinations.h>

namespace fiction
{

/**
 * Rounds a number to a specified number of decimal places.
 *
 * @tparam T The type of the number to round.
 * @param number The number to round.
 * @param n The number of decimal places to round to.
 * @return The number rounded to n decimal places.
 */
template <typename T>
T round_to_n_decimal_places(const T number, const uint64_t n) noexcept
{
    static_assert(std::is_arithmetic_v<T>, "T is not a number type");

    const auto factor = std::pow(10.0, static_cast<double>(n));
    return static_cast<T>(std::round(static_cast<double>(number) * factor) / factor);
}

/**
 * Takes the absolute value of an integral number if it is signed, and otherwise computes the identity. This avoids a
 * compiler warning when taking the absolute value of an unsigned number.
 * @tparam T The type of the number to take the absolute value of. Must be integral.
 * @param n The number to take the absolute value of.
 * @return |n|.
 */
template <typename T>
T integral_abs(const T n) noexcept
{
    static_assert(std::is_integral_v<T>, "T is not an integral number type");

    if constexpr (std::is_unsigned_v<T>)
    {
        return n;
    }

    return static_cast<T>(std::abs(static_cast<int64_t>(n)));  // needed to solve ambiguity of std::abs
}

/**
 * Calculates the binomial coefficient \f$\binom{n}{k}\f$.
 *
 * @param n The total number of items.
 * @param k The number of items to choose from n.
 * @return The binomial coefficient \f$\binom{n}{k}\f$.
 */
[[nodiscard]] inline uint64_t binomial_coefficient(uint64_t n, uint64_t k) noexcept
{
    if (k > n)
    {
        return 0;
    }
    uint64_t result = 1;
    if (2 * k > n)
    {
        k = n - k;
    }
    for (uint64_t i = 1; i <= k; i++)
    {
        result = result * (n + 1 - i) / i;
    }
    return result;
}

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
determine_all_combinations_of_distributing_k_entities_on_n_positions(const std::size_t k, const std::size_t n)
{
    std::vector<std::vector<std::size_t>> all_combinations{};
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

#endif  // FICTION_MATH_UTILS_HPP
