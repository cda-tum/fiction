//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_MATH_UTILS_HPP
#define FICTION_MATH_UTILS_HPP

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

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
 * Calculates the binomial coefficient C(n, k).
 *
 * @param n The total number of items.
 * @param k The number of items to choose from n.
 * @return The binomial coefficient C(n, k).
 */
uint64_t binomial_coefficient(uint64_t n, uint64_t k)
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
 * This function generates all combinations of 'k' unique elements of {0,1,2,...,n-1}.
 *
 * @param n The total number of elements.
 * @param k The number of elements in each combination.
 * @return A vector of vectors representing all combinations.
 */
void generate_unsigned_integer_combinations_helper(std::vector<std::vector<uint64_t>>& combinations,
                                                   std::vector<uint64_t>&              current_combination,
                                                   const uint64_t total_elements, const uint64_t current_index,
                                                   const uint64_t remaining_elements)
{
    // If we have selected 'k' elements, add the current combination to the result.
    if (remaining_elements == 0)
    {
        combinations.push_back(current_combination);
        return;
    }

    // Iterate through the remaining elements.
    for (uint64_t i = current_index; i <= total_elements - remaining_elements; ++i)
    {
        current_combination.push_back(i);
        generate_unsigned_integer_combinations_helper(combinations, current_combination, total_elements, i + 1,
                                                      remaining_elements - 1);
        current_combination.pop_back();  // Remove the last added element for backtracking.
    }
}

/**
 * Generates all combinations of 'k' unique elements of {0,1,2,...,n-1}. For example: n = 3 and k = 2, the combinations
 * would be {0,1}, {0,2}, {1,2}.
 *
 * @param n The total number of elements.
 * @param k The number of elements in each combination.
 * @return A vector of vectors representing all combinations.
 */
std::vector<std::vector<uint64_t>> generate_unsigned_integer_combinations(const uint64_t k, const uint64_t n)
{
    std::vector<std::vector<uint64_t>> combinations;
    combinations.reserve(binomial_coefficient(n, k));
    std::vector<uint64_t> current_combination;
    current_combination.reserve(k);
    generate_unsigned_integer_combinations_helper(combinations, current_combination, n, 0, k);
    return combinations;
}

}  // namespace fiction

#endif  // FICTION_MATH_UTILS_HPP
