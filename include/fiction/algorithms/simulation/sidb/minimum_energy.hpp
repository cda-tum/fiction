//
// Created by Jan Drewniok on 18.01.23.
//

#ifndef FICTION_MINIMUM_ENERGY_HPP
#define FICTION_MINIMUM_ENERGY_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <iterator>
#include <limits>

namespace fiction
{

/**
 * Computes the minimum energy of a range of `charge_distribution_surface` objects. If the range is empty, infinity is
 * returned.
 *
 * @tparam InputIt Must meet the requirements of `LegacyInputIterator`.
 * @param first Begin of the range to examime.
 * @param last End of the range to examine.
 * @return Value of the minimum energy found in the input range (unit: eV), or infinity if the range is empty.
 */
template <typename InputIt>
[[nodiscard]] double minimum_energy(const InputIt first, const InputIt last) noexcept
{
    static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>,
                  "InputIt must meet the requirements of LegacyInputIterator");
    static_assert(is_charge_distribution_surface_v<typename std::iterator_traits<InputIt>::value_type>,
                  "Range must be of charge_distribution_surface objects");

    if (first != last)
    {
        return minimum_energy_distribution(first, last)->get_system_energy();
    }

    return std::numeric_limits<double>::infinity();
}
/**
 * Returns an iterator to the charge distribution of minimum energy contained in a range of
 * `charge_distribution_surface` objects. If the range is empty, `last` is returned.
 *
 * @tparam InputIt Must meet the requirements of `LegacyInputIterator`.
 * @param first Begin of the range to examime.
 * @param last End of the range to examine.
 * @return Iterator to the minimum energy charge distribution found in the input range, or `last` if the range is empty.
 */
template <typename InputIt>
[[nodiscard]] InputIt minimum_energy_distribution(const InputIt first, const InputIt last) noexcept
{
    static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>,
                  "InputIt must meet the requirements of LegacyInputIterator");
    static_assert(is_charge_distribution_surface_v<typename std::iterator_traits<InputIt>::value_type>,
                  "Range must be of charge_distribution_surface objects");

    return std::min_element(first, last,
                            [](const auto& cds1, const auto& cds2)
                            { return cds1.get_system_energy() < cds2.get_system_energy(); });
}

}  // namespace fiction

#endif  // FICTION_MINIMUM_ENERGY_HPP
