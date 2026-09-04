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
 * @brief The charge states of the SiDBs of one layout plus the electrostatic potential energy they yield.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/utils/stl/hash.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace fiction::sidb
{

/**
 * A charge distribution assigns one charge state to every SiDB of a layout, in the layout's canonical raster order
 * (`layout::sidbs()`), and carries the electrostatic potential energy of that assignment. The site list is shared by
 * every distribution of one simulation result, so a distribution costs one byte per SiDB.
 */
class charge_distribution
{
  public:
    /**
     * The shared list of sites a distribution refers to, in raster order.
     */
    using site_list = std::shared_ptr<const std::vector<lattice_site>>;
    /**
     * Creates a distribution over no SiDBs.
     */
    charge_distribution() = default;
    /**
     * Creates a distribution over the SiDBs of a layout with every SiDB in one charge state.
     *
     * @param lyt Layout whose SiDBs the distribution covers.
     * @param cs Charge state of every SiDB.
     */
    explicit charge_distribution(const layout& lyt, const model::charge_state cs = model::charge_state::NEGATIVE) :
            site_storage{std::make_shared<const std::vector<lattice_site>>(lyt.sidbs())},
            charge_state_values(site_storage->size(), cs)
    {}
    /**
     * Creates a distribution over a shared site list with every SiDB in one charge state.
     *
     * @param sites Sites in raster order.
     * @param cs Charge state of every SiDB.
     */
    explicit charge_distribution(site_list sites, const model::charge_state cs = model::charge_state::NEGATIVE) :
            site_storage{std::move(sites)},
            charge_state_values(site_storage ? site_storage->size() : 0, cs)
    {}
    /**
     * Creates a distribution over a shared site list from explicit charge states and an energy.
     *
     * @param sites Sites in raster order.
     * @param states One charge state per site.
     * @param energy Electrostatic potential energy of the distribution (unit: eV).
     */
    charge_distribution(site_list sites, std::vector<model::charge_state> states, const double energy) :
            site_storage{std::move(sites)},
            charge_state_values{std::move(states)},
            electrostatic_energy{energy}
    {}
    /**
     * The sites the distribution covers, in raster order.
     *
     * @return The sites.
     */
    [[nodiscard]] const std::vector<lattice_site>& sites() const noexcept
    {
        static const std::vector<lattice_site> none{};

        return site_storage ? *site_storage : none;
    }
    /**
     * The shared site list, to build further distributions over the same SiDBs.
     *
     * @return The shared site list.
     */
    [[nodiscard]] const site_list& shared_sites() const noexcept
    {
        return site_storage;
    }
    /**
     * Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        return charge_state_values.size();
    }
    /**
     * Whether the distribution covers no SiDB.
     *
     * @return `true` iff there is no SiDB.
     */
    [[nodiscard]] bool empty() const noexcept
    {
        return charge_state_values.empty();
    }
    /**
     * The index of a site in the distribution.
     *
     * @param s Site.
     * @return Index of `s`, or `std::nullopt` if the distribution covers no SiDB at `s`.
     */
    [[nodiscard]] std::optional<std::size_t> index_of(const lattice_site& s) const noexcept
    {
        const auto& v  = sites();
        const auto  it = std::ranges::lower_bound(v, s);

        if (it != v.cend() && *it == s)
        {
            return static_cast<std::size_t>(std::distance(v.cbegin(), it));
        }

        return std::nullopt;
    }
    /**
     * The charge state of the SiDB at a site.
     *
     * @param s Site.
     * @return Charge state at `s`, `charge_state::NONE` if the distribution covers no SiDB at `s`.
     */
    [[nodiscard]] model::charge_state get_charge_state(const lattice_site& s) const noexcept
    {
        const auto i = index_of(s);

        return i.has_value() ? charge_state_values[*i] : model::charge_state::NONE;
    }
    /**
     * The charge state of the SiDB at an index.
     *
     * @param index Index in raster order.
     * @return Charge state at `index`, `charge_state::NONE` if the index is out of range.
     */
    [[nodiscard]] model::charge_state get_charge_state_by_index(const std::size_t index) const noexcept
    {
        return index < charge_state_values.size() ? charge_state_values[index] : model::charge_state::NONE;
    }
    /**
     * Assigns the charge state of the SiDB at a site. A site the distribution does not cover is ignored.
     *
     * @param s Site.
     * @param cs Charge state to assign.
     */
    void assign_charge_state(const lattice_site& s, const model::charge_state cs) noexcept
    {
        if (const auto i = index_of(s); i.has_value())
        {
            charge_state_values[*i] = cs;
        }
    }
    /**
     * Assigns the charge state of the SiDB at an index.
     *
     * @param index Index in raster order.
     * @param cs Charge state to assign.
     */
    void assign_charge_state_by_index(const std::size_t index, const model::charge_state cs) noexcept
    {
        charge_state_values[index] = cs;
    }
    /**
     * Assigns one charge state to every SiDB.
     *
     * @param cs Charge state to assign.
     */
    void assign_all_charge_states(const model::charge_state cs) noexcept
    {
        std::ranges::fill(charge_state_values, cs);
    }
    /**
     * All charge states in raster order.
     *
     * @return The charge states.
     */
    [[nodiscard]] const std::vector<model::charge_state>& charge_states() const noexcept
    {
        return charge_state_values;
    }
    /**
     * The electrostatic potential energy of the distribution.
     *
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy() const noexcept
    {
        return electrostatic_energy;
    }
    /**
     * Sets the electrostatic potential energy of the distribution.
     *
     * @param e Energy (unit: eV).
     */
    void assign_energy(const double e) noexcept
    {
        electrostatic_energy = e;
    }
    /**
     * Applies a function to the site of every SiDB in raster order.
     *
     * @tparam Fn Callable on `(const lattice_site&)`.
     * @param fn Function to apply.
     */
    template <typename Fn>
    void foreach_cell(Fn&& fn) const
    {
        std::ranges::for_each(sites(), std::forward<Fn>(fn));
    }
    /**
     * Whether any SiDB has a given charge state.
     *
     * @param cs Charge state.
     * @return `true` iff at least one SiDB is in state `cs`.
     */
    [[nodiscard]] bool charge_exists(const model::charge_state cs) const noexcept
    {
        return std::ranges::find(charge_state_values, cs) != charge_state_values.cend();
    }
    /**
     * Number of negatively charged SiDBs.
     *
     * @return Number of negatively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_negative_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(charge_state_values, model::charge_state::NEGATIVE));
    }
    /**
     * Number of neutrally charged SiDBs.
     *
     * @return Number of neutrally charged SiDBs.
     */
    [[nodiscard]] std::size_t num_neutral_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(charge_state_values, model::charge_state::NEUTRAL));
    }
    /**
     * Number of positively charged SiDBs.
     *
     * @return Number of positively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_positive_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(charge_state_values, model::charge_state::POSITIVE));
    }
    /**
     * The charge index: the distribution read as a number in the given base with the first SiDB as the most
     * significant digit and digit `sign + 1` per SiDB. Two distributions over the same sites have the same index iff
     * they assign the same charge states. The index wraps around past 64 base-2 digits, so it identifies
     * distributions uniquely only for layouts of at most 64 (base 2) or 40 (base 3) SiDBs.
     *
     * @param base Base of the index, 2 or 3.
     * @return The charge index.
     */
    [[nodiscard]] uint64_t charge_index(const uint8_t base) const noexcept
    {
        uint64_t index = 0;

        for (const auto cs : charge_state_values)
        {
            index = (index * base) + static_cast<uint64_t>(model::charge_state_to_sign(cs) + int8_t{1});
        }

        return index;
    }
    /**
     * Compares two distributions for equal charge states, ignoring the energy.
     *
     * @param other Right-hand side distribution.
     * @return `true` iff both assign the same charge states to the same number of SiDBs.
     */
    [[nodiscard]] bool same_charge_states(const charge_distribution& other) const noexcept
    {
        return charge_state_values == other.charge_state_values;
    }
    /**
     * Compares two distributions: same charge states and same energy.
     *
     * @param other Right-hand side distribution.
     * @return `true` iff charge states and energy are equal.
     */
    [[nodiscard]] bool operator==(const charge_distribution& other) const noexcept
    {
        return charge_state_values == other.charge_state_values && electrostatic_energy == other.electrostatic_energy;
    }

  private:
    /**
     * The sites in raster order, shared with the other distributions of the same result.
     */
    site_list site_storage{};
    /**
     * One charge state per site.
     */
    std::vector<model::charge_state> charge_state_values{};
    /**
     * Electrostatic potential energy (unit: eV).
     */
    double electrostatic_energy{0.0};
};

}  // namespace fiction::sidb

namespace std
{
/**
 * @brief Hash for charge distributions, over the charge states.
 */
template <>
struct hash<fiction::sidb::charge_distribution>
{
    /**
     * @brief Computes a hash from the ordered charge states.
     * @param cd Charge distribution to hash.
     * @return Hash value of `cd`.
     */
    std::size_t operator()(const fiction::sidb::charge_distribution& cd) const noexcept
    {
        std::size_t h = cd.size();

        for (const auto cs : cd.charge_states())
        {
            fiction::utils::stl::hash_combine(h, cs);
        }

        return h;
    }
};
}  // namespace std
