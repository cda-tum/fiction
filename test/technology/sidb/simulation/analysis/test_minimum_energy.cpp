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
 * @brief Tests for `fiction/technology/sidb/simulation/analysis/minimum_energy.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 * @author OpenAI Codex
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/charge_state.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>
#include <fiction/technology/sidb/simulation/potential_landscape.hpp>

#include <cmath>
#include <forward_list>
#include <iterator>
#include <ranges>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::analysis;

/** @brief Tests minimum-distribution search availability. @tparam Iterator Charge-distribution iterator. */
template <typename Iterator>
concept minimum_distribution_range = requires(Iterator first) { minimum_energy_distribution(first, first); };

/** @brief Tests minimum-energy search availability. @tparam Iterator Charge-distribution iterator. */
template <typename Iterator>
concept minimum_energy_range = requires(Iterator first) { minimum_energy(first, first); };

static_assert(minimum_distribution_range<std::vector<charge_distribution>::const_iterator>);
static_assert(minimum_energy_range<std::vector<charge_distribution>::const_iterator>);
static_assert(minimum_distribution_range<std::forward_list<charge_distribution>::const_iterator>);
static_assert(minimum_energy_range<std::forward_list<charge_distribution>::const_iterator>);
static_assert(!minimum_distribution_range<std::istream_iterator<charge_distribution>>);
static_assert(!minimum_energy_range<std::istream_iterator<charge_distribution>>);
static_assert(!minimum_distribution_range<const int*>);
static_assert(!minimum_energy_range<const int*>);

TEST_CASE("Test minimum energy function", "[minimum-energy]")
{
    layout lyt{};

    SECTION("layout with no SiDB placed")
    {
        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.begin(), all_cds.end())));

        all_cds.push_back(land.evaluate(charge_distribution{lyt}));

        CHECK(std::abs(minimum_energy(all_cds.begin(), all_cds.end()) - 0) < 0.00000001);
    }

    SECTION("layout with one SiDB placed")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);

        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.cbegin(), all_cds.cend())));

        all_cds.push_back(land.evaluate(charge_distribution{lyt}));

        CHECK(std::abs(minimum_energy(all_cds.cbegin(), all_cds.cend()) - 0) < 0.00000001);
    }

    SECTION("layout with three SiDBs placed")
    {
        lyt.assign_sidb({0, 0, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({10, 5, 0}, dot_tag::NORMAL);
        lyt.assign_sidb({9, 4, 1}, dot_tag::NORMAL);

        const potential_landscape        land{lyt};
        std::vector<charge_distribution> all_cds{};

        CHECK(std::isinf(minimum_energy(all_cds.cbegin(), all_cds.cend())));

        charge_distribution first{lyt};
        first.assign_charge_state({0, 0, 0}, charge_state::NEUTRAL);
        all_cds.push_back(land.evaluate(first));

        charge_distribution second{lyt};
        second.assign_charge_state({10, 5, 0}, charge_state::NEUTRAL);
        second.assign_charge_state({9, 4, 1}, charge_state::NEUTRAL);
        all_cds.push_back(land.evaluate(second));

        CHECK_THAT(minimum_energy(all_cds.cbegin(), all_cds.cend()), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(minimum_energy_distribution(all_cds.cbegin(), all_cds.cend())->same_charge_states(second));

        const auto transformed = all_cds | std::views::transform([](const auto& cd) -> const auto& { return cd; });
        CHECK_THAT(minimum_energy(transformed.begin(), transformed.end()), Catch::Matchers::WithinAbs(0.0, 0.00001));

        const std::forward_list<charge_distribution> forward_cds{all_cds.front(), all_cds.back()};
        CHECK_THAT(minimum_energy(forward_cds.cbegin(), forward_cds.cend()), Catch::Matchers::WithinAbs(0.0, 0.00001));
        CHECK(minimum_energy_distribution(forward_cds.cbegin(), forward_cds.cend())->same_charge_states(second));
    }
}
