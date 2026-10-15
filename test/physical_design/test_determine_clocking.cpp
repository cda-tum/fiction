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
 * @brief Tests for `fiction/physical_design/determine_clocking.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"
#include "utils/blueprints/layout_blueprints.hpp"
#include "utils/blueprints/network_blueprints.hpp"
#include "utils/equivalence_checking_utils.hpp"

#include <fiction/layouts/arrangement.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/physical_design/determine_clocking.hpp>
#include <fiction/physical_design/orthogonal.hpp>
#include <fiction/synthesis/network_conversion.hpp>
#include <fiction/types.hpp>

#include <bill/sat/interface/common.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/utils/stopwatch.hpp>

#include <concepts>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::layouts;
using namespace fiction::physical_design;

/**
 * @brief Assigns clock zero to every occupied tile.
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to update.
 */
template <typename Lyt>
    requires requires { requires std::same_as<typename Lyt::object_id, layout_object_id>; }
void remove_clocking(Lyt& lyt)
{
    lyt.foreach_object([&lyt](const auto id) { lyt.assign_clock_number(lyt.get_tile(id), 0); });
}

/**
 * @brief Checks clock assignment with each supported solver.
 * @tparam Lyt Gate-level layout type.
 * @param lyt Layout to clock and compare.
 */
template <typename Lyt>
    requires requires { requires std::same_as<typename Lyt::object_id, layout_object_id>; }
void remove_assign_and_check_clocking(Lyt lyt)
{
    const auto lyt_clone = lyt.clone();

    // for each supported solver
    for (const auto& solver : {
             bill::solvers::ghack,
             bill::solvers::glucose_41,
             bill::solvers::bsat2,
#ifndef BILL_WINDOWS_PLATFORM
             bill::solvers::maple,
             bill::solvers::bmcg,
#endif
         })
    {
        remove_clocking(lyt);

        determine_clocking_stats st{};

        const auto result = determine_clocking(lyt, determine_clocking_params{solver}, &st);

        REQUIRE(result == true);
        CHECK(mockturtle::to_seconds(st.time_total) > 0);

        check_eq(lyt, lyt_clone);
    }
}

TEST_CASE("Determine clock numbers for an empty layout", "[determine-clocking]")
{
    cartesian_gate_layout layout{{5, 5}};

    CHECK(determine_clocking(layout) == true);
}

TEST_CASE("Determine clock numbers for simple layouts", "[determine-clocking]")
{
    remove_assign_and_check_clocking(blueprints::straight_wire_gate_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::and_not_gate_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::or_not_gate_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::use_and_gate_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::res_maj_gate_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::crossing_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::fanout_layout<cartesian_gate_layout>());
    remove_assign_and_check_clocking(blueprints::unbalanced_and_layout<cartesian_gate_layout>());
}

TEST_CASE("Determine clock numbers for a 3-phase layout", "[determine-clocking]")
{
    remove_assign_and_check_clocking(orthogonal<cartesian_gate_layout>(
        fiction::synthesis::convert_network<tec_nt>(blueprints::maj1_network<mockturtle::aig_network>()),
        {.number_of_clock_phases = clocking::num_clks::THREE}));
}

TEST_CASE("Determine clock numbers for non-Cartesian layout topologies", "[determine-clocking]")
{
    SECTION("shifted Cartesian")
    {
        SECTION("odd column")
        {
            constexpr auto arr = arrangement::ODD_COLUMN;

            remove_assign_and_check_clocking(
                blueprints::shifted_cart_and_or_inv_gate_layout<shifted_cartesian_gate_layout>(arr));
        }
        SECTION("even row")
        {
            constexpr auto arr = arrangement::EVEN_ROW;

            remove_assign_and_check_clocking(
                blueprints::row_clocked_and_xor_gate_layout<shifted_cartesian_gate_layout>(arr));
        }
    }
    SECTION("hexagonal")
    {
        SECTION("even row")
        {
            constexpr auto arr = arrangement::EVEN_ROW;

            remove_assign_and_check_clocking(blueprints::row_clocked_and_xor_gate_layout<hexagonal_gate_layout>(arr));
        }
    }
}

TEST_CASE("Determine clock numbers for complex layouts", "[determine-clocking]")
{
    remove_assign_and_check_clocking(orthogonal<cartesian_gate_layout>(
        fiction::synthesis::convert_network<tec_nt>(blueprints::maj1_network<mockturtle::aig_network>())));
    remove_assign_and_check_clocking(orthogonal<cartesian_gate_layout>(
        fiction::synthesis::convert_network<tec_nt>(blueprints::maj4_network<mockturtle::aig_network>())));
    remove_assign_and_check_clocking(orthogonal<cartesian_gate_layout>(
        fiction::synthesis::convert_network<tec_nt>(blueprints::nary_operation_network<mockturtle::aig_network>())));
}

TEST_CASE("Determine clock numbers for a non-clockable layout", "[determine-clocking]")
{
    auto lyt = blueprints::unclockable_gate_layout<cartesian_gate_layout>();

    const auto scheme = lyt.get_clocking_scheme();
    CHECK(determine_clocking(lyt) == false);
    CHECK(lyt.get_clocking_scheme() == scheme);
}

TEST_CASE("Clock determination rejects incomplete or invalid editing states", "[determine-clocking]")
{
    cartesian_gate_layout lyt{{4, 4}, clocking::twoddwave()};
    const auto            original_scheme = lyt.get_clocking_scheme();
    SECTION("Missing input outside output dependencies")
    {
        lyt.create_buf({1, 1});
    }
    SECTION("Nonadjacent declared connection")
    {
        const auto pi = lyt.create_pi("a", {0, 0});
        lyt.create_buf(pi, {3, 3});
    }
    SECTION("Outside the frame")
    {
        lyt.create_pi("a", {-1, 0});
    }
    SECTION("Complete adjacent cycle")
    {
        const auto a = lyt.create_buf({0, 0});
        const auto b = lyt.create_buf(a, {1, 0});
        const auto c = lyt.create_buf(b, {1, 1});
        const auto d = lyt.create_buf(c, {0, 1});
        lyt.connect(d, {a, 0});
    }
    CHECK_THROWS_AS(determine_clocking(lyt), std::invalid_argument);
    CHECK(lyt.get_clocking_scheme() == original_scheme);
}

TEST_CASE("Clock determination uses explicit constants and logical input ports", "[determine-clocking]")
{
    cartesian_gate_layout lyt{{3, 1}, clocking::open()};
    SECTION("Placed constant without PIs")
    {
        const auto constant = lyt.create_gate({}, kitty::dynamic_truth_table{0}, {0, 0});
        lyt.create_po(constant, "zero", {1, 0});
    }
    SECTION("Duplicate source input ports")
    {
        const auto pi   = lyt.create_pi("a", {0, 0});
        const auto gate = lyt.create_lt(pi, pi, {1, 0});
        lyt.create_po(gate, "less", {2, 0});
    }
    REQUIRE(determine_clocking(lyt));
    lyt.foreach_object(
        [&](const auto id)
        {
            lyt.foreach_fanin(id, [&](const auto src)
                              { CHECK(lyt.is_incoming_clocked(lyt.get_tile(id), lyt.get_tile(src))); });
        });
}

TEST_CASE("Clock zones span occupied layers without phantom ground objects", "[determine-clocking]")
{
    cartesian_gate_layout lyt{{3, 1, 2}, clocking::open()};
    SECTION("Floating crossing wire")
    {
        const auto pi   = lyt.create_pi("a", {0, 0});
        const auto wire = lyt.create_buf(pi, {1, 0, 1});
        lyt.create_po(wire, "out", {2, 0});
        CHECK_FALSE(lyt.find_object({1, 0}).has_value());
    }
    SECTION("Occupied ground and crossing layers")
    {
        for (const auto layer : {0, 1})
        {
            const auto pi   = lyt.create_pi("a", {0, 0, layer});
            const auto wire = lyt.create_buf(pi, {1, 0, layer});
            lyt.create_po(wire, "out", {2, 0, layer});
        }
    }
    REQUIRE(determine_clocking(lyt));
    lyt.foreach_object(
        [&](const auto id)
        {
            const auto t = lyt.get_tile(id);
            CHECK(lyt.get_clock_number(t) == lyt.get_clock_number({t.x, t.y, 0}));
            lyt.foreach_fanin(id, [&](const auto src) { CHECK(lyt.is_incoming_clocked(t, lyt.get_tile(src))); });
        });
}

TEST_CASE("Clock determination commits complete clocking values", "[determine-clocking]")
{
    require_allocation_failure_support();
    cartesian_gate_layout original{{3, 1}, clocking::open()};
    const auto            pi   = original.create_pi("a", {0, 0});
    const auto            wire = original.create_buf(pi, {1, 0});
    const auto            po   = original.create_po(wire, "out", {2, 0});
    original.assign_clock_number({9, 9}, 2);
    original.assign_synchronization_element({1, 0}, 2);
    const auto original_scheme = original.get_clocking_scheme();
    for (std::size_t failure{};; ++failure)
    {
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        auto candidate = original;
        try
        {
            allocation_budget  = failure;
            const auto success = determine_clocking(candidate);
            allocation_budget.reset();
            REQUIRE(success);
            CHECK(candidate.get_clock_number({9, 9}) == 2);
            CHECK(candidate.get_synchronization_element({1, 0}) == 2);
            CHECK(candidate.source({wire, 0}) == pi);
            CHECK(candidate.source({po, 0}) == wire);
            CHECK(candidate.is_incoming_clocked({1, 0}, {0, 0}));
            CHECK(candidate.is_incoming_clocked({2, 0}, {1, 0}));
            break;
        }
        catch (const std::bad_alloc&)
        {
            allocation_budget.reset();
            CHECK(candidate.get_clocking_scheme() == original_scheme);
            CHECK(candidate.get_synchronization_element({1, 0}) == 2);
            CHECK(candidate.source({wire, 0}) == pi);
            CHECK(candidate.source({po, 0}) == wire);
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
    }
}

TEST_CASE("Clock determination visits sparse occupancy independently of frame area", "[determine-clocking]")
{
    gate_layout<cartesian_layout> lyt{{1'000'000, 1'000'000}, clocking::open()};
    const auto                    pi   = lyt.create_pi("a", {0, 0});
    const auto                    wire = lyt.create_buf(pi, {1, 0});
    lyt.create_po(wire, "out", {2, 0});
    REQUIRE(determine_clocking(lyt));
    CHECK(lyt.size() == 3);
    CHECK(lyt.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(lyt.is_incoming_clocked({2, 0}, {1, 0}));
}
