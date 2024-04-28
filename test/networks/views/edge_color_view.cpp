//
// Created by marcel on 18.01.22.
//

// This file is based on mockturtle/test/views/color_view.cpp

#include "fiction/networks/views/edge_color_view.hpp"

#include <catch2/catch_test_macros.hpp>

#include "fiction/networks/technology_network.hpp"

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/networks/xag.hpp>
#include <mockturtle/networks/xmg.hpp>
#include <mockturtle/traits.hpp>

#include <vector>

using namespace fiction;

template <typename Ntk>
void test_color_view()
{
    CHECK(mockturtle::is_network_type_v<Ntk>);
    CHECK(!mockturtle::has_new_color_v<Ntk>);
    CHECK(!mockturtle::has_current_color_v<Ntk>);
    CHECK(!mockturtle::has_clear_colors_v<Ntk>);
    CHECK(!mockturtle::has_color_v<Ntk>);
    CHECK(!mockturtle::has_paint_v<Ntk>);
    CHECK(!mockturtle::has_eval_color_v<Ntk>);
    CHECK(!mockturtle::has_eval_fanins_color_v<Ntk>);

    Ntk ntk{};

    using color_ntk = out_of_place_edge_color_view<Ntk>;
    CHECK(mockturtle::is_network_type_v<color_ntk>);
    CHECK(mockturtle::has_new_color_v<color_ntk>);
    CHECK(mockturtle::has_current_color_v<color_ntk>);
    CHECK(mockturtle::has_clear_colors_v<color_ntk>);
    CHECK(mockturtle::has_color_v<color_ntk>);
    CHECK(mockturtle::has_paint_v<color_ntk>);
    CHECK(mockturtle::has_eval_color_v<color_ntk>);
    CHECK(mockturtle::has_eval_fanins_color_v<color_ntk>);

    color_ntk color_view{ntk};

    using color_color_ntk = out_of_place_edge_color_view<color_ntk>;
    CHECK(mockturtle::is_network_type_v<color_color_ntk>);
    CHECK(mockturtle::has_new_color_v<color_color_ntk>);
    CHECK(mockturtle::has_current_color_v<color_color_ntk>);
    CHECK(mockturtle::has_clear_colors_v<color_color_ntk>);
    CHECK(mockturtle::has_color_v<color_color_ntk>);
    CHECK(mockturtle::has_paint_v<color_color_ntk>);
    CHECK(mockturtle::has_eval_color_v<color_color_ntk>);
    CHECK(mockturtle::has_eval_fanins_color_v<color_color_ntk>);

    [[maybe_unused]] color_color_ntk color_color_view{color_view};
}

TEST_CASE("Create edge color views", "[edge-color-view]")
{
    test_color_view<mockturtle::aig_network>();
    test_color_view<mockturtle::mig_network>();
    test_color_view<mockturtle::xag_network>();
    test_color_view<mockturtle::xmg_network>();
    test_color_view<mockturtle::klut_network>();
    test_color_view<fiction::technology_network>();
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

    out_of_place_edge_color_view aig_ecv{aig_ntk};

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
}
