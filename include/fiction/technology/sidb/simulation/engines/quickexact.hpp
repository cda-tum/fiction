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
 * @brief *QuickExact*, exact SiDB simulation with physically informed pruning.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/model/simulation_parameters.hpp"
#include "fiction/technology/sidb/simulation/detail/simulation_state.hpp"
#include "fiction/technology/sidb/simulation/potential_landscape.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/math/gray_code_iterator.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::sidb::simulation::engines
{

/**
 * This struct stores the parameters for the *QuickExact* algorithm.
 */
struct quickexact_params
{
    /**
     * Modes to use for the *QuickExact* algorithm.
     */
    enum class automatic_base_number_detection : uint8_t
    {
        /**
         * Simulation is conducted with the required base number (i.e., if positively charged SiDBs can occur, three
         * state simulation is conducted).
         */
        ON,
        /**
         * The base number from the physical parameters is used for the simulation.
         */
        OFF
    };
    /**
     * All parameters for physical SiDB simulations.
     */
    sidb::model::simulation_parameters sim_params{};
    /**
     * If `ON`, *QuickExact* checks which base number is required for the simulation, i.e., whether positively charged
     * SiDBs can occur. If `OFF`, the base number from the physical parameters is used.
     */
    automatic_base_number_detection base_number_detection = automatic_base_number_detection::ON;
    /**
     * Local external electrostatic potentials (e.g., locally applied electrodes), per site (unit: V).
     */
    std::unordered_map<lattice_site, double> local_external_potential = {};
    /**
     * Global external electrostatic potential (unit: V). Value is applied on each SiDB.
     */
    double global_potential = 0;
};

namespace detail
{

/**
 * Runs *QuickExact* on one layout.
 */
class quickexact_impl
{
  public:
    /**
     * Builds the potential landscape of the layout and the all-negative starting state.
     *
     * @param lyt Layout to simulate.
     * @param parameter *QuickExact* parameters.
     */
    quickexact_impl(const layout& lyt, const quickexact_params& parameter) :
            params{parameter},
            landscape{lyt, parameter.sim_params, parameter.local_external_potential, parameter.global_potential},
            state{landscape, model::charge_state::NEGATIVE}
    {}
    /**
     * Runs the simulation.
     *
     * @return The physically valid charge distributions of the layout.
     */
    [[nodiscard]] result run() noexcept
    {
        sim_result.algorithm_name = "QuickExact";
        sim_result.sim_params     = params.sim_params;
        sim_result.lyt            = landscape.get_layout();
        sim_result.additional_simulation_parameters.emplace("global_potential", params.global_potential);

        mockturtle::stopwatch<>::duration time_counter{};
        {
            const mockturtle::stopwatch stop{time_counter};

            // Determine if three state simulation (i.e., positively charged SiDBs can occur) is required.
            const auto base_number =
                (params.base_number_detection == quickexact_params::automatic_base_number_detection::ON &&
                 state.is_three_state_simulation_required()) ||
                        (params.base_number_detection == quickexact_params::automatic_base_number_detection::OFF &&
                         params.sim_params.base == 3) ?
                    required_simulation_base_number::THREE :
                    required_simulation_base_number::TWO;

            preassigned_negative_sidbs = state.negative_sidb_detection();

            const auto number_of_sidbs = landscape.num_sidbs();

            if (number_of_sidbs > 1)
            {
                if (preassigned_negative_sidbs.size() < number_of_sidbs)
                {
                    simulate_without_preassigned_negative_sidbs(base_number);
                }
                // If the layout consists of only pre-assigned negatively charged SiDBs
                // (i.e., only SiDBs that are far away from each other).
                else
                {
                    sim_result.charge_distributions.push_back(state.snapshot());
                }
            }
            // If there is only one SiDB in the layout, this single SiDB can be neutrally or even positively charged due
            // to external potentials or defects.
            else if (number_of_sidbs == 1)
            {
                state.assign_base_number(base_number == required_simulation_base_number::THREE ? 3 : 2);

                // A check is performed to see if the charge index is still below the maximum charge index. If not, the
                // charge index is increased and the corresponding charge distribution is checked for physical validity.
                while (state.charge_index() < state.max_charge_index())
                {
                    if (state.is_physically_valid())
                    {
                        sim_result.charge_distributions.push_back(state.snapshot());
                    }

                    // `dependent_cell_mode::VARIABLE` allows that the charge state of the dependent cell is
                    // automatically changed based on the new charge distribution.
                    state.increase_charge_index_by_one(simulation::detail::dependent_cell_mode::VARIABLE);
                }

                if (state.is_physically_valid())
                {
                    sim_result.charge_distributions.push_back(state.snapshot());
                }
            }
        }

        sim_result.simulation_runtime = time_counter;

        return sim_result;
    }

  private:
    /**
     * The required base number for the simulation.
     */
    enum class required_simulation_base_number : uint8_t
    {
        /**
         * Two state simulation (i.e., negative and neutral).
         */
        TWO,
        /**
         * Three state simulation (i.e., negative, neutral, and positive).
         */
        THREE
    };
    /**
     * Parameters used for the simulation.
     */
    quickexact_params params{};
    /**
     * Landscape of the full layout.
     */
    potential_landscape landscape;
    /**
     * State over the full layout. It stays all-negative except for the single-SiDB case.
     */
    simulation::detail::simulation_state state;
    /**
     * SiDBs that have to be negatively charged in every physically valid distribution.
     */
    std::vector<std::size_t> preassigned_negative_sidbs{};
    /**
     * SiDBs the simulation enumerates, i.e., every SiDB that is not pre-assigned negative.
     */
    std::vector<std::size_t> free_sidbs{};
    /**
     * Simulation results.
     */
    result sim_result{};
    /**
     * Simulates the layout with the pre-assigned negative SiDBs removed and modeled as negative charges the remaining
     * SiDBs see, which shrinks the enumerated space. The external potential of the removed SiDBs is carried over, so
     * the energy of the reduced system equals that of the full one.
     *
     * @param base_number Whether positively charged SiDBs can occur.
     */
    void simulate_without_preassigned_negative_sidbs(const required_simulation_base_number base_number) noexcept
    {
        const auto& sites = *landscape.sites();

        layout reduced{landscape.get_layout()};
        auto   local_external_potential = params.local_external_potential;

        for (const auto i : preassigned_negative_sidbs)
        {
            reduced.assign_cell_type(sites[i], sidb_technology::cell_type::EMPTY);
            // IMPORTANT: The pre-assigned negatively charged SiDBs (they have to be negatively charged to
            // fulfill the population stability) are considered as negatively charged defects in the layout.
            reduced.assign_defect(sites[i], model::defect{model::defect_type::UNKNOWN, -1, params.sim_params.epsilon_r,
                                                          params.sim_params.lambda_tf});
            local_external_potential[sites[i]] += params.global_potential;
        }

        for (std::size_t i = 0; i < landscape.num_sidbs(); ++i)
        {
            if (!std::ranges::binary_search(preassigned_negative_sidbs, i))
            {
                free_sidbs.push_back(i);
            }
        }

        const potential_landscape reduced_landscape{reduced, params.sim_params, local_external_potential,
                                                    params.global_potential};

        simulation::detail::simulation_state reduced_state{
            reduced_landscape, model::charge_state::NEUTRAL, simulation::detail::simulation_state::energy_model::FULL,
            simulation::detail::simulation_state::index_decoding::TRACKED};

        reduced_state.update_after_charge_change(simulation::detail::dependent_cell_mode::FIXED);
        reduced_state.assign_dependent_cell(0);

        // Update all local potentials, system energy, and physical validity. The flag is set to
        // `dependent_cell_mode::VARIABLE` to allow the dependent cell to change its charge state based on the N-1 SiDBs
        // to fulfill the local population stability at its position.
        reduced_state.update_after_charge_change(simulation::detail::dependent_cell_mode::VARIABLE);

        if (base_number == required_simulation_base_number::TWO)
        {
            sim_result.additional_simulation_parameters.emplace("base_number", uint64_t{2});
            two_state_simulation(reduced_state);
        }
        // If positively charged SiDBs can occur in the layout, 3-state simulation is conducted.
        else
        {
            sim_result.additional_simulation_parameters.emplace("base_number", uint64_t{3});
            three_state_simulation(reduced_state);
        }
    }
    /**
     * Records the current charge states of the reduced state as a distribution over the full layout. The pre-assigned
     * SiDBs are negative; the energy is that of the reduced system, which equals the full one.
     *
     * @param reduced_state State over the reduced layout.
     */
    void record(const simulation::detail::simulation_state& reduced_state)
    {
        charge_distribution cd{landscape.sites(), model::charge_state::NEGATIVE};

        for (std::size_t k = 0; k < free_sidbs.size(); ++k)
        {
            cd.assign_charge_state_by_index(free_sidbs[k], reduced_state.get_charge_state_by_index(k));
        }

        cd.assign_energy(
            reduced_state.landscape().energy(reduced_state.current(), reduced_state.local_internal_potentials()));

        sim_result.charge_distributions.push_back(std::move(cd));
    }
    /**
     * Enumerates every two-state charge distribution of the reduced layout as a Gray code and records the physically
     * valid ones.
     *
     * @param reduced_state State over the reduced layout.
     */
    void two_state_simulation(simulation::detail::simulation_state& reduced_state) noexcept
    {
        reduced_state.assign_base_number(2);

        uint64_t previous_charge_index = 0;

        fiction::utils::math::gray_code_iterator gci{0};

        for (gci = 0; gci <= reduced_state.max_charge_index(); ++gci)
        {
            reduced_state.assign_charge_index_by_gray_code(
                *gci, previous_charge_index, simulation::detail::dependent_cell_mode::VARIABLE,
                simulation::detail::energy_calculation::KEEP_OLD_ENERGY_VALUE,
                simulation::detail::charge_distribution_history::CONSIDER);

            previous_charge_index = *gci;

            if (reduced_state.is_physically_valid())
            {
                record(reduced_state);
            }
        }
    }
    /**
     * Enumerates every charge distribution of the reduced layout with the SiDBs that can be positively charged in
     * base 3 and the rest in base 2, and records the physically valid ones.
     *
     * @param reduced_state State over the reduced layout.
     */
    void three_state_simulation(simulation::detail::simulation_state& reduced_state) noexcept
    {
        reduced_state.assign_all_charge_states(model::charge_state::NEGATIVE);
        reduced_state.update_after_charge_change();
        // Not executed to detect if 3-state simulation is required, but to detect the SiDBs that could be positively
        // charged (important to speed up the simulation).
        [[maybe_unused]] const auto required = reduced_state.is_three_state_simulation_required();
        reduced_state.update_after_charge_change(simulation::detail::dependent_cell_mode::VARIABLE);

        while (reduced_state.charge_index() < reduced_state.max_charge_index())
        {
            while (reduced_state.charge_index_of_sub_layout() < reduced_state.max_charge_index_sub_layout())
            {
                if (reduced_state.is_physically_valid())
                {
                    record(reduced_state);
                }

                reduced_state.increase_charge_index_of_sub_layout_by_one(
                    simulation::detail::dependent_cell_mode::VARIABLE,
                    simulation::detail::energy_calculation::KEEP_OLD_ENERGY_VALUE,
                    simulation::detail::charge_distribution_history::CONSIDER);
            }

            if (reduced_state.is_physically_valid())
            {
                record(reduced_state);
            }

            if (reduced_state.max_charge_index_sub_layout() != 0)
            {
                reduced_state.reset_charge_index_sub_layout();
            }

            reduced_state.increase_charge_index_by_one(simulation::detail::dependent_cell_mode::VARIABLE,
                                                       simulation::detail::energy_calculation::KEEP_OLD_ENERGY_VALUE);
        }

        // charge configurations of the sublayout are iterated
        while (reduced_state.charge_index_of_sub_layout() < reduced_state.max_charge_index_sub_layout())
        {
            if (reduced_state.is_physically_valid())
            {
                record(reduced_state);
            }

            reduced_state.increase_charge_index_of_sub_layout_by_one(
                simulation::detail::dependent_cell_mode::VARIABLE,
                simulation::detail::energy_calculation::KEEP_OLD_ENERGY_VALUE,
                simulation::detail::charge_distribution_history::CONSIDER);
        }

        if (reduced_state.is_physically_valid())
        {
            record(reduced_state);
        }
    }
};

}  // namespace detail

/**
 * *QuickExact* is a quick and exact physical simulation algorithm designed specifically for SiDB layouts. It was
 * proposed in \"The Need for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic\" by J. Drewniok, M.
 * Walter, and R. Wille in ASP-DAC 2024 (https://ieeexplore.ieee.org/document/10473946). It determines all physically
 * valid charge configurations of a given SiDB layout, providing a significant performance advantage of more than three
 * orders of magnitude over *ExGS*.
 *
 * The performance improvement stems from the use of physically informed search space pruning: SiDBs whose local
 * potential forces them negative are removed from the enumeration and modeled as fixed negative charges, one SiDB's
 * charge state follows from the others, and positively charged SiDBs are enumerated only where they can occur.
 * Charged surface defects of the layout and external potentials enter the potential landscape.
 *
 * @param lyt Layout to simulate.
 * @param params Parameter required for the simulation.
 * @return Simulation result: every physically valid charge distribution of `lyt`.
 */
[[nodiscard]] inline result quickexact(const layout& lyt, const quickexact_params& params = {}) noexcept
{
    detail::quickexact_impl p{lyt, params};

    return p.run();
}

}  // namespace fiction::sidb::simulation::engines
