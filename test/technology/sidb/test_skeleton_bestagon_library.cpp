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
 * @brief Tests for `fiction/technology/sidb/skeleton_bestagon_library.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/skeleton_bestagon_library.hpp>

#include <type_traits>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("Bestagon traits", "[sidb-dynamic-gate-library]")
{
    STATIC_REQUIRE(std::is_same_v<skeleton_bestagon_library::layout, sidb::layout>);
}

TEST_CASE("Bestagon supports non-strict comparison functions", "[sidb-dynamic-gate-library]")
{
    const auto implementations = skeleton_bestagon_library::get_functional_implementations();
    CHECK(fiction::synthesis::create_le_tt() == ~fiction::synthesis::create_gt_tt());
    CHECK(fiction::synthesis::create_ge_tt() == ~fiction::synthesis::create_lt_tt());
    CHECK(implementations.contains(~fiction::synthesis::create_gt_tt()));
    CHECK(implementations.contains(~fiction::synthesis::create_lt_tt()));

    fiction::networks::technology_network network{};
    const auto                            a = network.create_pi();
    const auto                            b = network.create_pi();
    CHECK(network.node_function(network.get_node(network.create_le(a, b))) == fiction::synthesis::create_le_tt());
    CHECK(network.node_function(network.get_node(network.create_ge(a, b))) == fiction::synthesis::create_ge_tt());
    for (const auto& entry : implementations)
    {
        CHECK(entry.first == ~(~entry.first));
    }
}
