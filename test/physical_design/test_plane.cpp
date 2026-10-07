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

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"
#include "utils/progress_recorder.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/networks/network_utils.hpp>
#include <fiction/networks/technology_network.hpp>
#include <fiction/networks/views/mutable_rank_view.hpp>
#include <fiction/networks/virtual_pi_network.hpp>
#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/physical_design/plane.hpp>
#include <fiction/synthesis/network_balancing.hpp>
#include <fiction/synthesis/node_duplication_planarization.hpp>
#include <fiction/synthesis/planarization.hpp>
#include <fiction/technology/qca/qca_one_library.hpp>

#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/mig.hpp>

#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::networks;
using namespace fiction::networks::views;
using namespace fiction::physical_design;
using namespace fiction::synthesis;

namespace
{

using gate_layout = gate_level_layout<cartesian_layout<coords::offset>>;

/**
 * Balances the network with unified outputs and ranks it.
 */
template <typename Ntk>
mutable_rank_view<technology_network> rank(const Ntk& ntk)
{
    network_balancing_params ps{};
    ps.unify_outputs = true;

    return mutable_rank_view{network_balancing<technology_network>(ntk, ps)};
}

void check_stats(const plane_stats& st)
{
    CHECK(st.x_size > 0);
    CHECK(st.y_size > 0);
    CHECK(st.num_gates > 0);
    CHECK(st.num_wires > 0);
    CHECK(st.num_crossings == 0);
}

/**
 * Planarizes the network, places and routes it, and checks that the layout is crossing-free and computes the planar
 * network's functions.
 */
template <typename Ntk>
void check_plane(const Ntk& ntk, const bool xor_gates, const clocking::num_clks phases = clocking::num_clks::FOUR)
{
    planarization_params p_ps{};
    p_ps.duplication.xor_gates = xor_gates;

    const auto planar = planarization(rank(ntk), p_ps);

    plane_params ps{};
    ps.number_of_clock_phases = phases;
    plane_stats st{};

    const auto layout = plane<gate_layout>(planar, ps, &st);

    check_stats(st);
    CHECK(layout.num_crossings() == 0);
    CHECK(layout.num_pis() == planar.num_pis());
    CHECK(layout.num_pos() == planar.num_pos());
    check_eq(planar, layout);
}

template <typename Ntk>
void check_plane_both_gadgets(const Ntk& ntk)
{
    check_plane(ntk, false);
    check_plane(ntk, true);
}

}  // namespace

TEST_CASE("Layout equivalence", "[plane]")
{
    check_plane_both_gadgets(blueprints::unbalanced_and_inv_network<mockturtle::aig_network>());
    check_plane_both_gadgets(blueprints::maj1_network<mockturtle::aig_network>());
    check_plane_both_gadgets(blueprints::maj4_network<mockturtle::aig_network>());
    check_plane_both_gadgets(blueprints::half_adder_network<mockturtle::mig_network>());
    check_plane_both_gadgets(blueprints::full_adder_network<mockturtle::mig_network>());
    check_plane_both_gadgets(blueprints::mux21_network<technology_network>());
    check_plane_both_gadgets(blueprints::parity_network<technology_network>());
    check_plane_both_gadgets(blueprints::clpl<technology_network>());
    check_plane_both_gadgets(blueprints::multi_output_network<technology_network>());
    check_plane_both_gadgets(blueprints::fanout_substitution_corner_case_network<technology_network>());
    check_plane_both_gadgets(blueprints::nary_operation_network<technology_network>());
}

TEST_CASE("Three clock phases", "[plane]")
{
    check_plane(blueprints::full_adder_network<mockturtle::mig_network>(), true, clocking::num_clks::THREE);
    check_plane(blueprints::parity_network<technology_network>(), false, clocking::num_clks::THREE);
}

TEST_CASE("Gate library applies to the layout", "[plane]")
{
    const auto planar = planarization(rank(blueprints::full_adder_network<mockturtle::mig_network>()));
    const auto layout = plane<gate_layout>(planar);

    CHECK_NOTHROW(apply_gate_library<qca::qca_one_library>(layout));
}

TEST_CASE("Constant fanins", "[plane]")
{
    SECTION("a gate with two inputs and a constant third one is placed")
    {
        // the MIG adders map to majority gates with a constant fanin
        check_plane_both_gadgets(blueprints::half_adder_network<mockturtle::mig_network>());
    }
    SECTION("a gate with a single input next to a constant is rejected")
    {
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto a1 = tec.create_and(x1, tec.get_constant(true));
        tec.create_po(tec.create_or(a1, x2));
        tec.create_po(tec.create_and(x1, x2));

        const auto planar = planarization(rank(tec));

        CHECK_THROWS_AS(plane<gate_layout>(planar), std::invalid_argument);
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

    check_plane_both_gadgets(tec);
}

TEST_CASE("Progress is reported", "[plane]")
{
    const auto planar = planarization(rank(blueprints::full_adder_network<mockturtle::mig_network>()));

    progress_recorder rec{};
    plane_params      ps{};
    ps.on_progress = rec.callback();

    const auto layout = plane<gate_layout>(planar, ps);

    check_eq(planar, layout);

    CHECK(rec.is_consistent("placing levels"));
    CHECK(rec.final_count("placing levels") == planar.depth() + 2);
}

TEST_CASE("Invalid inputs are rejected", "[plane]")
{
    SECTION("a rank order with crossings")
    {
        // two gates whose fanins cross in the input order
        technology_network tec{};

        const auto x1 = tec.create_pi();
        const auto x2 = tec.create_pi();
        const auto x3 = tec.create_pi();
        const auto x4 = tec.create_pi();
        tec.create_po(tec.create_and(x1, x3));
        tec.create_po(tec.create_and(x2, x4));

        const auto ranked = rank(tec);

        CHECK_THROWS_AS(plane<gate_layout>(ranked), std::invalid_argument);
    }
    SECTION("a gate with three fanins")
    {
        const auto ranked = rank(blueprints::maj1_network<technology_network>());

        CHECK_THROWS_AS(plane<gate_layout>(ranked), high_degree_fanin_exception);
    }
}

TEST_CASE("Shared wires and routing conflicts", "[plane]")
{
    gate_layout lyt{{4, 4}, clocking::twoddwave(clocking::num_clks::FOUR)};

    const auto a = lyt.create_pi("a", {0, 0});

    // a wire east of the input, then a second consumer that reuses it
    const auto first = physical_design::detail::wire_straight(lyt, {0, 0}, {3, 0});
    CHECK(lyt.get_tile(lyt.get_node(first)) == tile<gate_layout>{2, 0});

    const auto wires_after_first = lyt.num_wires();

    const auto second = physical_design::detail::wire_straight(lyt, {0, 0}, {2, 0});
    CHECK(lyt.get_node(second) == lyt.get_node(tile<gate_layout>{1, 0}));
    CHECK(lyt.num_wires() == wires_after_first);

    // a foreign signal on the way is a conflict
    const auto b = lyt.create_pi("b", {1, 2});
    lyt.create_po(b, "o", {1, 3});

    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {1, 0}, {1, 4}), std::logic_error);

    // neither east nor south
    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {2, 0}, {1, 1}), std::logic_error);
    CHECK_THROWS_AS(physical_design::detail::wire_straight(lyt, {3, 3}, {3, 4}), std::logic_error);  // empty source

    CHECK(physical_design::detail::rows_between<gate_layout>({0, 0}, {0, 3}) == 2);
    CHECK_THROWS_AS(physical_design::detail::rows_between<gate_layout>({0, 3}, {0, 0}), std::logic_error);

    (void)a;
}
