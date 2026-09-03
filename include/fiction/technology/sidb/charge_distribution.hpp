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

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
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
            sites_{std::make_shared<const std::vector<lattice_site>>(lyt.sidbs())},
            states_(sites_->size(), cs)
    {}
    /**
     * Creates a distribution over a shared site list with every SiDB in one charge state.
     *
     * @param sites Sites in raster order.
     * @param cs Charge state of every SiDB.
     */
    explicit charge_distribution(site_list sites, const model::charge_state cs = model::charge_state::NEGATIVE) :
            sites_{std::move(sites)},
            states_(sites_ ? sites_->size() : 0, cs)
    {}
    /**
     * Creates a distribution over a shared site list from explicit charge states and an energy.
     *
     * @param sites Sites in raster order.
     * @param states One charge state per site.
     * @param energy Electrostatic potential energy of the distribution (unit: eV).
     */
    charge_distribution(site_list sites, std::vector<model::charge_state> states, const double energy) :
            sites_{std::move(sites)},
            states_{std::move(states)},
            energy_{energy}
    {}
    /**
     * The sites the distribution covers, in raster order.
     *
     * @return The sites.
     */
    [[nodiscard]] const std::vector<lattice_site>& sites() const noexcept
    {
        static const std::vector<lattice_site> none{};

        return sites_ ? *sites_ : none;
    }
    /**
     * The shared site list, to build further distributions over the same SiDBs.
     *
     * @return The shared site list.
     */
    [[nodiscard]] const site_list& shared_sites() const noexcept
    {
        return sites_;
    }
    /**
     * Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        return states_.size();
    }
    /**
     * Whether the distribution covers no SiDB.
     *
     * @return `true` iff there is no SiDB.
     */
    [[nodiscard]] bool empty() const noexcept
    {
        return states_.empty();
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

        return i.has_value() ? states_[*i] : model::charge_state::NONE;
    }
    /**
     * The charge state of the SiDB at an index.
     *
     * @param index Index in raster order.
     * @return Charge state at `index`, `charge_state::NONE` if the index is out of range.
     */
    [[nodiscard]] model::charge_state get_charge_state_by_index(const std::size_t index) const noexcept
    {
        return index < states_.size() ? states_[index] : model::charge_state::NONE;
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
            states_[*i] = cs;
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
        states_[index] = cs;
    }
    /**
     * Assigns one charge state to every SiDB.
     *
     * @param cs Charge state to assign.
     */
    void assign_all_charge_states(const model::charge_state cs) noexcept
    {
        std::ranges::fill(states_, cs);
    }
    /**
     * All charge states in raster order.
     *
     * @return The charge states.
     */
    [[nodiscard]] const std::vector<model::charge_state>& charge_states() const noexcept
    {
        return states_;
    }
    /**
     * The electrostatic potential energy of the distribution.
     *
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy() const noexcept
    {
        return energy_;
    }
    /**
     * Sets the electrostatic potential energy of the distribution.
     *
     * @param e Energy (unit: eV).
     */
    void assign_energy(const double e) noexcept
    {
        energy_ = e;
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
        for (const auto& s : sites())
        {
            fn(s);
        }
    }
    /**
     * Whether any SiDB has a given charge state.
     *
     * @param cs Charge state.
     * @return `true` iff at least one SiDB is in state `cs`.
     */
    [[nodiscard]] bool charge_exists(const model::charge_state cs) const noexcept
    {
        return std::ranges::find(states_, cs) != states_.cend();
    }
    /**
     * Number of negatively charged SiDBs.
     *
     * @return Number of negatively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_negative_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(states_, model::charge_state::NEGATIVE));
    }
    /**
     * Number of neutrally charged SiDBs.
     *
     * @return Number of neutrally charged SiDBs.
     */
    [[nodiscard]] std::size_t num_neutral_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(states_, model::charge_state::NEUTRAL));
    }
    /**
     * Number of positively charged SiDBs.
     *
     * @return Number of positively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_positive_sidbs() const noexcept
    {
        return static_cast<std::size_t>(std::ranges::count(states_, model::charge_state::POSITIVE));
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

        for (const auto cs : states_)
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
        return states_ == other.states_;
    }
    /**
     * Compares two distributions: same charge states and same energy.
     *
     * @param other Right-hand side distribution.
     * @return `true` iff charge states and energy are equal.
     */
    [[nodiscard]] bool operator==(const charge_distribution& other) const noexcept
    {
        return states_ == other.states_ && energy_ == other.energy_;
    }

  private:
    /**
     * The sites in raster order, shared with the other distributions of the same result.
     */
    site_list sites_{};
    /**
     * One charge state per site.
     */
    std::vector<model::charge_state> states_{};
    /**
     * Electrostatic potential energy (unit: eV).
     */
    double energy_{0.0};
};

}  // namespace fiction::sidb

namespace std
{
/**
 * Hash for charge distributions, over the charge states.
 */
template <>
struct hash<fiction::sidb::charge_distribution>
{
    std::size_t operator()(const fiction::sidb::charge_distribution& cd) const noexcept
    {
        std::size_t h = cd.size();

        for (const auto cs : cd.charge_states())
        {
            h ^= static_cast<std::size_t>(static_cast<int8_t>(cs) + 2) +
                 static_cast<std::size_t>(0x9e3779b97f4a7c15ULL) + (h << 6) + (h >> 2);
        }

        return h;
    }
};
}  // namespace std
