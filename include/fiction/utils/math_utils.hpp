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
[[nodiscard]] inline T round_to_n_decimal_places(const T number, const uint64_t n) noexcept
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
[[nodiscard]] inline T integral_abs(const T n) noexcept
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
    // Handle a special case
    if (k > n)
    {
        return {};
    }

    std::vector<std::vector<std::size_t>> all_combinations{};

    if (k == 0 || k > n)
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
/**
 * This function computes the Cartesian product of a list of vectors. Each vector in the input list
 * represents a dimension, and the function produces all possible combinations where each combination
 * consists of one element from each dimension vector.
 *
 * @tparam VectorDataType The type of elements in the vectors.
 * @param sets The sets to compute the Cartesian product for. In this implementation, a vector of vectors is utilized
 * for efficiency. Each inner vector represents one dimension. The function generates combinations using one element
 * from each dimension vector.
 * @return A vector of vectors, where each inner vector represents a combination of elements, one from each dimension.
 * The total number of combinations is the product of the sizes of the input vectors.
 */
template <typename VectorDataType>
[[nodiscard]] inline std::vector<std::vector<VectorDataType>>
cartesian_combinations(const std::vector<std::vector<VectorDataType>>& sets) noexcept
{
    std::vector<std::vector<VectorDataType>> all_combinations{{}};

    for (const auto& dimension : sets)
    {
        std::vector<std::vector<VectorDataType>> expanded_products{};
        expanded_products.reserve(all_combinations.size() * dimension.size());

        for (const auto& product : all_combinations)
        {
            for (const auto& element : dimension)
            {
                std::vector<VectorDataType> new_product = product;
                new_product.push_back(element);
                expanded_products.push_back(new_product);
            }
        }
        all_combinations = expanded_products;
    }

    return all_combinations;  // Return the final list of combinations
}

}  // namespace fiction

#endif  // FICTION_MATH_UTILS_HPP
