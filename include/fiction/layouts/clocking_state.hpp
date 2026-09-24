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
 * @brief Clocking and synchronization state shared by gate and cell layouts.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/clocking_scheme.hpp"

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <utility>

namespace fiction::layouts::clocking
{
/**
 * @brief Clock numbers and synchronization delays, independent of layout geometry.
 * Copies own independent schemes and synchronization maps.
 * @tparam Coordinate Coordinate identifying a clock zone.
 */
template <typename Coordinate>
class state
{
  public:
    /** @brief Coordinate identifying a clock zone. */
    using clock_zone = Coordinate;
    /** @brief Clocking scheme over tile positions. */
    using clocking_scheme_t = scheme;
    /** @brief Clock phase index. */
    using clock_number_t = typename clocking_scheme_t::clock_number;
    /** @brief Hold-phase extension in full clock cycles. */
    using sync_elem_t = uint8_t;
    /** @brief Creates state with the given scheme. @param s Initial scheme. */
    explicit state(clocking_scheme_t s) : clocking{std::move(s)} {}
    /**
     * Replaces the stored clocking scheme with the provided one.
     *
     * @param scheme New clocking scheme.
     */
    void replace_clocking_scheme(const clocking_scheme_t& scheme) noexcept
    {
        clocking = scheme;
    }
    /**
     * Overrides the clock number of a clock zone in the stored scheme. A clock zone spans every layer, so the override
     * ignores the z-coordinate of `cz`.
     *
     * @param cz Clock zone to override.
     * @param cn New clock number for `cz`.
     */
    void assign_clock_number(const clock_zone& cz, const clock_number_t cn) noexcept
    {
        clocking.override_clock_number(static_cast<int64_t>(cz.x), static_cast<int64_t>(cz.y), cn);
    }
    /**
     * Returns the clock number of a clock zone. A clock zone spans every layer, so the lookup ignores the z-coordinate
     * of `cz`.
     *
     * @param cz Clock zone.
     * @return Clock number of `cz`.
     */
    [[nodiscard]] clock_number_t get_clock_number(const clock_zone& cz) const noexcept
    {
        return clocking(static_cast<int64_t>(cz.x), static_cast<int64_t>(cz.y));
    }
    /**
     * Returns the number of clock phases in the layout. Each clock cycle is divided into n phases. In QCA, the number
     * of phases is usually 4. In iNML it is 3. Clocking schemes support 3 or 4 phases.
     *
     * @return The number of different clock signals in the layout.
     */
    [[nodiscard]] clock_number_t num_clocks() const noexcept
    {
        return clocking.num_clocks();
    }
    /**
     * Returns whether the layout is clocked by a regular clocking scheme with no overwritten zones.
     *
     * @return `true` iff the layout is clocked by a regular scheme and no zones have been overwritten.
     */
    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return clocking.is_regular();
    }
    /**
     * Compares the stored clocking scheme against the provided name. Predefined names are constants in
     * `fiction::layouts::clocking`.
     *
     * @param name Clocking scheme name.
     * @return `true` iff the layout is clocked by a clocking scheme of name `name`.
     */
    [[nodiscard]] bool is_clocking_scheme(const std::string_view& name) const noexcept
    {
        return clocking.name() == name;
    }
    /**
     * Returns a copy of the stored clocking scheme object.
     *
     * @return A copy of the stored clocking scheme object.
     */
    [[nodiscard]] clocking_scheme_t get_clocking_scheme() const noexcept
    {
        return clocking;
    }
    /**
     * Evaluates whether clock zone `cz2` feeds information to clock zone `cz1`, i.e., whether `cz2` is clocked with a
     * clock number that is lower by 1 modulo `num_clocks()`, or either zone is a synchronization element.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is lower by 1.
     * @return `true` iff `cz2` can feed information to `cz1`.
     */
    [[nodiscard]] bool is_incoming_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return static_cast<clock_number_t>((get_clock_number(cz2) + static_cast<clock_number_t>(1)) % num_clocks()) ==
                   get_clock_number(cz1) ||
               is_synchronization_element(cz1) || is_synchronization_element(cz2);
    }
    /**
     * Evaluates whether clock zone `cz2` accepts information from clock zone `cz1`, i.e., whether `cz2` is clocked with
     * a clock number that is higher by 1 modulo `num_clocks()`, or either zone is a synchronization element.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is higher by 1.
     * @return `true` iff `cz2` can accept information from `cz1`.
     */
    [[nodiscard]] bool is_outgoing_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return static_cast<clock_number_t>((get_clock_number(cz1) + static_cast<clock_number_t>(1)) % num_clocks()) ==
                   get_clock_number(cz2) ||
               is_synchronization_element(cz1) || is_synchronization_element(cz2);
    }

    /**
     * Assigns a synchronization element to the provided clock zone.
     *
     * @param cz Clock zone to turn into a synchronization element.
     * @param se Number of full clock cycles to extend `cz`'s Hold phase by. If this value is 0, `cz` is turned back
     * into a normal clock zone.
     */
    void assign_synchronization_element(const clock_zone& cz, const sync_elem_t se) noexcept
    {
        if (se == sync_elem_t{0})
        {
            synchronization.erase(cz);
        }
        else
        {
            synchronization[cz] = se;
        }
    }
    /**
     * Check whether the provided clock zone is a synchronization element.
     *
     * @param cz Clock zone to check.
     * @return `true` iff `cz` is a synchronization element.
     */
    [[nodiscard]] bool is_synchronization_element(const clock_zone& cz) const noexcept
    {
        return synchronization.contains(cz);
    }
    /**
     * Returns the Hold phase extension in clock cycles of clock zone `cz`.
     *
     * @param cz Clock zone to check.
     * @return Synchronization element value, i.e., Hold phase extension, of clock zone `cz`.
     */
    [[nodiscard]] sync_elem_t get_synchronization_element(const clock_zone& cz) const noexcept
    {
        if (const auto it = synchronization.find(cz); it != synchronization.end())
        {
            return it->second;
        }

        return 0;
    }

    /** @brief Counts zones with a nonzero Hold-phase extension. @return Synchronization element count. */
    [[nodiscard]] uint32_t num_se() const noexcept
    {
        return static_cast<uint32_t>(synchronization.size());
    }

  private:
    /**
     * @brief Scheme and manually overridden clock numbers.
     */
    clocking_scheme_t clocking;
    /** @brief Nonzero synchronization delays indexed by coordinate. */
    std::unordered_map<Coordinate, sync_elem_t> synchronization{};
};
}  // namespace fiction::layouts::clocking
