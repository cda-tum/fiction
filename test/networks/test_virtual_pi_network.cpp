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
 * @brief Tests for `fiction/networks/virtual_pi_network.hpp`.
 * @author Benjamin Hien (hibenj)
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>

#include <type_traits>
#include <vector>

using namespace fiction;
using namespace fiction::networks;

TEST_CASE("Real and virtual input visitors report complete collections", "[virtual-pi-network]")
{
    virtual_pi_network<technology_network> vpi{};
    STATIC_REQUIRE(is_virtual_network_type_v<decltype(vpi)>);

    const auto a    = vpi.create_pi();
    const auto b    = vpi.create_pi();
    const auto c    = vpi.create_virtual_pi(a);
    const auto d    = vpi.create_virtual_pi(a);
    const auto e    = vpi.create_virtual_pi(b);
    const auto gate = vpi.create_and(b, c);
    vpi.create_po(gate);

    CHECK(vpi.get_real_pi(c) == a);
    CHECK(vpi.get_real_pi(d) == a);
    CHECK(vpi.get_real_pi(e) == b);
    CHECK(vpi.num_real_pis() == 2);
    CHECK(vpi.num_real_cis() == 2);
    CHECK(vpi.num_virtual_pis() == 3);
    CHECK(vpi.num_virtual_cis() == 3);
    for (const auto node : {a, b})
    {
        CAPTURE(node);
        CHECK(vpi.is_real_pi(node));
        CHECK(vpi.is_real_ci(node));
        CHECK_FALSE(vpi.is_virtual_pi(node));
        CHECK_FALSE(vpi.is_virtual_ci(node));
    }
    for (const auto node : {c, d, e})
    {
        CAPTURE(node);
        CHECK(vpi.is_virtual_pi(node));
        CHECK(vpi.is_virtual_ci(node));
        CHECK_FALSE(vpi.is_real_pi(node));
        CHECK_FALSE(vpi.is_real_ci(node));
    }
    CHECK_FALSE(vpi.is_real_pi(gate));
    CHECK_FALSE(vpi.is_virtual_pi(gate));

    /** @brief Complete traversal observations, including duplicate visits. */
    std::vector<technology_network::node> visited{};
    vpi.foreach_real_pi([&](const auto node) { visited.push_back(node); });
    CHECK_THAT(visited, Catch::Matchers::UnorderedRangeEquals(std::vector{a, b}));
    visited.clear();
    vpi.foreach_real_ci([&](const auto node) { visited.push_back(node); });
    CHECK_THAT(visited, Catch::Matchers::UnorderedRangeEquals(std::vector{a, b}));
    visited.clear();
    vpi.foreach_virtual_pi([&](const auto node) { visited.push_back(node); });
    CHECK_THAT(visited, Catch::Matchers::UnorderedRangeEquals(std::vector{c, d, e}));
    visited.clear();
    vpi.foreach_virtual_ci([&](const auto node) { visited.push_back(node); });
    CHECK_THAT(visited, Catch::Matchers::UnorderedRangeEquals(std::vector{c, d, e}));
}

TEMPLATE_TEST_CASE("Clones preserve virtual input mappings and own independent storage", "[virtual-pi-network]",
                   technology_network, mockturtle::aig_network, mockturtle::xag_network, mockturtle::mig_network,
                   mockturtle::xmg_network)
{
    virtual_pi_network<TestType> vpi{};
    const auto                   a  = vpi.create_pi();
    const auto                   b  = vpi.create_pi();
    const auto                   f1 = vpi.create_and(a, b);
    const auto                   c  = vpi.create_virtual_pi(a);
    const auto                   d  = vpi.create_virtual_pi(b);
    const auto                   f2 = vpi.create_and(b, c);
    const auto                   f3 = vpi.create_or(a, d);
    vpi.create_po(f1);
    vpi.create_po(f2);
    vpi.create_po(f3);

    if constexpr (std::is_same_v<TestType, technology_network>)
    {
        CHECK(vpi.size() == 9);
        CHECK(vpi.real_size() == 7);
    }
    else
    {
        CHECK(vpi.size() == 8);
        CHECK(vpi.real_size() == 6);
    }
    CHECK(vpi.num_real_pis() == 2);
    CHECK(vpi.num_real_cis() == 2);
    CHECK(vpi.num_virtual_pis() == 2);
    CHECK(vpi.num_virtual_cis() == 2);

    auto clone = vpi.clone();
    CHECK(clone.size() == vpi.size());
    CHECK(clone.real_size() == vpi.real_size());
    CHECK(clone.get_real_pi(clone.get_node(c)) == clone.get_node(a));
    CHECK(clone.get_real_pi(clone.get_node(d)) == clone.get_node(b));
    CHECK(clone.num_real_pis() == 2);
    CHECK(clone.num_real_cis() == 2);
    CHECK(clone.num_virtual_pis() == 2);
    CHECK(clone.num_virtual_cis() == 2);
    CHECK(clone.num_pos() == 3);

    SECTION("Changes to the clone leave the source unchanged")
    {
        const auto extra         = clone.create_pi();
        const auto virtual_extra = clone.create_virtual_pi(extra);
        CHECK(clone.get_real_pi(clone.get_node(virtual_extra)) == clone.get_node(extra));
        CHECK(clone.num_real_pis() == 3);
        CHECK(clone.num_virtual_pis() == 3);
        CHECK(clone.size() == vpi.size() + 2);
        CHECK(vpi.num_real_pis() == 2);
        CHECK(vpi.num_virtual_pis() == 2);
        CHECK_FALSE(vpi.is_virtual_pi(clone.get_node(virtual_extra)));
    }
    SECTION("Changes to the source leave the clone unchanged")
    {
        const auto extra = vpi.create_virtual_pi(a);
        CHECK(vpi.num_virtual_pis() == 3);
        CHECK(vpi.size() == clone.size() + 1);
        CHECK(clone.num_real_pis() == 2);
        CHECK(clone.num_virtual_pis() == 2);
        CHECK_FALSE(clone.is_virtual_pi(vpi.get_node(extra)));
    }
}
