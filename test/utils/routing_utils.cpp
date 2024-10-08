//
// Created by marcel on 08.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/types.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <algorithm>
#include <vector>

using namespace fiction;

template <typename Lyt>
void check_containing_objectives(const std::vector<routing_objective<Lyt>>& objectives,
                                 const std::vector<routing_objective<Lyt>>& expected_objectives)
{
    CHECK(objectives.size() == expected_objectives.size());

    std::for_each(objectives.cbegin(), objectives.cend(),
                  [&expected_objectives](const auto& obj)
                  {
                      CHECK(std::find(expected_objectives.cbegin(), expected_objectives.cend(), obj) !=
                            expected_objectives.cend());
                  });
}

TEST_CASE("Extract routing objectives", "[routing-utils]")
{
    SECTION("Simple wire connection")
    {
        const auto layout     = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{{0, 1}, {2, 1}}});
    }
    SECTION("Two paths wire connections")
    {
        const auto layout     = blueprints::unbalanced_and_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{{0, 2}, {2, 0}}, {{1, 0}, {2, 0}}, {{2, 0}, {3, 0}}});
    }
    SECTION("Three paths wire connections")
    {
        const auto layout     = blueprints::three_wire_paths_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{{0, 0}, {4, 0}}, {{0, 2}, {4, 2}}, {{0, 4}, {4, 4}}});
    }
    SECTION("Direct gate connections")
    {
        const auto layout     = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{{1, 1}, {2, 1}},
                                                 {{2, 0}, {2, 1}},
                                                 {{3, 1}, {2, 1}},
                                                 {{1, 1}, {1, 0}},
                                                 {{2, 0}, {1, 0}},
                                                 {{2, 1}, {2, 2}},
                                                 {{1, 0}, {0, 0}}});
    }
    SECTION("Two incoming gate wires")
    {
        const auto layout     = blueprints::use_and_gate_layout<cart_gate_clk_lyt>();
        const auto objectives = extract_routing_objectives(layout);

        check_containing_objectives(objectives, {{{0, 1}, {1, 2}}, {{3, 3}, {1, 2}}, {{1, 2}, {3, 2}}});
    }
}

template <typename Lyt>
void check_non_empty_tile(const Lyt& lyt, const tile<Lyt>& t) noexcept
{
    CHECK(!lyt.is_empty_tile(t));
    CHECK(lyt.has_no_incoming_signal(t));
    CHECK(lyt.has_no_outgoing_signal(t));
}

TEST_CASE("Clear routing", "[routing-utils]")
{
    SECTION("Simple wire connection")
    {
        auto layout = blueprints::straight_wire_gate_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        CHECK(layout.is_empty_tile({1, 1}));
        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {2, 1});
    }
    SECTION("Direct gate connections")
    {
        auto layout = blueprints::xor_maj_gate_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {3, 1});
        check_non_empty_tile(layout, {2, 1});
        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {2, 2});
        check_non_empty_tile(layout, {0, 0});
    }
    SECTION("Crossings")
    {
        auto layout = blueprints::crossing_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {0, 2});
        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 2});
        check_non_empty_tile(layout, {3, 1});
        check_non_empty_tile(layout, {3, 2});

        CHECK(layout.is_empty_tile({2, 1}));
        CHECK(layout.is_empty_tile({1, 2}));
        CHECK(layout.is_empty_tile({2, 1, 1}));
    }
    SECTION("Fan-outs")
    {
        auto layout = blueprints::fanout_layout<cart_gate_clk_lyt>();

        clear_routing(layout);

        check_non_empty_tile(layout, {0, 1});
        check_non_empty_tile(layout, {1, 1});
        check_non_empty_tile(layout, {2, 1});
        check_non_empty_tile(layout, {1, 0});
        check_non_empty_tile(layout, {2, 0});
        check_non_empty_tile(layout, {1, 2});

        CHECK(layout.is_empty_tile({2, 2}));
    }
}
