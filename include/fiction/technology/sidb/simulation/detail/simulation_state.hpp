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
 * @brief The mutable working set of the SiDB simulators: charge states, local potentials, energy, and charge-index
 * enumeration over one potential landscape.
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
#include <cmath>
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
 * Whether the dependent SiDB adapts its charge state to the other SiDBs after a change.
 */
enum class dependent_cell_mode : uint8_t
{
    /**
     * The dependent SiDB keeps its charge state.
     */
    FIXED = 0,
    /**
     * The dependent SiDB takes the charge state its local potential dictates.
     */
    VARIABLE = 1
};
/**
 * Whether the energy is recomputed after a change.
 */
enum class energy_calculation : uint8_t
{
    /**
     * Keep the stored energy.
     */
    KEEP_OLD_ENERGY_VALUE = 0,
    /**
     * Recompute the energy.
     */
    UPDATE_ENERGY = 1
};
/**
 * Whether the local potentials are updated from the record of changed SiDBs or recomputed from scratch.
 */
enum class charge_distribution_history : uint8_t
{
    /**
     * Update only the contributions of the SiDBs the last index change flipped.
     */
    CONSIDER = 0,
    /**
     * Recompute every local potential.
     */
    NEGLECT = 1
};
/**
 * Whether the charge index follows a charge-state assignment.
 */
enum class charge_index_mode : uint8_t
{
    /**
     * Recompute the charge index.
     */
    UPDATE_CHARGE_INDEX = 0,
    /**
     * Keep the charge index.
     */
    KEEP_CHARGE_INDEX = 1
};
/**
 * Whether the charge states follow a charge-index assignment.
 */
enum class charge_distribution_mode : uint8_t
{
    /**
     * Decode the index into charge states.
     */
    UPDATE_CHARGE_DISTRIBUTION = 0,
    /**
     * Keep the charge states.
     */
    KEEP_CHARGE_DISTRIBUTION = 1
};
/**
 * How a charge index is decoded into charge states.
 */
enum class charge_index_recomputation : uint8_t
{
    /**
     * Decode every digit; leading zero digits become negative charge states.
     */
    FROM_SCRATCH = 0,
    /**
     * Decode only the non-zero digits.
     */
    IGNORE_LEADING_ZEROES = 1
};

/**
 * The mutable working set of the SiDB simulators over one `potential_landscape`: the charge state of every SiDB, the
 * local internal potentials, the energy, the validity flag, and the charge-index machinery the exhaustive engines
 * enumerate with (Gray codes, a dependent SiDB whose charge follows the others, and a sublayout of SiDBs that can be
 * positively charged). The landscape stays read-only, so worker threads share one landscape and own one state each.
 *
 * `snapshot()` yields the current `charge_distribution` in O(N).
 */
class simulation_state
{
  public:
    /**
     * How the energy is computed.
     */
    enum class energy_model : uint8_t
    {
        /**
         * The full energy: SiDB and defect charges in the internal and external potentials.
         */
        FULL,
        /**
         * The SiDB charges in the internal potential only, which is what QuickSim compares.
         */
        INTERNAL_ONLY
    };
    /**
     * How a charge index is decoded.
     */
    enum class index_decoding : uint8_t
    {
        /**
         * Decode the whole index.
         */
        PLAIN,
        /**
         * Decode the layout index and the sublayout index separately and record the flipped SiDBs, as QuickExact
         * needs.
         */
        TRACKED
    };
    /**
     * Creates a state over a landscape with every SiDB in one charge state and the potentials, energy, and validity
     * computed for it.
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
            land_{&land},
            n_{land.num_sidbs()},
            model_{model},
            decoding_{decoding},
            base_{land.params().base},
            index_base_{land.params().base},
            cd_{land.sites(), cs},
            local_int_pot_(n_, 0.0)
    {
        max_charge_index_ = max_index(base_, n_);
        charge_distribution_to_index();
        update_after_charge_change();
    }
    /**
     * The landscape.
     *
     * @return The landscape.
     */
    [[nodiscard]] const potential_landscape& landscape() const noexcept
    {
        return *land_;
    }
    /**
     * Number of SiDBs.
     *
     * @return Number of SiDBs.
     */
    [[nodiscard]] std::size_t num_sidbs() const noexcept
    {
        return n_;
    }
    /**
     * The base of the physical model, 2 or 3: whether positive charge states take part at all.
     *
     * @return The base.
     */
    [[nodiscard]] uint8_t base() const noexcept
    {
        return base_;
    }
    /**
     * The base the charge index is decoded in. It equals `base()` until a three-state sublayout is split off, from
     * then on the remaining SiDBs are decoded in base 2.
     *
     * @return The index base.
     */
    [[nodiscard]] uint8_t index_base() const noexcept
    {
        return index_base_;
    }

    // ----------------------------------------------------------------------------------------------- charge states

    /**
     * The charge state of an SiDB.
     *
     * @param i Index of the SiDB.
     * @return Charge state.
     */
    [[nodiscard]] model::charge_state get_charge_state_by_index(const std::size_t i) const noexcept
    {
        return cd_.get_charge_state_by_index(i);
    }
    /**
     * Assigns the charge state of an SiDB.
     *
     * @param i Index of the SiDB.
     * @param cs Charge state.
     * @param index_mode Whether to recompute the charge index.
     */
    void
    assign_charge_state_by_index(const std::size_t i, const model::charge_state cs,
                                 const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        cd_.assign_charge_state_by_index(i, cs);

        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            charge_distribution_to_index();
        }
    }
    /**
     * Assigns one charge state to every SiDB.
     *
     * @param cs Charge state.
     * @param index_mode Whether to recompute the charge index.
     */
    void assign_all_charge_states(const model::charge_state cs,
                                  const charge_index_mode index_mode = charge_index_mode::UPDATE_CHARGE_INDEX) noexcept
    {
        cd_.assign_all_charge_states(cs);

        if (index_mode == charge_index_mode::UPDATE_CHARGE_INDEX)
        {
            charge_distribution_to_index();
        }
    }
    /**
     * Number of negatively charged SiDBs.
     *
     * @return Number of negatively charged SiDBs.
     */
    [[nodiscard]] std::size_t num_negative_sidbs() const noexcept
    {
        return cd_.num_negative_sidbs();
    }
    /**
     * The current charge distribution with its energy.
     *
     * @return A copy of the charge states and the energy.
     */
    [[nodiscard]] charge_distribution snapshot() const
    {
        return cd_;
    }
    /**
     * The current charge distribution with its energy and the charge index recomputed in the plain base of the
     * landscape's parameters, as consumers of a simulation result expect.
     *
     * @return A copy of the charge states and the energy.
     */
    [[nodiscard]] const charge_distribution& current() const noexcept
    {
        return cd_;
    }

    // -------------------------------------------------------------------------------------------------- potentials

    /**
     * Updates the local internal potentials after charge states changed.
     *
     * @param history_mode Whether to apply only the recorded flips or to recompute from scratch.
     */
    void update_local_internal_potential(
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        if (history_mode == charge_distribution_history::NEGLECT)
        {
            for (std::size_t i = 0; i < n_; ++i)
            {
                double collect = 0.0;

                for (std::size_t j = 0; j < n_; ++j)
                {
                    collect += land_->chargeless_potential(i, j) *
                               static_cast<double>(model::charge_state_to_sign(cd_.get_charge_state_by_index(j)));
                }

                local_int_pot_[i] = land_->local_potential_caused_by_defects(i) + collect;
            }
        }
        else if (base_ == 2)
        {
            if (history_gray_code_.first != -1)
            {
                const auto changed     = static_cast<std::size_t>(history_gray_code_.first);
                const auto charge_diff = static_cast<double>(
                    model::charge_state_to_sign(cd_.get_charge_state_by_index(changed)) - history_gray_code_.second);

                for (std::size_t j = 0; j < n_; ++j)
                {
                    local_int_pot_[j] += land_->chargeless_potential(changed, j) * charge_diff;
                }
            }
        }
        else
        {
            for (const auto& [changed, old_sign] : history_)
            {
                const auto charge_diff =
                    static_cast<double>(model::charge_state_to_sign(cd_.get_charge_state_by_index(changed))) - old_sign;

                for (std::size_t j = 0; j < n_; ++j)
                {
                    local_int_pot_[j] += land_->chargeless_potential(changed, j) * charge_diff;
                }
            }
        }
    }
    /**
     * The local internal potential at an SiDB: charged SiDBs plus charged defects.
     *
     * @param i Index of the SiDB.
     * @return Local internal potential (unit: V).
     */
    [[nodiscard]] double local_internal_potential(const std::size_t i) const noexcept
    {
        return local_int_pot_[i];
    }
    /**
     * The local potential at an SiDB: internal plus external.
     *
     * @param i Index of the SiDB.
     * @return Local potential (unit: V).
     */
    [[nodiscard]] double local_potential(const std::size_t i) const noexcept
    {
        return local_int_pot_[i] + land_->local_external_potential(i);
    }
    /**
     * All local internal potentials.
     *
     * @return Local internal potential per SiDB (unit: V).
     */
    [[nodiscard]] const std::vector<double>& local_internal_potentials() const noexcept
    {
        return local_int_pot_;
    }
    /**
     * Overwrites the local internal potential at an SiDB.
     *
     * @param i Index of the SiDB.
     * @param pot Local internal potential (unit: V).
     */
    void assign_local_internal_potential(const std::size_t i, const double pot) noexcept
    {
        local_int_pot_[i] = pot;
    }

    // ---------------------------------------------------------------------------------------- energy and validity

    /**
     * Recomputes the energy from the current charge states and local internal potentials.
     */
    void recompute_energy() noexcept
    {
        if (model_ == energy_model::INTERNAL_ONLY)
        {
            double collect = 0.0;

            for (std::size_t i = 0; i < n_; ++i)
            {
                collect += local_int_pot_[i] *
                           static_cast<double>(model::charge_state_to_sign(cd_.get_charge_state_by_index(i)));
            }

            cd_.assign_energy(0.5 * collect);

            return;
        }

        cd_.assign_energy(land_->energy(cd_, local_int_pot_));
    }
    /**
     * The energy of the current charge states.
     *
     * @return Energy (unit: eV).
     */
    [[nodiscard]] double energy() const noexcept
    {
        return cd_.energy();
    }
    /**
     * Updates potentials, the dependent SiDB, the energy, and the validity after charge states changed.
     *
     * @param dep_cell Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void update_after_charge_change(
        const dependent_cell_mode         dep_cell     = dependent_cell_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        update_local_internal_potential(history_mode);

        if (dep_cell == dependent_cell_mode::VARIABLE)
        {
            update_charge_state_of_dependent_cell();
        }
        if (energy_mode == energy_calculation::UPDATE_ENERGY)
        {
            recompute_energy();
        }

        validity_check();
    }
    /**
     * Recomputes the validity flag: population stability of every SiDB, then configuration stability.
     */
    void validity_check() noexcept
    {
        valid_ =
            land_->is_population_stable(cd_, local_int_pot_) && land_->is_configuration_stable(cd_, local_int_pot_);
    }
    /**
     * Whether the current charge states are physically valid, as of the last `validity_check`.
     *
     * @return `true` iff physically valid.
     */
    [[nodiscard]] bool is_physically_valid() const noexcept
    {
        return valid_;
    }
    /**
     * Whether no charge hop lowers the energy of the current charge states.
     *
     * @return `true` iff configuration stable.
     */
    [[nodiscard]] bool is_configuration_stable() const noexcept
    {
        return land_->is_configuration_stable(cd_, local_int_pot_);
    }
    /**
     * Marks the current charge states physically valid without checking.
     */
    void declare_physically_valid() noexcept
    {
        valid_ = true;
    }

    // ------------------------------------------------------------------------------------------------ charge index

    /**
     * Recomputes the charge index over every SiDB in the plain base, ignoring the dependent SiDB and the sublayout.
     */
    void charge_distribution_to_index_general() noexcept
    {
        charge_index_ = cd_.charge_index(base_);
    }
    /**
     * Recomputes the charge index (and the sublayout index) from the charge states, skipping the dependent SiDB and
     * splitting the SiDBs that can be positively charged into their own base-3 index.
     */
    void charge_distribution_to_index() noexcept
    {
        uint64_t index     = 0;
        uint64_t sub_index = 0;

        const auto digit = [this](const std::size_t i)
        { return static_cast<uint64_t>(model::charge_state_to_sign(cd_.get_charge_state_by_index(i)) + int8_t{1}); };

        if (!three_state_.empty())
        {
            for (const auto i : three_state_)
            {
                sub_index = (sub_index * 3) + digit(i);
            }
            for (const auto i : two_state_)
            {
                if (!dependent_.has_value() || i != *dependent_)
                {
                    index = (index * 2) + digit(i);
                }
            }
        }
        else
        {
            for (std::size_t i = 0; i < n_; ++i)
            {
                if (!dependent_.has_value() || i != *dependent_)
                {
                    index = (index * base_) + digit(i);
                }
            }
        }

        charge_index_     = index;
        sub_charge_index_ = sub_index;
    }
    /**
     * The charge index.
     *
     * @return The charge index.
     */
    [[nodiscard]] uint64_t charge_index() const noexcept
    {
        return charge_index_;
    }
    /**
     * The largest charge index over the SiDBs the index enumerates.
     *
     * @return The maximum charge index.
     */
    [[nodiscard]] uint64_t max_charge_index() const noexcept
    {
        return max_charge_index_;
    }
    /**
     * The charge index of the sublayout of SiDBs that can be positively charged.
     *
     * @return The sublayout charge index.
     */
    [[nodiscard]] uint64_t charge_index_of_sub_layout() const noexcept
    {
        return sub_charge_index_;
    }
    /**
     * The largest sublayout charge index.
     *
     * @return The maximum sublayout charge index.
     */
    [[nodiscard]] uint64_t max_charge_index_sub_layout() const noexcept
    {
        return max_sub_charge_index_;
    }
    /**
     * Assigns a charge index.
     *
     * @param index Charge index, at most `max_charge_index()`.
     * @param mode Whether to decode the index into charge states.
     */
    void assign_charge_index(const uint64_t index, const charge_distribution_mode mode =
                                                       charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION) noexcept
    {
        assert(index <= max_charge_index_ && "charge index is too large");

        charge_index_ = index;

        if (mode == charge_distribution_mode::UPDATE_CHARGE_DISTRIBUTION)
        {
            index_to_charge_distribution();
        }
    }
    /**
     * Steps the charge index up by one and decodes it.
     *
     * @param dep_cell Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void increase_charge_index_by_one(
        const dependent_cell_mode         dep_cell     = dependent_cell_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        if (charge_index_ >= max_charge_index_)
        {
            return;
        }

        charge_index_ += 1;

        if (decoding_ == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
        }

        update_after_charge_change(dep_cell, energy_mode, history_mode);
    }
    /**
     * Steps the sublayout charge index up by one and decodes it.
     *
     * @param dep_cell Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flips to the potentials.
     */
    void increase_charge_index_of_sub_layout_by_one(
        const dependent_cell_mode         dep_cell     = dependent_cell_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        if (sub_charge_index_ >= max_sub_charge_index_)
        {
            return;
        }

        sub_charge_index_ += 1;

        if (decoding_ == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution(charge_index_recomputation::IGNORE_LEADING_ZEROES);
        }

        update_after_charge_change(dep_cell, energy_mode, history_mode);
    }
    /**
     * Resets the sublayout charge index to zero and decodes it with the flips recorded.
     */
    void reset_charge_index_sub_layout() noexcept
    {
        sub_charge_index_ = 0;

        if (decoding_ == index_decoding::TRACKED)
        {
            index_to_charge_distribution_tracked();
        }
        else
        {
            index_to_charge_distribution();
        }

        update_after_charge_change(dependent_cell_mode::VARIABLE, energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                   charge_distribution_history::CONSIDER);
    }
    /**
     * Assigns a charge index given as a Gray code and flips the one SiDB that differs from the previous Gray code.
     *
     * @param current_gray_code New charge index.
     * @param previous_gray_code Previous charge index.
     * @param dep_cell Whether the dependent SiDB adapts its charge state.
     * @param energy_mode Whether to recompute the energy.
     * @param history_mode Whether to apply only the recorded flip to the potentials.
     */
    void assign_charge_index_by_gray_code(
        const uint64_t current_gray_code, const uint64_t previous_gray_code,
        const dependent_cell_mode         dep_cell     = dependent_cell_mode::FIXED,
        const energy_calculation          energy_mode  = energy_calculation::UPDATE_ENERGY,
        const charge_distribution_history history_mode = charge_distribution_history::NEGLECT) noexcept
    {
        if (current_gray_code <= max_charge_index_)
        {
            charge_index_ = current_gray_code;
            gray_code_to_charge_distribution(current_gray_code, previous_gray_code);
            update_after_charge_change(dep_cell, energy_mode, history_mode);
        }
    }

    // ---------------------------------------------------------------------------------- dependent SiDB, sublayout

    /**
     * Declares the SiDB whose charge state follows the other SiDBs; the charge index then enumerates the others.
     *
     * @param i Index of the dependent SiDB.
     */
    void assign_dependent_cell(const std::size_t i) noexcept
    {
        assert(i < n_ && "dependent cell is not part of the layout");

        dependent_        = i;
        max_charge_index_ = max_index(base_, n_ - 1);
    }
    /**
     * The dependent SiDB, if any.
     *
     * @return Index of the dependent SiDB.
     */
    [[nodiscard]] std::optional<std::size_t> dependent_cell() const noexcept
    {
        return dependent_;
    }
    /**
     * Sets the base of the charge index and the maximum index accordingly.
     *
     * @param base 2 or 3.
     */
    void assign_base_number(const uint8_t base) noexcept
    {
        base_             = base;
        index_base_       = base;
        max_charge_index_ = max_index(base, dependent_.has_value() ? n_ - 1 : n_);
    }
    /**
     * Determines from the all-negative charge distribution which SiDBs can become positively charged and, if any can,
     * switches to a base-3 sublayout index over them and a base-2 index over the rest.
     *
     * @return `true` iff some SiDB other than the dependent one can be positively charged.
     */
    [[nodiscard]] bool is_three_state_simulation_required() noexcept
    {
        update_after_charge_change();

        three_state_.clear();
        two_state_.clear();
        dependent_in_sub_layout_ = false;

        assert(num_negative_sidbs() == n_ && "All SiDBs have to be negatively charged");

        bool required = false;

        for (std::size_t i = 0; i < n_; ++i)
        {
            if (-local_int_pot_[i] <
                land_->effective_charge_transition_thresholds(
                    i)[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
            {
                continue;
            }

            if (dependent_.has_value() && i == *dependent_)
            {
                dependent_in_sub_layout_ = true;
            }
            else
            {
                three_state_.push_back(i);
                required = true;
            }
        }

        for (std::size_t i = 0; i < n_; ++i)
        {
            if (!std::ranges::binary_search(three_state_, i) && (!dependent_.has_value() || i != *dependent_))
            {
                two_state_.push_back(i);
            }
        }

        if (required)
        {
            assign_base_number_to_three();
        }

        return required;
    }
    /**
     * The SiDBs that can be positively charged, ascending.
     *
     * @return Indices of the sublayout SiDBs.
     */
    [[nodiscard]] const std::vector<std::size_t>& three_state_sidbs() const noexcept
    {
        return three_state_;
    }
    /**
     * The SiDBs that can only be negatively or neutrally charged, ascending, without the dependent SiDB.
     *
     * @return Indices of the two-state SiDBs.
     */
    [[nodiscard]] const std::vector<std::size_t>& two_state_sidbs() const noexcept
    {
        return two_state_;
    }

    // ---------------------------------------------------------------------------------------------- QuickSim helpers

    /**
     * Reseeds the random generator of `adjacent_search` from `std::random_device`. A state copied into a worker
     * thread calls this so the threads do not replay one random sequence.
     */
    void reseed() noexcept
    {
        rng_.seed(std::random_device{}());
    }

    /**
     * The SiDBs whose local potential forces them negative regardless of the other charges, i.e., whose (0/-)
     * transition lies above the Fermi level even at the current potentials.
     *
     * @return Indices of the SiDBs that have to be negatively charged.
     */
    [[nodiscard]] std::vector<std::size_t> negative_sidb_detection() const noexcept
    {
        std::vector<std::size_t> negative{};
        negative.reserve(n_);

        for (std::size_t i = 0; i < n_; ++i)
        {
            if ((-local_potential(i) + land_->params().mu_minus) < -utils::math::ERROR_MARGIN)
            {
                negative.push_back(i);
            }
        }

        return negative;
    }
    /**
     * QuickSim's max-min diversity step: among the neutral SiDBs, picks one at random that is at least `alpha` times
     * the maximal distance away from every negative SiDB, charges it negatively, and updates the potentials and the
     * energy incrementally.
     *
     * @param alpha Fraction of the maximal distance a candidate has to keep from the negative SiDBs.
     * @param negative_indices Indices of the negative SiDBs; the chosen SiDB is appended.
     */
    void adjacent_search(const double alpha, std::vector<std::size_t>& negative_indices) noexcept
    {
        double     dist_max     = 0.0;
        const auto reserve_size = n_ - negative_indices.size();

        std::vector<std::size_t> index_vector{};
        index_vector.reserve(reserve_size);
        std::vector<double> distance{};
        distance.reserve(reserve_size);

        for (std::size_t unocc = 0; unocc < n_; ++unocc)
        {
            if (cd_.get_charge_state_by_index(unocc) != model::charge_state::NEUTRAL)
            {
                continue;
            }

            const auto dist_min = std::accumulate(
                negative_indices.cbegin(), negative_indices.cend(), std::numeric_limits<double>::infinity(),
                [&](const double acc, const std::size_t occ) { return std::min(acc, land_->nm_distance(unocc, occ)); });

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

        const auto chosen = index_vector[candidates[dist(rng_)]];

        cd_.assign_charge_state_by_index(chosen, model::charge_state::NEGATIVE);
        negative_indices.push_back(chosen);

        cd_.assign_energy(cd_.energy() - local_int_pot_[chosen]);

        for (std::size_t i = 0; i < n_; ++i)
        {
            local_int_pot_[i] -= land_->chargeless_potential(i, chosen);
        }
    }

  private:
    /**
     * The landscape.
     */
    const potential_landscape* land_;
    /**
     * Number of SiDBs.
     */
    std::size_t n_;
    /**
     * Energy model.
     */
    energy_model model_;
    /**
     * Index decoding.
     */
    index_decoding decoding_;
    /**
     * Base of the physical model.
     */
    uint8_t base_;
    /**
     * Base the charge index is decoded in.
     */
    uint8_t index_base_;
    /**
     * Charge states and energy.
     */
    charge_distribution cd_;
    /**
     * Local internal potential per SiDB (unit: V).
     */
    std::vector<double> local_int_pot_;
    /**
     * Validity flag.
     */
    bool valid_{false};
    /**
     * Charge index over the SiDBs that are not in the sublayout and not the dependent SiDB.
     */
    uint64_t charge_index_{0};
    /**
     * Charge index over the sublayout.
     */
    uint64_t sub_charge_index_{0};
    /**
     * Maximum charge index.
     */
    uint64_t max_charge_index_{0};
    /**
     * Maximum sublayout charge index.
     */
    uint64_t max_sub_charge_index_{0};
    /**
     * The dependent SiDB.
     */
    std::optional<std::size_t> dependent_{};
    /**
     * Whether the dependent SiDB can be positively charged.
     */
    bool dependent_in_sub_layout_{false};
    /**
     * SiDBs that can be positively charged, ascending.
     */
    std::vector<std::size_t> three_state_{};
    /**
     * SiDBs that cannot be positively charged, ascending, without the dependent SiDB.
     */
    std::vector<std::size_t> two_state_{};
    /**
     * The SiDB the last Gray-code step flipped and its previous sign; -1 if none.
     */
    std::pair<int64_t, int8_t> history_gray_code_{-1, 0};
    /**
     * The SiDBs the last tracked decoding flipped with their previous signs.
     */
    std::vector<std::pair<std::size_t, int8_t>> history_{};
    /**
     * Random generator of `adjacent_search`; one per state, so worker threads never share it.
     */
    std::mt19937_64 rng_{std::random_device{}()};
    /**
     * The largest index of `digits` digits in `base`.
     */
    [[nodiscard]] static uint64_t max_index(const uint8_t base, const std::size_t digits) noexcept
    {
        return static_cast<uint64_t>(std::pow(static_cast<double>(base), static_cast<double>(digits))) - 1;
    }
    /**
     * Switches to a base-3 sublayout index over the SiDBs that can be positively charged.
     */
    void assign_base_number_to_three() noexcept
    {
        base_       = 3;
        index_base_ = 2;

        if (dependent_.has_value())
        {
            if (!three_state_.empty())
            {
                max_charge_index_     = max_index(2, n_ - 1 - three_state_.size());
                max_sub_charge_index_ = max_index(3, three_state_.size());
            }
        }
        else
        {
            max_charge_index_     = max_index(3, n_);
            max_sub_charge_index_ = max_index(3, three_state_.size());
        }

        if (max_charge_index_ == 0)
        {
            assign_charge_index(0);
        }
    }
    /**
     * Sets the dependent SiDB to the charge state its local potential dictates and updates the potentials of the
     * others incrementally.
     */
    void update_charge_state_of_dependent_cell() noexcept
    {
        if (!dependent_.has_value())
        {
            return;
        }

        const auto  d            = *dependent_;
        const auto  loc_pot_cell = -local_int_pot_[d];
        const auto& t            = land_->effective_charge_transition_thresholds(d);
        const auto  current      = cd_.get_charge_state_by_index(d);

        const auto switch_to = [&](const model::charge_state cs)
        {
            const auto charge_diff = static_cast<double>(model::charge_state_to_sign(cs)) -
                                     static_cast<double>(model::charge_state_to_sign(current));

            for (std::size_t i = 0; i < n_; ++i)
            {
                if (i != d)
                {
                    local_int_pot_[i] += land_->chargeless_potential(i, d) * charge_diff;
                }
            }

            cd_.assign_charge_state_by_index(d, cs);
        };

        if (loc_pot_cell < t[static_cast<std::size_t>(charge_transition_threshold_bounds::NEGATIVE_UPPER_BOUND)])
        {
            if (current != model::charge_state::NEGATIVE)
            {
                switch_to(model::charge_state::NEGATIVE);
            }
        }
        else if (loc_pot_cell > t[static_cast<std::size_t>(charge_transition_threshold_bounds::POSITIVE_LOWER_BOUND)])
        {
            // the dependent SiDB can only be positively charged in a three-state simulation
            if ((index_base_ == 3 || !three_state_.empty()) && current != model::charge_state::POSITIVE)
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
     * Flips the one SiDB in which two Gray codes differ and records it for the incremental potential update.
     */
    void gray_code_to_charge_distribution(const uint64_t new_gray_code, const uint64_t old_gray_code) noexcept
    {
        history_gray_code_ = {-1, 0};

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
        const auto sidb = dependent_.has_value() && index_changed >= *dependent_ ? index_changed + 1 : index_changed;

        history_gray_code_ = {static_cast<int64_t>(sidb), sign_old};
        cd_.assign_charge_state_by_index(sidb, model::sign_to_charge_state(sign_new));
    }
    /**
     * Decodes the charge index into charge states, skipping the dependent SiDB.
     */
    void index_to_charge_distribution(
        const charge_index_recomputation mode = charge_index_recomputation::FROM_SCRATCH) noexcept
    {
        // a charge index of zero corresponds to a layout with all SiDBs set to negative
        if (charge_index_ == 0)
        {
            assign_all_charge_states(model::charge_state::NEGATIVE);
            return;
        }

        const auto base        = static_cast<uint64_t>(index_base_);
        uint64_t   charge_quot = charge_index_;
        auto       counter     = static_cast<int64_t>(n_) - 1;

        while (charge_quot > 0)
        {
            const auto cs = model::sign_to_charge_state(static_cast<int8_t>(charge_quot % base) - 1);

            // the dependent SiDB is skipped
            if (dependent_.has_value() && counter == static_cast<int64_t>(*dependent_))
            {
                counter -= 1;
            }

            cd_.assign_charge_state_by_index(static_cast<std::size_t>(counter), cs);

            charge_quot /= base;
            counter -= 1;
        }

        if (mode == charge_index_recomputation::IGNORE_LEADING_ZEROES)
        {
            return;
        }

        for (int64_t i = 0; i <= counter; ++i)
        {
            cd_.assign_charge_state_by_index(static_cast<std::size_t>(i), model::charge_state::NEGATIVE);
        }
    }
    /**
     * Decodes the sublayout index (base 3) and the layout index (base 2) into charge states and records every SiDB
     * that flipped, as QuickExact's incremental potential update needs.
     */
    void index_to_charge_distribution_tracked() noexcept
    {
        assert(n_ > 1 && "There must be multiple SiDBs");

        history_.clear();
        history_.reserve(n_);

        if (sub_charge_index_ == 0)
        {
            for (const auto i : three_state_)
            {
                cd_.assign_charge_state_by_index(i, model::charge_state::NEGATIVE);
            }
        }

        if (charge_index_ == 0)
        {
            for (const auto i : two_state_)
            {
                cd_.assign_charge_state_by_index(i, model::charge_state::NEGATIVE);
            }
        }

        const auto flip = [this](const std::size_t i, const model::charge_state cs)
        {
            if (const auto old = cd_.get_charge_state_by_index(i); old != cs)
            {
                history_.emplace_back(i, model::charge_state_to_sign(old));
                cd_.assign_charge_state_by_index(i, cs);
            }
        };

        // first the sublayout of SiDBs that can be positively charged
        auto quot    = sub_charge_index_;
        auto counter = static_cast<int64_t>(three_state_.size()) - 1;

        while (quot > 0)
        {
            flip(three_state_[static_cast<std::size_t>(counter)],
                 model::sign_to_charge_state(static_cast<int8_t>(quot % 3) - 1));
            quot /= 3;
            counter -= 1;
        }

        // then the SiDBs that can only be negatively or neutrally charged
        quot    = charge_index_;
        counter = static_cast<int64_t>(two_state_.size()) - 1;

        while (quot > 0)
        {
            flip(two_state_[static_cast<std::size_t>(counter)],
                 model::sign_to_charge_state(static_cast<int8_t>(quot % index_base_) - 1));
            quot /= index_base_;
            counter -= 1;
        }
    }
};

}  // namespace fiction::sidb::simulation::detail
