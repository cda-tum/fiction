//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_MATH_UTILS_HPP
#define FICTION_MATH_UTILS_HPP

#include <cmath>
#include <cstdint>

namespace fiction
{

/**
 * Rounds a number to a specified number of decimal places.
 *
 * @tparam T the type of the number to round.
 * @param number the number to round.
 * @param n the number of decimal places to round to.
 * @return the number rounded to n decimal places.
 */
template <typename T>
T round_to_n_decimal_places(const T number, const uint64_t n)
{
    static_assert(std::is_arithmetic_v<T>, "T is not a number type");
    const T factor = std::pow(10, n);
    return std::round(number * factor) / factor;
}

}  // namespace fiction

#endif  // FICTION_MATH_UTILS_HPP
