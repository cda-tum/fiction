//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_MATH_UTILS_HPP
#define FICTION_MATH_UTILS_HPP

#include "fiction/utils/units_utils.hpp"
#include "units.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <type_traits>

namespace fiction
{

/**
 * Rounds a number or unit (allowed are: temperature, length, voltage, and energy) to a specified number of decimal
 * places.
 *
 * @tparam T The type of the number or unit to round.
 * @param number The number or unit to round.
 * @param n The number or unit of decimal places to round to.
 * @return The number or unit rounded to n decimal places.
 */
template <typename T>
T round_to_n_decimal_places(const T number, const uint64_t n) noexcept
{
    static_assert(std::is_arithmetic_v<T> || units::traits::is_temperature_unit<T>::value ||
                      units::traits::is_length_unit<T>::value || units::traits::is_voltage_unit<T>::value ||
                      units::traits::is_energy_unit<T>::value,
                  "T is neither a number type nor a unit type");

    if constexpr (std::is_arithmetic_v<T>)
    {
        const auto factor = std::pow(10.0, static_cast<double>(n));
        return static_cast<T>(std::round(static_cast<double>(number) * factor) / static_cast<double>(factor));
    }
    else if constexpr (units::traits::is_temperature_unit<T>::value || units::traits::is_length_unit<T>::value ||
                       units::traits::is_voltage_unit<T>::value || units::traits::is_energy_unit<T>::value)
    {
        const double factor = std::pow(10.0, n);
        return static_cast<T>(std::round(static_cast<double>(number.value()) * factor) / factor);
    }
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
