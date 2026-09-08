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
 * @brief Mutable charge states, potentials, energy, and charge-index enumeration for SiDB simulation.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/utils/math/math_utils.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <optional>
#include <random>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::detail
{

/**
 * @brief Whether the dependent SiDB adapts its charge state to the other SiDBs after a change.
 */
enum class dependent_dot_mode : uint8_t
{
    /**
     * @brief The dependent SiDB keeps its charge state.
     */
    FIXED = 0,
    /**
     * @brief The dependent SiDB takes the charge state its local potential dictates.
     */
    VARIABLE = 1
};
/**
 * @brief Whether the energy is recomputed after a change.
 */
enum class energy_calculation : uint8_t
{
    /**
     * @brief Keep the stored energy.
     */
    KEEP_OLD_ENERGY_VALUE = 0,
    /**
     * @brief Recompute the energy.
     */
    UPDATE_ENERGY = 1
};
/**
 * @brief Whether the local potentials are updated from the record of changed SiDBs or recomputed from scratch.
 */
enum class charge_distribution_history : uint8_t
{
    /**
     * @brief Update only the contributions of the SiDBs the last index change flipped.
     */
    CONSIDER = 0,
    /**
     * @brief Recompute every local potential.
     */
    NEGLECT = 1
};
/**
 * @brief Whether the charge index follows a charge-state assignment.
 */
enum class charge_index_mode : uint8_t
{
    /**
     * @brief Recompute the charge index.
     */
    UPDATE_CHARGE_INDEX = 0,
    /**
     * @brief Keep the charge index.
     */
    KEEP_CHARGE_INDEX = 1
};
/**
 * @brief Whether the charge states follow a charge-index assignment.
 */
enum class charge_distribution_mode : uint8_t
{
    /**
     * @brief Decode the index into charge states.
     */
    UPDATE_CHARGE_DISTRIBUTION = 0,
    /**
     * @brief Keep the charge states.
     */
    KEEP_CHARGE_DISTRIBUTION = 1
};
/**
 * @brief How a charge index is decoded into charge states.
 */
enum class charge_index_recomputation : uint8_t
{
    /**
     * @brief Decode every digit; leading zero digits become negative charge states.
     */
    FROM_SCRATCH = 0,
    /**
     * @brief Decode only the non-zero digits.
     */
    IGNORE_LEADING_ZEROES = 1
};

/**
 * @brief The mutable working set of the SiDB simulators over one `potential_landscape`: the charge state of every SiDB,
 * the local internal potentials, the energy, the validity flag, and the charge-index machinery the exhaustive engines
 * enumerate with (Gray codes, a dependent SiDB whose charge follows the others, and a sublayout of SiDBs that can be
 * positively charged). The landscape stays read-only, so worker threads share one landscape and own one state each.
 *
 * `snapshot()` yields the current `charge_distribution` in O(N).
 */
class simulation_state
{
  public:
    /**
     * @brief How the energy is computed.
     */
    enum class energy_model : uint8_t
    {
        /**
         * @brief The full energy: SiDB and defect charges in the internal and external potentials.
         */
        FULL,
        /**
         * @brief The SiDB charges in the internal potential only, which is what QuickSim compares.
         */
        INTERNAL_ONLY
    };
    /**
     * @brief How a charge index is decoded.
     */
    enum class index_decoding : uint8_t
    {
        /**
         * @brief Decode the whole index.
         */
        PLAIN,
        /**
         * @brief Decode the layout index and the sublayout index separately and record the flipped SiDBs, as QuickExact
         * needs.
         */
        TRACKED
    };
    /**
     * @brief Creates a state over a landscape with every SiDB in one charge state and the potentials, energy, and
     * validity computed for it.
     *
     * @param land Landscape; must outlive the state.
     * @param cs Initial charge state of every SiDB.
     * @param model Energy model.
     * @param decoding Index decoding.
     */
    explicit simulation_state(const potential_landscape& land,
                              const model::charge_state  cs       = model::charge_state::NEGATIVE,
                              const energy_model         model    = energy_model::FULL,
                              const index_decoding       decoding = index_decoding::PLAIN) :
            landscape_ptr{&land},
            num_sites{land.num_sidbs()},
            selected_energy_model{model},
            selected_index_decoding{decoding},
            simulation_base{land.params().base},
            charge_index_base{land.params().base},
            charge_distribution_state{land.sites(), cs},
            internal_potential_values(num_sites, 0.0),
            maximum_charge_index{max_index(simulation_base, num_sites)}
    {
        charge_distribution_to_index();
        update_after_charge_change();
    }
    /**
     * @brief The landscape.
     *
     * @return The landscape.
     */
    [[nodiscard]] const potential_landscape& landscape() const noexcept
    {
        return *landscape_ptr;
    }
    /**
     * @brief Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] std::size_t num_sidbs() const noexcept
    {
        return num_sites;
    }
    /**
     * @brief The base of the physical model, 2 or 3: whether positive charge states take part at all.
     *
     * @return The base.
     */
    [[nodiscard]] uint8_t base() const noexcept
    {
        return simulation_base;
    }
    /**
     * @brief The base the charge index is decoded in. It equals `base()` until a three-state sublayout is split off,
     * from then on the remaining SiDBs are decoded in base 2.
     *
     * @return The index base.
     */
    [[nodiscard]] uint8_t index_base() const noexcept
    {
        return charge_index_base;
    }

    // ----------------------------------------------------------------------------------------------- charge states

    /**
     * @brief The charge state of an SiDB.
     *
     * @param i Index of the SiDB.
     * @return Charge state.
     */
    [[nodiscard]] model::charge_state get_charge_state_by_index(const std::size_t i) const noexcept
    {
        return charge_distribution_state.charge_states()[i];
    }
    /**
     * @brief Assigns the charge state of an SiDB.
     *
     * @param i Index of the SiDB.
     * @param cs Charge state.
     * @param index_mode Whether to recompute the charge index.
     */
    void assign_charge_state_by_index(const std::size_t i, const model::charge_state cs,
                                      const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX)
    {
        charge_distribution_state.assign_charge_state_by_index(i, cs);

        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            charge_distribution_to_index();
        }
    }
    /**
     * @brief Assigns one charge state to every SiDB.
     *
     * @param cs Charge state.
     * @param index_mode Whether to recompute the charge index.
     */
    void assign_all_charge_states(const model::charge_state cs,
                                  const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        charge_distribution_state.assign_all_charge_states(cs);

        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            charge_distribution_to_index();
        }
    }
    /**
     * @brief Number of negatively charged SiDBs.
     *
     * @return Number of negatively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_negative_sidbs() const noexcept
    {
        return charge_distribution_state.num_negative_sidbs();
    }
    /**
     * @brief The current charge distribution with its energy.
     *
     * @return A copy of the charge states and the energy.
     */
    [[nodiscard]] charge_distribution snapshot() const
    {
        return charge_distribution_state;
    }
    /**
     * @brief The current charge distribution with its energy.
     *
     * @return A constant reference to the stored charge distribution.
     */
    [[nodiscard]] const charge_distribution& current() const noexcept
    {
        return charge_distribution_state;
    }

    // -------------------------------------------------------------------------------------------------- potentials

    /**
     * @brief Updates the local internal potentials after charge states changed.
     *
     * @param history_mode Whether to apply only the recorded flips or to recompute from scratch.
     */
    void update_local_internal_potential(
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT)
    {
        if (history_mode == charge_distribution_history::NEGLECT)
        {
            for (std::size_t i = 0; i < num_sites; ++i)
            {
                double collect = 0.0;

                for (std::size_t j = 0; j < num_sites; ++j)
                {
                    collect +=
                        landscape_ptr->chargeless_potential(i, j) *
                        static_cast<double>(model::charge_state_to_sign(charge_distribution_state.charge_states()[j]));
                }

                internal_potential_values[i] = landscape_ptr->local_potential_caused_by_defects(i) + collect;
            }
        }
        else if (simulation_base == 2)
        {
            if (gray_code_history.first != -1)
            {
                const auto changed     = static_cast<std::size_t>(gray_code_history.first);
                const auto charge_diff = static_cast<double>(
                    model::charge_state_to_sign(charge_distribution_state.charge_states()[changed]) -
                    gray_code_history.second);

                for (std::size_t j = 0; j < num_sites; ++j)
                {
                    internal_potential_values[j] += landscape_ptr->chargeless_potential(changed, j) * charge_diff;
                }
            }
        }
        else
        {
            for (const auto& [changed, old_sign] : changed_sidb_history)
            {
                const auto charge_diff = static_cast<double>(model::charge_state_to_sign(
                                             charge_distribution_state.charge_states()[changed])) -
                                         old_sign;

                for (std::size_t j = 0; j < num_sites; ++j)
                {
                    internal_potential_values[j] += landscape_ptr->chargeless_potential(changed, j) * charge_diff;
                }
            }
        }
    }
    /**
     * @brief The local internal potential at an SiDB: charged SiDBs plus charged defects.
     *
     * @param i Index of the SiDB.
     * @return Local internal potential (unit: V).
     */
    [[nodiscard]] double local_internal_potential(const std::size_t i) const noexcept
    {
        return internal_potential_values[i];
    }
    /**
     * @brief The local potential at an SiDB: internal plus external.
     *
     * @param i Index of the SiDB.
     * @return Local potential (unit: V).
     */
    [[nodiscard]] double local_potential(const std::size_t i) const
    {
        return internal_potential_values[i] + landscape_ptr->local_external_potential(i);
    }
    /**
     * @brief All local internal potentials.
     *
     * @return Local internal potential per SiDB (unit: V).
     */
    [[nodiscard]] const std::vector<double>& local_internal_potentials() const noexcept
    {
        return internal_potential_values;
    }
    /**
     * @brief Overwrites the local internal potential at an SiDB.
     *
     * @param i Index of the SiDB.
     * @param pot Local internal potential (unit: V).
     */
    void assign_local_internal_potential(const std::size_t i, const double pot) noexcept
    {
        internal_potential_values[i] = pot;
    }

    // ---------------------------------------------------------------------------------------- energy and validity

    /**
     * @brief Recomputes the energy from the current charge states and local internal potentials.
     */
    void recompute_energy()
    {
        if (selected_energy_model == energy_model::INTERNAL_ONLY)
        {
            double collect = 0.0;

            for (std::size_t i = 0; i < num_sites; ++i)
            {
                collect +=
                    internal_potential_values[i] *
                    static_cast<double>(model::charge_state_to_sign(charge_distribution_state.charge_states()[i]));
            }

            charge_distribution_state.assign_energy(0.5 * collect);

            return;
        }

        charge_distribution_state.assign_energy(
            landscape_ptr->energy(charge_distribution_state, internal_potential_values));
    }
    /**
     * @brief The energy of the current charge states.
     *
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy() const noexcept
    {
        return charge_distribution_state.energy();
    }
    /**
     * @brief Updates potentials, the dependent SiDB, the energy, and the validity after charge states changed.
     *
     * @param dep_dot Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void
    update_after_charge_change(const dependent_dot_mode          dep_dot      = dependent_dot_mode::FIXED,
                               const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
                               const charge_distribution_history history_mode = charge_distribution_history::NEGLECT)
    {
        update_local_internal_potential(history_mode);

        if (dep_dot == dependent_dot_mode::VARIABLE)
        {
            update_charge_state_of_dependent_dot();
        }
        if (energy_mode == energy_calculation::UPDATE_ENERGY)
        {
            recompute_energy();
        }

        validity_check();
    }
    /**
     * @brief Recomputes the validity flag: population stability of every SiDB, then configuration stability.
     */
    void validity_check()
    {
        physically_valid = landscape_ptr->is_population_stable(charge_distribution_state, internal_potential_values) &&
                           landscape_ptr->is_configuration_stable(charge_distribution_state, internal_potential_values);
    }
    /**
     * @brief Whether the current charge states are physically valid, as of the last `validity_check`.
     *
     * @return `true` iff physically valid.
     */
    [[nodiscard]] bool is_physically_valid() const noexcept
    {
        return physically_valid;
    }
    /**
     * @brief Whether no charge hop lowers the energy of the current charge states.
     *
     * @return `true` iff configuration stable.
     */
    [[nodiscard]] bool is_configuration_stable() const
    {
        return landscape_ptr->is_configuration_stable(charge_distribution_state, internal_potential_values);
    }
    /**
     * @brief Marks the current charge states physically valid without checking.
     */
    void declare_physically_valid() noexcept
    {
        physically_valid = true;
    }

    // ------------------------------------------------------------------------------------------------ charge index

    /**
     * @brief Recomputes the charge index over every SiDB in the plain base, ignoring the dependent SiDB and the
     * sublayout.
     */
    void charge_distribution_to_index_general() noexcept
    {
        charge_index_value = charge_distribution_state.charge_index(simulation_base);
    }
    /**
     * @brief Recomputes the charge index (and the sublayout index) from the charge states, skipping the dependent SiDB
     * and splitting the SiDBs that can be positively charged into their own base-3 index.
     */
    void charge_distribution_to_index() noexcept
    {
        uint64_t index     = 0;
        uint64_t sub_index = 0;

        const auto digit = [this](const std::size_t i)
        {
            return static_cast<uint64_t>(model::charge_state_to_sign(charge_distribution_state.charge_states()[i]) +
                                         int8_t{1});
        };

        if (!three_state_sidb_indices.empty())
        {
            for (const auto i : three_state_sidb_indices)
            {
                sub_index = (sub_index * 3) + digit(i);
            }
            for (const auto i : two_state_sidb_indices)
            {
                if (!dependent_sidb.has_value() || i != *dependent_sidb)
                {
                    index = (index * 2) + digit(i);
                }
            }
        }
        else
        {
            for (std::size_t i = 0; i < num_sites; ++i)
            {
                if (!dependent_sidb.has_value() || i != *dependent_sidb)
                {
                    index = (index * simulation_base) + digit(i);
                }
            }
        }

        charge_index_value     = index;
        sublayout_charge_index = sub_index;
    }
    /**
     * @brief The charge index.
     *
     * @return The charge index.
     */
    [[nodiscard]] uint64_t charge_index() const noexcept
    {
        return charge_index_value;
    }
    /**
     * @brief The largest charge index over the SiDBs the index enumerates.
     *
     * @return The maximum charge index.
     */
    [[nodiscard]] uint64_t max_charge_index() const noexcept
    {
        return maximum_charge_index;
    }
    /**
     * @brief The charge index of the sublayout of SiDBs that can be positively charged.
     *
     * @return The sublayout charge index.
     */
    [[nodiscard]] uint64_t charge_index_of_sub_layout() const noexcept
    {
        return sublayout_charge_index;
    }
    /**
     * @brief The largest sublayout charge index.
     *
     * @return The maximum sublayout charge index.
     */
    [[nodiscard]] uint64_t max_charge_index_sub_layout() const noexcept
    {
        return maximum_sublayout_charge_index;
    }
    /**
     * @brief Assigns a charge index.
     *
     * @param index Charge index, at most `max_charge_index()`.
     * @param mode Whether to decode the index into charge states.
     */
    void assign_charge_index(const uint64_t                 index,
                             const charge_distribution_mode mode = charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION)
    {
        assert(index <= maximum_charge_index && "charge index is too large");

        charge_index_value = index;

        if (mode == charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION)
        {
            index_to_charge_distribution();
        }
    }
    /**
     * @brief Steps the charge index up by one and decodes it.
     *
     * @param dep_dot Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void
    increase_charge_index_by_one(const dependent_dot_mode          dep_dot      = dependent_dot_mode::FIXED,
                                 const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
                                 const charge_distribution_history history_mode = charge_distribution_history::NEGLECT)
    {
        if (charge_index_value >= maximum_charge_index)
        {
            return;
        }

        charge_index_value += 1;

        if (selected_index_decoding == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
        }

        update_after_charge_change(dep_dot, energy_mode, history_mode);
    }
    /**
     * @brief Steps the sublayout charge index up by one and decodes it.
     *
     * @param dep_dot Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void increase_charge_index_of_sub_layout_by_one(
        const dependent_dot_mode          dep_dot      = dependent_dot_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT)
    {
        if (sublayout_charge_index >= maximum_sublayout_charge_index)
        {
            return;
        }

        sublayout_charge_index += 1;

        if (selected_index_decoding == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
        }

        update_after_charge_change(dep_dot, energy_mode, history_mode);
    }
    /**
     * @brief Resets the sublayout charge index to zero and decodes it with the flips recorded.
     */
    void reset_charge_index_sub_layout()
    {
        sublayout_charge_index = 0;

        if (selected_index_decoding == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution();
        }

        update_after_charge_change(dependent_dot_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                   charge_distribution_history::CONSIDER);
    }
    /**
     * @brief Assigns a charge index given as a Gray code and flips the one SiDB that differs from the previous Gray
     * code.
     *
     * @param current_gray_code New charge index.
     * @param previous_gray_code Previous charge index.
     * @param dep_dot Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flip to the potentials.
     */
    void assign_charge_index_by_gray_code(
        const uint64_t current_gray_code, const uint64_t previous_gray_code,
        const dependent_dot_mode          dep_dot      = dependent_dot_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT)
    {
        if (current_gray_code <= maximum_charge_index)
        {
            charge_index_value = current_gray_code;
            gray_code_to_charge_distribution(current_gray_code, previous_gray_code);
            update_after_charge_change(dep_dot, energy_mode, history_mode);
        }
    }

    // ---------------------------------------------------------------------------------- dependent SiDB, sublayout

    /**
     * @brief Declares the SiDB whose charge state follows the other SiDBs; the charge index then enumerates the others.
     *
     * @param i Index of the dependent SiDB.
     */
    void assign_dependent_dot(const std::size_t i) noexcept
    {
        assert(i < num_sites && "dependent dot is not part of the layout");

        dependent_sidb       = i;
        maximum_charge_index = max_index(simulation_base, num_sites - 1);
    }
    /**
     * @brief The dependent SiDB, if any.
     *
     * @return Index of the dependent SiDB.
     */
    [[nodiscard]] std::optional<std::size_t> dependent_dot() const noexcept
    {
        return dependent_sidb;
    }
    /**
     * @brief Sets the base of the charge index and the maximum index accordingly.
     *
     * @param base 2 or 3.
     */
    void assign_base_number(const uint8_t base) noexcept
    {
        simulation_base      = base;
        charge_index_base    = base;
        maximum_charge_index = max_index(base, dependent_sidb.has_value() ? num_sites - 1 : num_sites);
    }
    /**
     * @brief Determines from the all-negative charge distribution which SiDBs can become positively charged and, if any
     * can, switches to a base-3 sublayout index over them and a base-2 index over the rest.
     *
     * @return `true` iff some SiDB other than the dependent one can be positively charged.
     */
    [[nodiscard]] bool is_three_state_simulation_required()
    {
        update_after_charge_change();

        three_state_sidb_indices.clear();
        two_state_sidb_indices.clear();
        dependent_in_sublayout = false;

        assert(num_negative_sidbs() == num_sites && "All SiDBs have to be negatively charged");

        bool required = false;

        for (std::size_t i = 0; i < num_sites; ++i)
        {
            if (-internal_potential_values[i] <
                landscape_ptr->effective_charge_transition_thresholds(
                    i)[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
            {
                continue;
            }

            if (dependent_sidb.has_value() && i == *dependent_sidb)
            {
                dependent_in_sublayout = true;
            }
            else
            {
                three_state_sidb_indices.push_back(i);
                required = true;
            }
        }

        for (std::size_t i = 0; i < num_sites; ++i)
        {
            if (!std::ranges::binary_search(three_state_sidb_indices, i) &&
                (!dependent_sidb.has_value() || i != *dependent_sidb))
            {
                two_state_sidb_indices.push_back(i);
            }
        }

        if (required)
        {
            assign_base_number_to_three();
        }

        return required;
    }
    /**
     * @brief The SiDBs that can be positively charged, ascending.
     *
     * @return Indices of the sublayout SiDBs.
     */
    [[nodiscard]] const std::vector<std::size_t>& three_state_sidbs() const noexcept
    {
        return three_state_sidb_indices;
    }
    /**
     * @brief The SiDBs that can only be negatively or neutrally charged, ascending, without the dependent SiDB.
     *
     * @return Indices of the two-state SiDBs.
     */
    [[nodiscard]] const std::vector<std::size_t>& two_state_sidbs() const noexcept
    {
        return two_state_sidb_indices;
    }

    // ---------------------------------------------------------------------------------------------- QuickSim helpers

    /**
     * @brief Reseeds the random generator of `adjacent_search` from `std::random_device`. A state copied into a worker
     * thread calls this so the threads do not replay one random sequence.
     */
    void reseed() noexcept
    {
        random_generator.seed(std::random_device{}());
    }

    /**
     * @brief The SiDBs whose local potential forces them negative regardless of the other charges, i.e., whose (0/-)
     * transition lies above the Fermi level even at the current potentials.
     *
     * @return Indices of the SiDBs that have to be negatively charged.
     */
    [[nodiscard]] std::vector<std::size_t> negative_sidb_detection() const
    {
        std::vector<std::size_t> negative{};
        negative.reserve(num_sites);

        for (std::size_t i = 0; i < num_sites; ++i)
        {
            if ((-local_potential(i) + landscape_ptr->params().mu_minus) < -utils::math::ERROR_MARGIN)
            {
                negative.push_back(i);
            }
        }

        return negative;
    }
    /**
     * @brief QuickSim's max-min diversity step: among the neutral SiDBs, picks one at random that is at least `alpha`
     * times the maximal distance away from every negative SiDB, charges it negatively, and updates the potentials and
     * the energy incrementally.
     *
     * @param alpha Fraction of the maximal distance a candidate has to keep from the negative SiDBs.
     * @param negative_indices Indices of the negative SiDBs; the chosen SiDB is appended.
     */
    void adjacent_search(const double alpha, std::vector<std::size_t>& negative_indices)
    {
        double     dist_max     = 0.0;
        const auto reserve_size = num_sites - negative_indices.size();

        std::vector<std::size_t> index_vector{};
        index_vector.reserve(reserve_size);
        std::vector<double> distance{};
        distance.reserve(reserve_size);

        for (std::size_t unocc = 0; unocc < num_sites; ++unocc)
        {
            if (charge_distribution_state.charge_states()[unocc] != model::charge_state::NEUTRAL)
            {
                continue;
            }

            const auto dist_min =
                std::accumulate(negative_indices.cbegin(), negative_indices.cend(),
                                std::numeric_limits<double>::infinity(), [&](const double acc, const std::size_t occ)
                                { return std::min(acc, landscape_ptr->nm_distance(unocc, occ)); });

            index_vector.push_back(unocc);
            distance.push_back(dist_min);

            dist_max = std::max(dist_max, dist_min);
        }

        std::vector<std::size_t> candidates{};
        candidates.reserve(reserve_size);

        for (std::size_t i = 0; i < distance.size(); ++i)
        {
            if (distance[i] >= (alpha * dist_max))
            {
                candidates.push_back(i);
            }
        }

        if (candidates.empty())
        {
            return;
        }

        std::uniform_int_distribution<std::size_t> dist(0, candidates.size() - 1);

        const auto chosen = index_vector[candidates[dist(random_generator)]];

        charge_distribution_state.assign_charge_state_by_index(chosen, model::charge_state::NEGATIVE);
        negative_indices.push_back(chosen);

        charge_distribution_state.assign_energy(charge_distribution_state.energy() - internal_potential_values[chosen]);

        for (std::size_t i = 0; i < num_sites; ++i)
        {
            internal_potential_values[i] -= landscape_ptr->chargeless_potential(i, chosen);
        }
    }

  private:
    /**
     * @brief The landscape.
     */
    const potential_landscape* landscape_ptr;
    /**
     * @brief Number of SiDBs.
     */
    std::size_t num_sites;
    /**
     * @brief Energy model.
     */
    energy_model selected_energy_model;
    /**
     * @brief Index decoding.
     */
    index_decoding selected_index_decoding;
    /**
     * @brief Base of the physical model.
     */
    uint8_t simulation_base;
    /**
     * @brief Base the charge index is decoded in.
     */
    uint8_t charge_index_base;
    /**
     * @brief Charge states and energy.
     */
    charge_distribution charge_distribution_state;
    /**
     * @brief Local internal potential per SiDB (unit: V).
     */
    std::vector<double> internal_potential_values;
    /**
     * @brief Validity flag.
     */
    bool physically_valid{false};
    /**
     * @brief Charge index over the SiDBs that are not in the sublayout and not the dependent SiDB.
     */
    uint64_t charge_index_value{0};
    /**
     * @brief Charge index over the sublayout.
     */
    uint64_t sublayout_charge_index{0};
    /**
     * @brief Maximum charge index.
     */
    uint64_t maximum_charge_index{0};
    /**
     * @brief Maximum sublayout charge index.
     */
    uint64_t maximum_sublayout_charge_index{0};
    /**
     * @brief The dependent SiDB.
     */
    std::optional<std::size_t> dependent_sidb{};
    /**
     * @brief Whether the dependent SiDB can be positively charged.
     */
    bool dependent_in_sublayout{false};
    /**
     * @brief SiDBs that can be positively charged, ascending.
     */
    std::vector<std::size_t> three_state_sidb_indices{};
    /**
     * @brief SiDBs that cannot be positively charged, ascending, without the dependent SiDB.
     */
    std::vector<std::size_t> two_state_sidb_indices{};
    /**
     * @brief The SiDB the last Gray-code step flipped and its previous sign; -1 if none.
     */
    std::pair<int64_t, int8_t> gray_code_history{-1, int8_t{0}};
    /**
     * @brief The SiDBs the last tracked decoding flipped with their previous signs.
     */
    std::vector<std::pair<std::size_t, int8_t>> changed_sidb_history{};
    /**
     * @brief Random generator of `adjacent_search`; one per state, so worker threads never share it.
     */
    std::mt19937_64 random_generator{std::random_device{}()};
    /**
     * @brief The largest index of `digits` digits in `base`.
     *
     * @param base Charge-index base, either 2 or 3.
     * @param digits Number of charge-index digits.
     * @return The largest representable index, saturated at `uint64_t` maximum.
     */
    [[nodiscard]] static uint64_t max_index(const uint8_t base, const std::size_t digits) noexcept
    {
        uint64_t result = 1;

        for (std::size_t digit = 0; digit < digits; ++digit)
        {
            if (result > std::numeric_limits<uint64_t>::max() / base)
            {
                return std::numeric_limits<uint64_t>::max();
            }

            result *= base;
        }

        return result - 1;
    }
    /**
     * @brief Switches to a base-3 sublayout index over the SiDBs that can be positively charged.
     */
    void assign_base_number_to_three()
    {
        simulation_base   = 3;
        charge_index_base = 2;

        if (dependent_sidb.has_value())
        {
            if (!three_state_sidb_indices.empty())
            {
                maximum_charge_index           = max_index(2, num_sites - 1 - three_state_sidb_indices.size());
                maximum_sublayout_charge_index = max_index(3, three_state_sidb_indices.size());
            }
        }
        else
        {
            maximum_charge_index           = max_index(2, num_sites - three_state_sidb_indices.size());
            maximum_sublayout_charge_index = max_index(3, three_state_sidb_indices.size());
        }

        if (maximum_charge_index == 0)
        {
            assign_charge_index(0);
        }
    }
    /**
     * @brief Sets the dependent SiDB to the charge state its local potential dictates and updates the potentials of the
     * others incrementally.
     */
    void update_charge_state_of_dependent_dot()
    {
        if (!dependent_sidb.has_value())
        {
            return;
        }

        const auto  d           = *dependent_sidb;
        const auto  loc_pot_dot = -internal_potential_values[d];
        const auto& t           = landscape_ptr->effective_charge_transition_thresholds(d);
        const auto  current     = charge_distribution_state.charge_states()[d];

        const auto switch_to = [&](const model::charge_state cs)
        {
            const auto charge_diff = static_cast<double>(model::charge_state_to_sign(cs)) -
                                     static_cast<double>(model::charge_state_to_sign(current));

            for (std::size_t i = 0; i < num_sites; ++i)
            {
                if (i != d)
                {
                    internal_potential_values[i] += landscape_ptr->chargeless_potential(i, d) * charge_diff;
                }
            }

            charge_distribution_state.assign_charge_state_by_index(d, cs);
        };

        if (loc_pot_dot < t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND)])
        {
            if (current != model::charge_state::NEGATIVE)
            {
                switch_to(model::charge_state::NEGATIVE);
            }
        }
        else if (loc_pot_dot > t[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
        {
            // the dependent SiDB can only be positively charged in a three-state simulation
            if ((charge_index_base == 3 || !three_state_sidb_indices.empty()) &&
                current != model::charge_state::POSITIVE)
            {
                switch_to(model::charge_state::POSITIVE);
            }
        }
        else if (current != model::charge_state::NEUTRAL)
        {
            switch_to(model::charge_state::NEUTRAL);
        }
    }
    /**
     * @brief Flips the one SiDB in which two Gray codes differ and records it for the incremental potential update.
     *
     * @param new_gray_code New Gray code; differs from the old code in at most one bit.
     * @param old_gray_code Gray code of the current charge distribution.
     */
    void gray_code_to_charge_distribution(const uint64_t new_gray_code, const uint64_t old_gray_code)
    {
        gray_code_history = {-1, int8_t{0}};

        const std::bitset<64> r_new(new_gray_code);
        const std::bitset<64> r_old(old_gray_code);
        const std::bitset<64> diff = r_new ^ r_old;

        if (diff == 0)
        {
            return;
        }

        std::size_t index_changed = 0;

        while (index_changed < diff.size() && !diff.test(index_changed))
        {
            ++index_changed;
        }

        const auto sign_old = static_cast<int8_t>(-1 * static_cast<int8_t>(r_old[index_changed]));
        const auto sign_new = static_cast<int8_t>(-1 * static_cast<int8_t>(r_new[index_changed]));

        // the dependent SiDB is skipped: indices at or past it shift by one
        const auto sidb =
            dependent_sidb.has_value() && index_changed >= *dependent_sidb ? index_changed + 1 : index_changed;

        gray_code_history = {static_cast<int64_t>(sidb), sign_old};
        charge_distribution_state.assign_charge_state_by_index(sidb, model::sign_to_charge_state(sign_new));
    }
    /**
     * @brief Decodes the charge index into charge states, skipping the dependent SiDB.
     *
     * @param mode Whether leading zero digits are decoded or retain their charge states.
     */
    void index_to_charge_distribution(const charge_index_recomputation mode = charge_index_recomputation::FROM_SCRATCH)
    {
        // a charge index of zero corresponds to a layout with all SiDBs set to negative
        if (charge_index_value == 0)
        {
            assign_all_charge_states(model::charge_state::NEGATIVE);
            return;
        }

        const auto base        = static_cast<uint64_t>(charge_index_base);
        uint64_t   charge_quot = charge_index_value;
        auto       counter     = static_cast<int64_t>(num_sites) - 1;

        while (charge_quot > 0)
        {
            const auto sign = static_cast<int8_t>(static_cast<int64_t>(charge_quot % base) - 1);
            const auto cs   = model::sign_to_charge_state(sign);

            // the dependent SiDB is skipped
            if (dependent_sidb.has_value() && std::cmp_equal(counter, *dependent_sidb))
            {
                counter -= 1;
            }

            charge_distribution_state.assign_charge_state_by_index(static_cast<std::size_t>(counter), cs);

            charge_quot /= base;
            counter -= 1;
        }

        if (mode == charge_index_recomputation::IGNORE_LEADING_ZEROES)
        {
            return;
        }

        for (int64_t i = 0; i <= counter; ++i)
        {
            charge_distribution_state.assign_charge_state_by_index(static_cast<std::size_t>(i),
                                                                   model::charge_state::NEGATIVE);
        }
    }
    /**
     * @brief Decodes the sublayout index (base 3) and the layout index (base 2) into charge states and records every
     * SiDB that flipped, as QuickExact's incremental potential update needs.
     */
    void index_to_charge_distribution_tracked()
    {
        assert(num_sites > 1 && "There must be multiple SiDBs");

        changed_sidb_history.clear();
        changed_sidb_history.reserve(num_sites);

        if (sublayout_charge_index == 0)
        {
            for (const auto i : three_state_sidb_indices)
            {
                charge_distribution_state.assign_charge_state_by_index(i, model::charge_state::NEGATIVE);
            }
        }

        if (charge_index_value == 0)
        {
            for (const auto i : two_state_sidb_indices)
            {
                charge_distribution_state.assign_charge_state_by_index(i, model::charge_state::NEGATIVE);
            }
        }

        const auto flip = [this](const std::size_t i, const model::charge_state cs)
        {
            if (const auto old = charge_distribution_state.charge_states()[i]; old != cs)
            {
                changed_sidb_history.emplace_back(i, model::charge_state_to_sign(old));
                charge_distribution_state.assign_charge_state_by_index(i, cs);
            }
        };

        // first the sublayout of SiDBs that can be positively charged
        auto quot    = sublayout_charge_index;
        auto counter = static_cast<int64_t>(three_state_sidb_indices.size()) - 1;

        while (quot > 0)
        {
            const auto sign = static_cast<int8_t>(static_cast<int64_t>(quot % 3) - 1);
            flip(three_state_sidb_indices[static_cast<std::size_t>(counter)], model::sign_to_charge_state(sign));
            quot /= 3;
            counter -= 1;
        }

        // then the SiDBs that can only be negatively or neutrally charged
        quot    = charge_index_value;
        counter = static_cast<int64_t>(two_state_sidb_indices.size()) - 1;

        while (quot > 0)
        {
            const auto sign = static_cast<int8_t>(static_cast<int64_t>(quot % charge_index_base) - 1);
            flip(two_state_sidb_indices[static_cast<std::size_t>(counter)], model::sign_to_charge_state(sign));
            quot /= charge_index_base;
            counter -= 1;
        }
    }
};

}  // namespace fiction::sidb::simulation::detail
