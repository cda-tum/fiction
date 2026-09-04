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
 * @brief Tests for `fiction/technology/sidb/simulation/detail/simulation_state.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/detail/simulation_state.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>
#include <fiction/technology/sidb/technology.hpp>

#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace Catch::Matchers;

namespace
{

layout three_sidbs()
{
    layout lyt{};
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_technology::cell_type::NORMAL);
    return lyt;
}

}  // namespace

TEST_CASE("Simulation state over a landscape", "[simulation-state]")
{
    const auto                lyt = three_sidbs();
    const potential_landscape land{lyt, simulation_parameters{2, -0.32}};

    SECTION("initial state matches the landscape's evaluation")
    {
        const detail::simulation_state state{land, charge_state::NEGATIVE};

        CHECK(state.num_sidbs() == 3);
        CHECK(state.base() == 2);
        CHECK(state.index_base() == 2);
        CHECK(state.charge_index() == 0);
        CHECK(state.max_charge_index() == 7);
        CHECK(state.num_negative_sidbs() == 3);

        const auto cd = land.evaluate(charge_distribution{lyt, charge_state::NEGATIVE});
        CHECK_THAT(state.energy(), WithinAbs(cd.energy(), 1E-12));
        CHECK(state.is_physically_valid() == land.is_physically_valid(cd));
        CHECK(state.snapshot() == cd);

        const auto pot = land.local_internal_potentials(cd);
        for (std::size_t i = 0; i < 3; ++i)
        {
            CHECK_THAT(state.local_internal_potential(i), WithinAbs(pot[i], 1E-12));
        }
    }
    SECTION("enumerating every charge index visits every distribution once")
    {
        detail::simulation_state state{land, charge_state::NEGATIVE};

        std::set<uint64_t> seen{state.snapshot().charge_index(2)};

        while (state.charge_index() < state.max_charge_index())
        {
            state.increase_charge_index_by_one();
            CHECK(seen.insert(state.snapshot().charge_index(2)).second);
            CHECK(state.snapshot().charge_index(2) == state.charge_index());
        }

        CHECK(seen.size() == 8);
    }
    SECTION("physical validity and energy agree with the landscape along the enumeration")
    {
        detail::simulation_state state{land, charge_state::NEGATIVE};

        std::size_t valid = 0;

        while (true)
        {
            const auto cd = state.snapshot();
            CHECK(state.is_physically_valid() == land.is_physically_valid(cd));
            CHECK_THAT(state.energy(), WithinAbs(land.energy(cd), 1E-12));
            if (state.is_physically_valid())
            {
                ++valid;
            }

            if (state.charge_index() == state.max_charge_index())
            {
                break;
            }
            state.increase_charge_index_by_one();
        }

        CHECK(valid == 1);
    }
    SECTION("the dependent SiDB follows the others")
    {
        detail::simulation_state state{land, charge_state::NEUTRAL};
        state.assign_dependent_cell(0);
        CHECK(state.max_charge_index() == 3);

        state.update_after_charge_change(detail::dependent_cell_mode::VARIABLE);
        // the isolated SiDB 0 is negative whatever the others do
        CHECK(state.get_charge_state_by_index(0) == charge_state::NEGATIVE);

        std::vector<charge_distribution> found{};
        uint64_t                         previous = 0;

        for (uint64_t gray = 0; gray <= state.max_charge_index(); ++gray)
        {
            const auto code = gray ^ (gray >> uint64_t{1});
            state.assign_charge_index_by_gray_code(code, previous, detail::dependent_cell_mode::VARIABLE,
                                                   detail::energy_calculation::KEEP_OLD_ENERGY_VALUE,
                                                   detail::charge_distribution_history::CONSIDER);
            previous = code;

            const auto cd = state.snapshot();
            CHECK(state.is_physically_valid() == land.is_physically_valid(cd));

            // the incremental potentials agree with a full recomputation
            const auto pot = land.local_internal_potentials(cd);
            for (std::size_t i = 0; i < 3; ++i)
            {
                CHECK_THAT(state.local_internal_potential(i), WithinAbs(pot[i], 1E-9));
            }

            if (state.is_physically_valid())
            {
                found.push_back(cd);
            }
        }

        REQUIRE(found.size() == 1);
        CHECK(found.front().get_charge_state({5, 0, 0}) == charge_state::NEUTRAL);
    }
    SECTION("negative SiDB detection and adjacent search")
    {
        detail::simulation_state state{land, charge_state::NEGATIVE,
                                       detail::simulation_state::energy_model::INTERNAL_ONLY};

        const auto negative = state.negative_sidb_detection();
        CHECK(negative == std::vector<std::size_t>{0});

        state.assign_all_charge_states(charge_state::NEUTRAL);
        state.update_after_charge_change();

        auto negatives = negative;
        state.assign_charge_state_by_index(0, charge_state::NEGATIVE);
        state.update_after_charge_change();

        state.adjacent_search(0.7, negatives);
        CHECK(negatives.size() == 2);
        CHECK(state.num_negative_sidbs() == 2);

        // the incremental energy equals the internal-only energy of the new charges
        double collect = 0.0;
        for (std::size_t i = 0; i < 3; ++i)
        {
            collect += state.local_internal_potential(i) *
                       static_cast<double>(charge_state_to_sign(state.get_charge_state_by_index(i)));
        }
        CHECK_THAT(state.energy(), WithinAbs(0.5 * collect, 1E-9));
    }
    SECTION("three-state detection")
    {
        layout close{};
        close.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        close.assign_cell_type({1, 0, 0}, sidb_technology::cell_type::NORMAL);
        close.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::NORMAL);

        const potential_landscape dense{close, simulation_parameters{2, -0.32}};
        detail::simulation_state  state{dense, charge_state::NEGATIVE};

        CHECK(state.is_three_state_simulation_required());
        CHECK(state.base() == 3);
        CHECK(state.index_base() == 2);
        CHECK(!state.three_state_sidbs().empty());
        CHECK(state.three_state_sidbs().size() + state.two_state_sidbs().size() == 3);
    }
}
