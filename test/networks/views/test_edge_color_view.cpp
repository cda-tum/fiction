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
 * @brief Tests for `fiction/networks/views/edge_color_view.hpp`.
 * @author Marcel Walter (marcelwa)
 */

// This file is based on mockturtle/test/views/color_view.cpp

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/edge_color_view.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>

using namespace fiction;
using namespace fiction::networks;
using namespace fiction::networks::views;

TEMPLATE_TEST_CASE("Edge color views preserve network traits", "[edge-color-view]", mockturtle::aig_network,
                   mockturtle::mig_network, mockturtle::xag_network, mockturtle::xmg_network, mockturtle::klut_network,
                   technology_network)
{
    STATIC_REQUIRE(mockturtle::is_network_type_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_new_color_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_current_color_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_clear_colors_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_color_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_paint_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_eval_color_v<TestType>);
    STATIC_REQUIRE_FALSE(mockturtle::has_eval_fanins_color_v<TestType>);

    TestType ntk{};

    using color_ntk = out_of_place_edge_color_view<TestType>;
    STATIC_REQUIRE(mockturtle::is_network_type_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_new_color_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_current_color_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_clear_colors_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_color_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_paint_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_eval_color_v<color_ntk>);
    STATIC_REQUIRE(mockturtle::has_eval_fanins_color_v<color_ntk>);

    /** @brief Node and edge color view for the network. */
    const color_ntk color_view{ntk};

    using color_color_ntk = out_of_place_edge_color_view<color_ntk>;
    STATIC_REQUIRE(mockturtle::is_network_type_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_new_color_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_current_color_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_clear_colors_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_color_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_paint_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_eval_color_v<color_color_ntk>);
    STATIC_REQUIRE(mockturtle::has_eval_fanins_color_v<color_color_ntk>);

    /** @brief Nested node and edge color view. */
    [[maybe_unused]] const color_color_ntk color_color_view{color_view};
}

TEST_CASE("Out-of-place edge color view", "[edge-color-view]")
{
    mockturtle::aig_network aig_ntk{};

    const auto a = aig_ntk.create_pi();
    const auto b = aig_ntk.create_pi();
    const auto c = aig_ntk.create_pi();
    const auto d = aig_ntk.create_pi();
    const auto e = aig_ntk.create_pi();

    const auto f1 = aig_ntk.create_and(a, b);
    const auto f2 = aig_ntk.create_and(c, d);
    const auto f3 = aig_ntk.create_and(f1, f2);
    const auto f4 = aig_ntk.create_and(e, f2);
    const auto f5 = aig_ntk.create_and(f1, f3);
    const auto f6 = aig_ntk.create_and(f2, f3);
    const auto f7 = aig_ntk.create_and(f5, f6);
    const auto f8 = aig_ntk.create_and(f4, f7);
    aig_ntk.create_po(f8);

    /** @brief Node and edge color view over the AIG. */
    const out_of_place_edge_color_view aig_ecv{aig_ntk};

    auto const white  = aig_ecv.new_color();
    auto const yellow = aig_ecv.new_color();
    CHECK(yellow > white);
    auto const red = aig_ecv.new_color();
    CHECK(red > white);

    SECTION("acts as regular node color view")
    {
        /* assign some colors: f5 is white, f1 is yellow, and f3 is in the color of f1 */
        aig_ecv.paint(aig_ecv.get_node(f5), white);
        aig_ecv.paint(aig_ecv.get_node(f1), yellow);
        aig_ecv.paint(aig_ecv.get_node(f3), aig_ecv.get_node(f1));

        /* f1 and f3 have the same color */
        CHECK(
            aig_ecv.eval_color(aig_ecv.get_node(f1), aig_ecv.get_node(f3), [&](auto c0, auto c1) { return c0 == c1; }));

        /* f1 and f5 have different colors */
        CHECK(
            aig_ecv.eval_color(aig_ecv.get_node(f1), aig_ecv.get_node(f5), [&](auto c0, auto c1) { return c0 != c1; }));

        /* f5 is at least white */
        CHECK(aig_ecv.eval_color(aig_ecv.get_node(f5), [&](auto color) { return color >= white; }));

        /* f5 is not yellow */
        CHECK(aig_ecv.eval_color(aig_ecv.get_node(f5), [&](auto color) { return color != yellow; }));

        /* the fanins of f5 are at least white */
        CHECK(aig_ecv.eval_fanins_color(aig_ecv.get_node(f5), [&](auto color) { return color >= white; }));

        /* the fanins of f5 are yellow */
        CHECK(aig_ecv.eval_fanins_color(aig_ecv.get_node(f5), [&](auto color) { return color == yellow; }));

        /* at least one fanin of f5 is not red */
        CHECK(aig_ecv.eval_fanins_color(aig_ecv.get_node(f5), [&](auto color) { return color != red; }));

        /* visited flags have not been affected by assigning colors */
        CHECK(aig_ecv.visited(aig_ecv.get_node(f5)) == 0u);
        CHECK(aig_ecv.visited(aig_ecv.get_node(f1)) == 0u);
        CHECK(aig_ecv.visited(aig_ecv.get_node(f3)) == 0u);
    }
    SECTION("acts as edge color view")
    {
        /* assign some colors: (f1, f5) is white, (f1, f3) is yellow, and (f3, f6) is in the color of (f1, f3) */
        aig_ecv.paint_edge({aig_ecv.get_node(f1), aig_ecv.get_node(f5)}, white);
        aig_ecv.paint_edge({aig_ecv.get_node(f1), aig_ecv.get_node(f3)}, yellow);
        aig_ecv.paint_edge({aig_ecv.get_node(f3), aig_ecv.get_node(f6)}, {aig_ecv.get_node(f1), aig_ecv.get_node(f3)});

        /* (f1, f3) and (f3, f6) have the same color */
        CHECK(aig_ecv.eval_edge_color({aig_ecv.get_node(f1), aig_ecv.get_node(f3)},
                                      {aig_ecv.get_node(f3), aig_ecv.get_node(f6)},
                                      [](const auto c0, const auto c1) { return c0 == c1; }));

        /* (f1, f5) and (f1, f3) have different colors */
        CHECK(aig_ecv.eval_edge_color({aig_ecv.get_node(f1), aig_ecv.get_node(f5)},
                                      {aig_ecv.get_node(f1), aig_ecv.get_node(f3)},
                                      [](const auto c0, const auto c1) { return c0 != c1; }));

        /* (f1, f5) is at least white */
        CHECK(aig_ecv.eval_edge_color({aig_ecv.get_node(f1), aig_ecv.get_node(f5)},
                                      [white](const auto color) { return color >= white; }));

        /* (f1, f5) is not yellow */
        CHECK(aig_ecv.eval_edge_color({aig_ecv.get_node(f1), aig_ecv.get_node(f5)},
                                      [yellow](const auto color) { return color != yellow; }));
    }
    SECTION("clears current colors while preserving explicit node-color calls")
    {
        /** Const access to the mutable color storage. */
        const auto& color_view = aig_ecv;
        color_view.paint(color_view.get_node(f1), red);
        color_view.paint_edge({color_view.get_node(f1), color_view.get_node(f3)}, red);
        color_view.clear_colors();
        CHECK(color_view.color(color_view.get_node(f1)) == 0u);
        CHECK(color_view.edge_color({color_view.get_node(f1), color_view.get_node(f3)}) == 0u);

        color_view.paint(color_view.get_node(f1), red);
        color_view.paint_edge({color_view.get_node(f1), color_view.get_node(f3)}, red);
        color_view.clear_colors(yellow);
        CHECK(color_view.color(color_view.get_node(f1)) == yellow);
        CHECK(color_view.edge_color({color_view.get_node(f1), color_view.get_node(f3)}) == red);

        color_view.clear_colors(0u);
        CHECK(color_view.color(color_view.get_node(f1)) == 0u);
        CHECK(color_view.edge_color({color_view.get_node(f1), color_view.get_node(f3)}) == red);
        CHECK(color_view.current_color() == red);
    }
}
