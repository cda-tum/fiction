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
 * @brief Finds the charge distribution of least energy in a range.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/traits.hpp"

#include <algorithm>
#include <iterator>
#include <limits>
#include <type_traits>

namespace fiction::sidb::simulation::analysis
{

namespace detail
{

/**
 * The energy of a `charge_distribution` or of a `charge_distribution_surface`.
 *
 * @tparam T `charge_distribution` or `charge_distribution_surface`.
 * @param cd The distribution.
 * @return Its electrostatic potential energy (unit: eV).
 */
template <typename T>
[[nodiscard]] double energy_of(const T& cd) noexcept
{
    if constexpr (requires { cd.energy(); })
    {
        return cd.energy();
    }
    else
    {
        return cd.get_electrostatic_potential_energy();
    }
}

}  // namespace detail

/**
 * Returns an iterator to the charge distribution of minimum energy contained in a range of
 * `charge_distribution_surface` objects. If the range is empty, `last` is returned.
 *
 * @tparam InputIt Must meet the requirements of `LegacyInputIterator`.
 * @param first Begin of the range to examine.
 * @param last End of the range to examine.
 * @return Iterator to the minimum energy charge distribution found in the input range, or `last` if the range is empty.
 */
template <typename InputIt>
[[nodiscard]] InputIt minimum_energy_distribution(const InputIt first, const InputIt last) noexcept
{
    static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>,
                  "InputIt must meet the requirements of LegacyInputIterator");

    return std::min_element(first, last, [](const auto& cd1, const auto& cd2)
                            { return detail::energy_of(cd1) < detail::energy_of(cd2); });
}

/**
 * Computes the minimum energy of a range of `charge_distribution_surface` objects. If the range is empty, infinity is
 * returned to indicate no valid energy value exists.
 *
 * @tparam InputIt Must meet the requirements of `LegacyInputIterator`.
 * @param first Begin of the range to examine.
 * @param last End of the range to examine.
 * @return Value of the minimum energy found in the input range (unit: eV), or `std::numeric_limits<double>::infinity()`
 * if the range is empty.
 */
template <typename InputIt>
[[nodiscard]] double minimum_energy(const InputIt first, const InputIt last) noexcept
{
    static_assert(std::is_base_of_v<std::input_iterator_tag, typename std::iterator_traits<InputIt>::iterator_category>,
                  "InputIt must meet the requirements of LegacyInputIterator");

    if (first != last)
    {
        return detail::energy_of(*minimum_energy_distribution(first, last));
    }

    return std::numeric_limits<double>::infinity();
}

}  // namespace fiction::sidb::simulation::analysis
