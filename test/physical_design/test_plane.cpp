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
 * @brief Tests for `fiction/physical_design/plane.hpp`.
 * @author Benjamin Hien (hibenj)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/network_ranking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/networks/extract_layout_network.hpp>
#include <fiction/networks/network_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/plane.hpp>
#include <fiction/synthesis/planarization.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>
#include <fiction/types.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <concepts>
#include <cstdint>
#include <stdexcept>
#include <string>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::physical_design;
using namespace fiction::synthesis;

static_assert(std::same_as<decltype(&plane), cartesian_gate_layout (*)(const named_technology_network&,
                                                                       const plane_params&, plane_stats*)>);

namespace
{

/**
 * @brief Balanced, ranked technology network, the input of `planarization`.
 */
using ranked_network = mutable_rank_view<technology_network>;

/**
 * @brief Checks the reported statistics against the returned layout.
 * @param st Reported statistics.
 * @param layout Returned layout.
 */
void check_stats(const plane_stats& st, const cartesian_gate_layout& layout)
{
    CHECK(st.time_total >= decltype(st.time_total)::zero());
    CHECK(st.x_size == layout.width());
    CHECK(st.y_size == layout.height());
    CHECK(st.num_gates == layout.num_gates());
    CHECK(st.num_wires == layout.num_wires());
    CHECK(st.num_crossings == 0);
}

/**
 * @brief Planarizes a ranked network and flattens the result into the input form of `plane`.
 * @param ranked Ranked network.
 * @param xor_gates Whether crossing gates consist of XOR gates.
 * @return Named technology network whose node order is its planar rank order.
 */
named_technology_network planar_network(const ranked_network& ranked, const bool xor_gates = false)
{
    planarization_params ps{};
    ps.duplication.crossing_gates.xor_gates = xor_gates;

    return flatten_planarization(planarization(ranked, ps)).first;
}

/**
 * @brief Places and routes a planarized network and checks crossings, interface, statistics, and equivalence.
 * @param ranked Ranked network.
 * @param xor_gates Whether crossing gates consist of XOR gates.
 * @param phases Number of clock phases.
 * @return The layout.
 */
cartesian_gate_layout check_plane(const ranked_network& ranked, const bool xor_gates = false,
                                  const clocking::num_clks phases = clocking::num_clks::FOUR)
{
    const auto planar = planar_network(ranked, xor_gates);

    plane_stats st{};

    auto layout = plane(planar, {.number_of_clock_phases = phases}, &st);

    check_stats(st, layout);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == planar.num_pis());
    CHECK(layout.num_pos() == planar.num_pos());
    check_eq(planar, extract_layout_network(layout));

    return layout;
}

}  // namespace

TEST_CASE("Layouts are crossing-free and equivalent to the planar network", "[plane]")
{
    const auto [name, ranked] = GENERATE(table<std::string, ranked_network>({
        {"unbalanced_and_inv", rank(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>())},
        {"maj1", rank(blueprints::maj1_network<mockturtle::aig_network>())},
        {"maj4", rank(blueprints::maj4_network<mockturtle::aig_network>())},
        {"half_adder", rank(blueprints::half_adder_network<mockturtle::mig_network>())},
        {"full_adder", rank(blueprints::full_adder_network<mockturtle::mig_network>())},
        {"mux21", rank(blueprints::mux21_network<technology_network>())},
        {"parity", rank(blueprints::parity_network<technology_network>())},
        {"clpl", rank(blueprints::clpl<technology_network>())},
        {"multi_output", rank(blueprints::multi_output_network<technology_network>())},
        {"fanout_corner_case", rank(blueprints::fanout_substitution_corner_case_network<technology_network>())},
        {"nary_operation", rank(blueprints::nary_operation_network<technology_network>())},
    }));
    const auto xor_gates      = GENERATE(false, true);
    const auto phases         = GENERATE(clocking::num_clks::FOUR, clocking::num_clks::THREE);

    CAPTURE(name, xor_gates, static_cast<int>(phases));

    static_cast<void>(check_plane(ranked, xor_gates, phases));
}

TEST_CASE("Gate library applies to the layout", "[plane]")
{
    const auto layout = plane(planar_network(rank(blueprints::full_adder_network<mockturtle::mig_network>())));

    CHECK_NOTHROW(apply_gate_library<qca::qca_one_library>(layout));
}

TEST_CASE("Constant fanins", "[plane]")
{
    SECTION("a gate with two inputs and a constant third one is placed")
    {
        // the MIG adders map to majority gates with a constant fanin
        const auto xor_gates = GENERATE(false, true);

        CAPTURE(xor_gates);

        static_cast<void>(check_plane(rank(blueprints::half_adder_network<mockturtle::mig_network>()), xor_gates));
    }
    SECTION("a gate with a single input next to a constant is rejected")
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto a1 = tec.create_and(x1, tec.get_constant(true));
        tec.create_po(tec.create_or(a1, x2));
        tec.create_po(tec.create_and(x1, x2));

        const auto planar = planar_network(rank(tec));

        CHECK_THROWS_AS(plane(planar), std::invalid_argument);
    }
}

TEST_CASE("Inputs that drive outputs directly", "[plane]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    tec.create_po(x1);
    tec.create_po(tec.create_and(x2, x3));
    tec.create_po(x3);

    const auto xor_gates = GENERATE(false, true);

    CAPTURE(xor_gates);

    static_cast<void>(check_plane(rank(tec), xor_gates));
}

TEST_CASE("Nodes with two outputs push the following output rows down", "[plane]")
{
    // the first node of the output level drives two outputs; the next node sits right below it on the diagonal, so
    // its output row has to move down, and so has the row of the node after that
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    const auto x3 = tec.create_pi();
    const auto x4 = tec.create_pi();
    const auto a1 = tec.create_and(x1, x2);
    const auto o1 = tec.create_or(x2, x3);
    const auto a2 = tec.create_and(x3, x4);
    tec.create_po(a1);
    tec.create_po(a1);
    tec.create_po(o1);
    tec.create_po(a2);
    tec.create_po(a2);

    const auto xor_gates = GENERATE(false, true);

    CAPTURE(xor_gates);

    CHECK(check_plane(rank(tec), xor_gates).num_pos() == 5);
}

TEST_CASE("Constant outputs keep their position", "[plane]")
{
    technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(tec.create_and(x1, x2));
    tec.create_po(tec.get_constant(true));
    tec.create_po(tec.create_or(x1, x2));
    tec.create_po(tec.get_constant(false));

    const auto xor_gates = GENERATE(false, true);

    CAPTURE(xor_gates);

    const auto layout = check_plane(rank(tec), xor_gates);

    CHECK(layout.num_pos() == 4);
    CHECK(layout.is_po_tile(layout.get_tile(layout.po_at(1))));
}

TEST_CASE("Unused inputs stay at the border", "[plane]")
{
    technology_network tec{};

    const auto                  x1 = tec.create_pi();
    [[maybe_unused]] const auto x2 = tec.create_pi();
    const auto                  x3 = tec.create_pi();
    [[maybe_unused]] const auto x4 = tec.create_pi();
    tec.create_po(tec.create_and(x1, x3));

    const auto xor_gates = GENERATE(false, true);

    CAPTURE(xor_gates);

    const auto layout = check_plane(rank(tec), xor_gates);

    CHECK(layout.num_pis() == 4);

    // the two unused inputs sit at the border and drive nothing, so they have no wire
    uint32_t unused = 0;
    layout.foreach_pi([&layout, &unused](const auto& pi) { unused += layout.fanout_size(pi) == 0; });
    CHECK(unused == 2);
}

TEST_CASE("Progress is reported", "[plane]")
{
    const auto planar = planar_network(rank(blueprints::full_adder_network<mockturtle::mig_network>()));

    progress_recorder rec{};

    const auto layout = plane(planar, {.on_progress = rec.callback()});

    check_eq(planar, extract_layout_network(layout));

    CHECK(rec.is_consistent("placing levels"));
    CHECK(rec.final_count("placing levels") == mutable_rank_view{planar}.depth() + 2);
}

TEST_CASE("Asymmetric gates keep their fanin order", "[plane]")
{
    // a less-than gate in both fanin orders: the second fanin ends up north of the first in one of them
    const auto swap_fanins = GENERATE(false, true);

    CAPTURE(swap_fanins);

    named_technology_network tec{};

    const auto x1 = tec.create_pi();
    const auto x2 = tec.create_pi();
    tec.create_po(swap_fanins ? tec.create_lt(x2, x1) : tec.create_lt(x1, x2));

    const auto layout = plane(tec);

    CHECK(layout.num_crossings() == 0);
    check_eq(tec, extract_layout_network(layout));
}

TEST_CASE("Invalid inputs are rejected", "[plane]")
{
    named_technology_network tec{};

    SECTION("a rank order with crossings")
    {
        // two gates whose fanins cross in the input order
        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        const auto x4 = tec.create_pi();
        tec.create_po(tec.create_and(x1, x3));
        tec.create_po(tec.create_and(x2, x4));

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("a gate with three fanins")
    {
        const named_technology_network maj{blueprints::maj1_network<technology_network>()};

        CHECK_THROWS_AS(plane(maj), high_degree_fanin_exception);
    }
    SECTION("a node with three fanouts")
    {
        // not fanout-substituted: the gate drives three outputs
        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto a1 = tec.create_and(x1, x2);
        tec.create_po(a1);
        tec.create_po(a1);
        tec.create_po(a1);

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("a network without primary inputs")
    {
        tec.create_po(tec.get_constant(true));

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("an unbalanced network")
    {
        // the third input skips a level
        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        tec.create_po(tec.create_and(tec.create_and(x1, x2), x3));

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("a gate with only constant fanins")
    {
        const auto x1 = tec.create_pi();
        tec.create_po(x1);
        tec.create_po(tec.create_buf(tec.get_constant(true)));

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("a gate that no output reaches")
    {
        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        tec.create_po(tec.create_and(x1, x2));
        [[maybe_unused]] const auto dangling = tec.create_or(x1, x2);

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
    SECTION("an output driven above the last level")
    {
        // balanced gates, but the first output is driven by an input while the second one sits on level 1
        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        tec.create_po(x1);
        tec.create_po(tec.create_and(x2, x3));

        CHECK_THROWS_AS(plane(tec), std::invalid_argument);
    }
}

TEST_CASE("Shared wires and routing conflicts", "[plane]")
{
    cartesian_gate_layout lyt{{5, 5}, clocking::twoddwave(clocking::num_clks::FOUR)};

    const auto a = lyt.create_pi("a", {0, 0});

    // a wire east of the input, then a second consumer that reuses it
    const auto first = physical_design::detail::wire_straight(lyt, {0, 0}, {3, 0});
    CHECK(lyt.get_tile(first) == cartesian_gate_layout::tile{2, 0});
    CHECK(lyt.fanout_size(a) == 1);

    const auto wires_after_first = lyt.num_wires();

    const auto second = physical_design::detail::wire_straight(lyt, {0, 0}, {2, 0});
    CHECK(lyt.find_object(cartesian_gate_layout::tile{1, 0}) == second);
    CHECK(lyt.num_wires() == wires_after_first);

    // a foreign signal on the way is a conflict
    const auto b = lyt.create_pi("b", {1, 2});
    lyt.create_po(b, "o", {1, 3});

    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {1, 0}, {1, 4}), std::logic_error);

    // neither east nor south
    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {2, 0}, {1, 1}), std::logic_error);
    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {3, 3}, {3, 4}), std::logic_error);  // empty source

    CHECK(physical_design::detail::rows_between({0, 0}, {0, 3}) == 2);
    CHECK_THROWS_AS(physical_design::detail::rows_between({0, 3}, {0, 0}), std::logic_error);
}
