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
 * @brief Distribution of the electrostatic energies a simulation produced.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::analysis
{

/**
 * This struct stores the energy state of an SiDB layout. The energy state consists of the electrostatic potential
 * energy and the degeneracy of the state.
 */
struct energy_state
{
    /**
     * Default constructor.
     *
     * @param energy The electrostatic potential energy of the charge distribution (eV).
     * @param state_degeneracy The degeneracy of the state.
     */
    energy_state(const double energy, const uint64_t state_degeneracy) :
            electrostatic_potential_energy{energy},
            degeneracy{state_degeneracy}
    {}
    /**
     * The electrostatic potential energy of the charge distribution (eV).
     */
    double electrostatic_potential_energy;
    /**
     * The degeneracy of the state.
     */
    uint64_t degeneracy;
};

/**
 * This class is used to store the energy distribution of an SiDB layout. The energy distribution is a map that contains
 * the electrostatic potential as a key and its degeneracy as a value. To be more precise, if two different charge
 * distributions occur with the same energy, the degeneracy value of the energy state is 2.
 */
class energy_distribution
{
  public:
    /**
     * Default constructor.
     */
    energy_distribution() = default;
    /**
     * Returns the nth state (energy + degeneracy) in ascending energy order.
     *
     * @param state_index The index of the state to be retrieved.
     * @return Energy state, or `std::nullopt` (`None` in Python) if the index is out of range.
     */
    [[nodiscard]] std::optional<energy_state> get_nth_state(const uint64_t state_index) const noexcept
    {
        if (state_index < distribution.size())
        {
            const auto it =
                std::next(distribution.cbegin(),
                          static_cast<typename std::iterator_traits<decltype(distribution.cbegin())>::difference_type>(
                              state_index));
            return energy_state{it->first, it->second};
        }

        return std::nullopt;
    }
    /**
     * Returns the degeneracy value (number of states) with the given energy value.
     *
     * @param energy The energy value whose degeneracy is requested.
     * @return The degeneracy of the given energy, or `std::nullopt` (`None` in Python) if the energy is not found.
     */
    [[nodiscard]] std::optional<uint64_t> degeneracy(const double energy) const noexcept
    {
        for (const auto& [key, value] : distribution)
        {
            if (std::abs(key - energy) < fiction::utils::math::ERROR_MARGIN)
            {
                return value;
            }
        }

        return std::nullopt;
    }
    /**
     * Adds a state to the energy distribution, replacing any state at the same exact energy.
     *
     * @param state The energy state to be added.
     * @throws std::invalid_argument If the energy is non-finite.
     */
    void add_energy_state(const energy_state& state)
    {
        if (!std::isfinite(state.electrostatic_potential_energy))
        {
            throw std::invalid_argument{"Energy state must have finite energy"};
        }
        distribution[state.electrostatic_potential_energy] = state.degeneracy;
    }
    /**
     * Returns the number of energy states in the energy distribution.
     *
     * @return The number of energy states in the energy distribution.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        return distribution.size();
    }
    /**
     * Checks if the energy distribution is empty.
     *
     * @return `true` if the energy distribution is empty, `false` otherwise.
     */
    [[nodiscard]] bool empty() const noexcept
    {
        return distribution.empty();
    }
    /**
     * Returns the maximum energy value in the energy distribution.
     *
     * @return The maximum energy value in the energy distribution.
     * @throws std::invalid_argument If the energy distribution is empty.
     */
    [[nodiscard]] double max_energy() const
    {
        if (distribution.empty())
        {
            throw std::invalid_argument{"energy distribution is empty"};
        }
        return distribution.crbegin()->first;
    }
    /**
     * Returns the minimum energy value in the energy distribution.
     *
     * @return The minimum energy value in the energy distribution.
     * @throws std::invalid_argument If the energy distribution is empty.
     */
    [[nodiscard]] double min_energy() const
    {
        if (distribution.empty())
        {
            throw std::invalid_argument{"energy distribution is empty"};
        }
        return distribution.cbegin()->first;
    }
    /**
     * Applies a function to all states in the energy distribution.
     *
     * @tparam Fn Functor type.
     * @param fn Functor to apply to each key-value pair.
     */
    template <typename Fn>
    void for_each(Fn&& fn) const
    {
        for (const auto& [energy, degeneracy] : distribution)
        {
            std::invoke(std::forward<Fn>(fn), energy, degeneracy);
        }
    }

  private:
    /** @brief Lets the calculator adopt its populated map without reallocating its entries. */
    friend energy_distribution calculate_energy_distribution(const std::vector<charge_distribution>&);

    /**
     * The energy distribution map. The key is the energy value and the value is the degeneracy of the energy value.
     */
    std::map<double, uint64_t> distribution;
};
namespace detail
{

/**
 * @brief Accumulates each distinct configuration at its lowest matching first-seen energy level.
 * @tparam Group Accumulator stored at each representative energy.
 * @tparam Fn Callable that updates a group from one distinct configuration.
 * @param charge_distributions Charge distributions of one layout.
 * @param accumulate Called with the group and first occurrence of each configuration.
 * @return Groups ordered by representative energy.
 * @throws std::invalid_argument If any input energy is non-finite.
 */
template <typename Group, typename Fn>
[[nodiscard]] std::map<double, Group>
group_energy_configurations(const std::vector<charge_distribution>& charge_distributions, Fn accumulate)
{
    const auto by_charge_states = [](const charge_distribution* lhs, const charge_distribution* rhs)
    { return lhs->charge_states() < rhs->charge_states(); };
    std::set<const charge_distribution*, decltype(by_charge_states)> unique{};
    std::vector<const charge_distribution*>                          first_seen{};
    std::map<double, Group>                                          levels{};

    const auto matching_level = [&levels](const double energy)
    {
        const auto higher = levels.lower_bound(energy);
        // Representatives are at least ERROR_MARGIN apart. Only the two neighbors can match.
        if (higher != levels.begin())
        {
            const auto lower = std::prev(higher);
            if (energy - lower->first < utils::math::ERROR_MARGIN)
            {
                return lower;
            }
        }
        if (higher != levels.end() && higher->first - energy < utils::math::ERROR_MARGIN)
        {
            return higher;
        }
        return levels.end();
    };

    for (const auto& cd : charge_distributions)
    {
        if (!std::isfinite(cd.energy()))
        {
            throw std::invalid_argument{"Charge-distribution energy must be finite"};
        }

        if (!unique.insert(&cd).second)
        {
            continue;
        }
        first_seen.push_back(&cd);
        if (matching_level(cd.energy()) == levels.end())
        {
            levels.try_emplace(cd.energy());
        }
    }

    for (const auto* cd : first_seen)
    {
        const auto level = matching_level(cd->energy());
        assert(level != levels.end() && "Each unique configuration has a matching energy level.");
        std::invoke(accumulate, level->second, *cd);
    }

    return levels;
}

}  // namespace detail

/**
 * Computes the number of distinct charge configurations at each energy level. For identical charge states, the first
 * occurrence supplies the energy. A configuration starts a new level at its own energy when every existing level is
 * at least `ERROR_MARGIN` away. With the levels established, each configuration counts at the lowest level within
 * `ERROR_MARGIN` of its energy.
 *
 * @param charge_distributions Charge distributions of one layout.
 * @return The energy distribution.
 * @throws std::invalid_argument if a charge distribution has non-finite energy.
 */
[[nodiscard]] inline energy_distribution
calculate_energy_distribution(const std::vector<charge_distribution>& charge_distributions)
{
    energy_distribution result{};
    result.distribution =
        detail::group_energy_configurations<uint64_t>(charge_distributions, [](auto& count, const auto&) { ++count; });
    return result;
}

}  // namespace fiction::sidb::simulation::analysis
