//
// Created by Jan Drewniok on 19.04.23.
//

#ifndef FICTION_ROUND_N_DECIMAL_PLACES_HPP
#define FICTION_ROUND_N_DECIMAL_PLACES_HPP

#include <cmath>

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
T round_n_decimal_places(T number, const uint64_t n)
{
    T factor = std::pow(10, n);
    return std::round(number * factor) / factor;
}

}  // namespace fiction

#endif  // FICTION_ROUND_N_DECIMAL_PLACES_HPP
