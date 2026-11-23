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
     * Adds a state to the energy distribution.
     *
     * @param state The energy state to be added.
     */
    void add_energy_state(const energy_state& state) noexcept
    {
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
    /**
     * The energy distribution map. The key is the energy value and the value is the degeneracy of the energy value.
     */
    std::map<double, uint64_t> distribution;
};
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
    const auto by_charge_states = [](const charge_distribution* lhs, const charge_distribution* rhs)
    { return lhs->charge_states() < rhs->charge_states(); };
    std::set<const charge_distribution*, decltype(by_charge_states)> unique{};
    std::map<double, uint64_t>                                       levels{};

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

        if (unique.insert(&cd).second && matching_level(cd.energy()) == levels.end())
        {
            levels.emplace(cd.energy(), 0);
        }
    }

    for (const auto* cd : unique)
    {
        ++matching_level(cd->energy())->second;
    }

    energy_distribution distribution{};
    for (const auto& [energy, degeneracy] : levels)
    {
        distribution.add_energy_state(energy_state{energy, degeneracy});
    }

    return distribution;
}

}  // namespace fiction::sidb::simulation::analysis
