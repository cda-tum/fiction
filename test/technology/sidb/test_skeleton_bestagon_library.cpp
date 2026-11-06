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

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/skeleton_bestagon_library.hpp>

#include <algorithm>
#include <array>
#include <type_traits>
#include <utility>

using namespace fiction;
using namespace fiction::sidb;

TEST_CASE("Bestagon traits", "[sidb-dynamic-gate-library]")
{
    STATIC_REQUIRE(std::is_same_v<skeleton_bestagon_library::layout, sidb::layout>);
}

TEST_CASE("Bestagon supports non-strict comparison functions", "[sidb-dynamic-gate-library]")
{
    const auto implementations = skeleton_bestagon_library::get_functional_implementations();
    const auto ports           = skeleton_bestagon_library::get_gate_ports();
    const auto le              = fiction::synthesis::create_le_tt();
    const auto ge              = fiction::synthesis::create_ge_tt();
    CHECK(*le.cbegin() == 0xbu);
    CHECK(*ge.cbegin() == 0xdu);
    CHECK(le == ~fiction::synthesis::create_gt_tt());
    CHECK(ge == ~fiction::synthesis::create_lt_tt());
    CHECK(implementations.contains(~fiction::synthesis::create_gt_tt()));
    CHECK(implementations.contains(~fiction::synthesis::create_lt_tt()));

    fiction::networks::technology_network network{};
    const auto                            a = network.create_pi();
    const auto                            b = network.create_pi();
    CHECK(network.node_function(network.get_node(network.create_le(a, b))) == fiction::synthesis::create_le_tt());
    CHECK(network.node_function(network.get_node(network.create_ge(a, b))) == fiction::synthesis::create_ge_tt());
    const std::array supported{std::pair{"ID", fiction::synthesis::create_id_tt()},
                               std::pair{"NOT", fiction::synthesis::create_not_tt()},
                               std::pair{"AND", fiction::synthesis::create_and_tt()},
                               std::pair{"OR", fiction::synthesis::create_or_tt()},
                               std::pair{"NAND", fiction::synthesis::create_nand_tt()},
                               std::pair{"NOR", fiction::synthesis::create_nor_tt()},
                               std::pair{"XOR", fiction::synthesis::create_xor_tt()},
                               std::pair{"XNOR", fiction::synthesis::create_xnor_tt()},
                               std::pair{"LE", fiction::synthesis::create_le_tt()},
                               std::pair{"GE", fiction::synthesis::create_ge_tt()},
                               std::pair{"LT", fiction::synthesis::create_lt_tt()},
                               std::pair{"GT", fiction::synthesis::create_gt_tt()}};
    for (const auto& [name, function] : supported)
    {
        CAPTURE(name);
        const auto implementation = implementations.find(function);
        REQUIRE(implementation != implementations.end());
        CHECK_FALSE(implementation->second.empty());
        for (const auto& gate : implementation->second)
        {
            const auto gate_ports = ports.find(gate);
            REQUIRE(gate_ports != ports.end());
            CHECK_FALSE(gate_ports->second.empty());
            if (function.num_vars() == 2)
            {
                CHECK(std::ranges::any_of(gate_ports->second,
                                          [](const auto& p) { return p.inp.size() == 2 && !p.out.empty(); }));
            }
        }
    }
}
