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
    const T factor = std::pow(10, n);
    return std::round(number * factor) / factor;
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

}  // namespace fiction

#endif  // FICTION_MATH_UTILS_HPP
