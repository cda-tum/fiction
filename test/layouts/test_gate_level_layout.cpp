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
 * @brief Tests for `fiction/layouts/gate_level_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#include <catch2/catch_test_macros.hpp>

#include "utils/allocation_failure.hpp"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/layout_base.hpp>

#include <kitty/bit_operations.hpp>
#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <mockturtle/traits.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::test;
using namespace fiction::layouts;

TEST_CASE("Object identity survives placement and stale IDs reject reuse", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{8, 8}};
    const auto                          a    = lyt.create_pi("a", {0, 0});
    const auto                          b    = lyt.create_pi("b", {1, 0});
    const auto                          gate = lyt.create_lt(a, b, {1, 1});
    const auto                          id   = gate;
    lyt.move_node(a, {-2, 3, 7});
    CHECK(lyt.get_tile(a) == layout_base::coordinate{-2, 3, 7});
    CHECK(lyt.source({id, 0}) == a);
    CHECK(lyt.source({id, 1}) == b);
    CHECK_THROWS_AS(lyt.move_node(id, lyt.get_tile(b)), std::invalid_argument);
    CHECK(lyt.get_tile(id) == layout_base::coordinate{1, 1});
    CHECK_THROWS_AS(lyt.create_pi("occupied", {1, 1}), std::invalid_argument);
    CHECK(lyt.size() == 3);
    lyt.remove(a);
    CHECK_FALSE(lyt.source({id, 0}).has_value());
    CHECK(lyt.source({id, 1}) == b);
    const auto replacement = lyt.create_pi("replacement", {0, 0});
    CHECK(replacement != a);
    CHECK_FALSE(lyt.contains(a));
    CHECK_THROWS_AS(lyt.connect(a, {id, 0}), std::invalid_argument);
    CHECK(lyt.source({id, 1}) == b);
    lyt.connect(replacement, {id, 0});
    CHECK(lyt.source({id, 0}) == replacement);
}

TEST_CASE("Copies isolate geometry, terminals, and connectivity", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> original{{4, 4}};
    const auto                          a    = original.create_pi("a", {0, 0});
    const auto                          b    = original.create_pi("b", {1, 0});
    const auto                          gate = original.create_and(a, b, {1, 1});
    auto                                copy = original;
    copy.move_node(gate, {2, 2});
    copy.disconnect({gate, 0});
    copy.set_input_order(std::vector{b, a});
    copy.resize({9, 9});
    CHECK(original.get_tile(gate) == layout_base::coordinate{1, 1});
    CHECK(original.source({gate, 0}) == a);
    CHECK(original.pi_at(0) == a);
    CHECK(copy.pi_at(0) == b);
    CHECK(original.width() != copy.width());
    CHECK_THROWS_AS(copy.set_input_order(std::vector{a, a}), std::invalid_argument);
    CHECK(copy.pi_at(0) == b);
}

TEST_CASE("Connections expose declared ports despite physical violations", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{1, 1}, clocking::twoddwave()};
    const auto                          a    = lyt.create_pi("a", {-5, 0});
    const auto                          gate = lyt.create_buf(a, {99, 0, 3});
    CHECK(lyt.source({gate, 0}) == a);
    std::vector<gate_level_layout<cartesian_layout>::input_port> sinks{};
    lyt.foreach_sink(a, [&](const auto port) { sinks.push_back(port); });
    CHECK(sinks == std::vector{gate_level_layout<cartesian_layout>::input_port{gate, 0}});
    CHECK_THROWS_AS(lyt.connect(a, {gate, 1}), std::out_of_range);
    CHECK(lyt.source({gate, 0}) == a);
    CHECK_FALSE(lyt.find_object({0, 0}).has_value());
}

TEST_CASE("Empty layouts own no implicit constants and require placement", "[gate-layout-editing]")
{
    /** @brief Native layout type whose creation API requires placement. */
    using layout = gate_level_layout<cartesian_layout>;
    /** @brief Empty layout without implicit constant objects. */
    const layout lyt{};
    CHECK(lyt.size() == 0);
    CHECK_FALSE(mockturtle::is_network_type_v<layout>);
    static_assert(!std::is_invocable_v<decltype(&layout::create_pi), layout&, const std::string&>);
}

TEST_CASE("Layouts move without throwing so containers move them on growth", "[gate-layout-editing]")
{
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<gate_level_layout<cartesian_layout>>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<gate_level_layout<cartesian_layout>>);
}

TEST_CASE("Objects with more inputs than the inline capacity keep ordered ports", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout>                         lyt{{6, 6}};
    std::vector<gate_level_layout<cartesian_layout>::object_id> pis{};
    pis.reserve(5);
    for (uint32_t i = 0; i < 5; ++i)
    {
        pis.push_back(lyt.create_pi("pi" + std::to_string(i), {static_cast<int64_t>(i), 0}));
    }
    kitty::dynamic_truth_table parity{5};
    kitty::create_from_hex_string(parity, "96696996");
    const auto gate = lyt.create_node(pis, parity, {2, 2});
    CHECK(lyt.input_count(gate) == 5);
    CHECK(lyt.fanin_size(gate) == 5);
    for (uint32_t i = 0; i < 5; ++i)
    {
        CHECK(lyt.source({gate, i}) == pis[i]);
    }
    lyt.disconnect({gate, 3});
    CHECK(lyt.fanin_size(gate) == 4);
    CHECK_FALSE(lyt.source({gate, 3}).has_value());
    CHECK(lyt.source({gate, 4}) == pis[4]);
    CHECK_THROWS_AS(lyt.source({gate, 5}), std::out_of_range);
}

TEST_CASE("Moved layouts leave reusable empty sources", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> source{{4, 4}};
    const auto                          removed = source.create_pi("removed", {0, 0});
    source.create_pi("kept", {1, 0});
    source.remove(removed);
    auto destination = std::move(source);
    // The layout contract permits moved-from reuse.
    // NOLINTNEXTLINE(bugprone-use-after-move,clang-analyzer-cplusplus.Move,hicpp-invalid-access-moved)
    REQUIRE(source.is_empty());
    const auto reused = source.create_pi("reused", {0, 0});
    CHECK(source.contains(reused));
    CHECK(destination.num_pis() == 1);
    source = std::move(destination);
    // The layout contract permits moved-from reuse.
    // NOLINTNEXTLINE(bugprone-use-after-move,clang-analyzer-cplusplus.Move,hicpp-invalid-access-moved)
    REQUIRE(destination.is_empty());
    CHECK(destination.create_pi("new", {2, 0}).generation != 0);
    CHECK(source.get_input_name(0) == "kept");
}

TEST_CASE("Deletion disconnects self loops and duplicate destination inputs", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{4, 4}};
    const auto                          input = lyt.create_pi("a", {0, 0});
    const auto                          gate  = lyt.create_and(input, input, {1, 0});
    CHECK(lyt.fanout_size(input) == 2);
    lyt.disconnect({gate, 0});
    CHECK(lyt.fanout_size(input) == 1);
    CHECK(lyt.source({gate, 1}) == input);
    lyt.connect(gate, {gate, 0});
    lyt.remove(gate);
    CHECK(lyt.fanout_size(input) == 0);
    CHECK(lyt.size() == 1);
}

TEST_CASE("Deep copy clocked layout", "[clocked-layout]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;

    clk_lyt original{{6, 6, 1}, clocking::twoddwave()};
    original.assign_clock_number({0, 0}, 3);

    auto copy = original.clone();

    CHECK(copy.get_clock_number({0, 0}) == 3);
    original.assign_clock_number({0, 0}, 1);
    CHECK(copy.get_clock_number({0, 0}) == 3);
    copy.assign_clock_number({0, 0}, 2);
    CHECK(original.get_clock_number({0, 0}) == 1);

    copy.resize({11, 11, 2});
    copy.replace_clocking_scheme(clocking::use());

    CHECK(original.width() == 6);
    CHECK(original.height() == 6);
    CHECK(original.layers() == 1);
    CHECK(original.is_clocking_scheme(clocking::TWODDWAVE_NAME));

    CHECK(copy.width() == 11);
    CHECK(copy.height() == 11);
    CHECK(copy.layers() == 2);
    CHECK(copy.is_clocking_scheme(clocking::USE_NAME));
}

TEST_CASE("Borrowed clocking schemes observe overrides and isolate clones", "[clocked-layout]")
{
    /** Clocked gate layout under test. */
    using clk_lyt = gate_level_layout<cartesian_layout>;

    clk_lyt     original{{6, 6, 1}, clocking::twoddwave()};
    const auto& scheme = original.get_clocking_scheme();
    CHECK((std::is_same_v<decltype(original.get_clocking_scheme()), const clk_lyt::clocking_scheme_t&>));
    CHECK(noexcept(original.get_clocking_scheme()));

    auto copy = original.clone();
    original.assign_clock_number({0, 0}, 3);
    CHECK(scheme(0, 0) == 3);
    CHECK(copy.get_clocking_scheme()(0, 0) == 0);
    copy.assign_clock_number({0, 0}, 2);
    CHECK(scheme(0, 0) == 3);
    CHECK(copy.get_clocking_scheme()(0, 0) == 2);
}

TEST_CASE("Clock zone assignment", "[clocked-layout]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;

    clk_lyt layout{clk_lyt::extent{2, 2, 1}, clocking::twoddwave()};

    SECTION("2DDWave Clocking")
    {
        CHECK(layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(!layout.is_clocking_scheme(clocking::RES_NAME));
        CHECK(layout.is_regularly_clocked());
        CHECK(layout.num_clocks() == 4);

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 1);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);

        // a clock zone spans every layer of its tile
        CHECK(layout.get_clock_number({1, 1, 1}) == 3);
        layout.assign_clock_number({0, 1, 1}, 3);
        CHECK(layout.get_clock_number({0, 1, 0}) == 3);
        layout.assign_clock_number({0, 1}, 2);

        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
        CHECK(!layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));

        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
    }

    SECTION("Replace with USE")
    {
        layout.replace_clocking_scheme(clocking::use());

        CHECK(!layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
        CHECK(layout.is_clocking_scheme(clocking::USE_NAME));
        CHECK(layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 1);
        CHECK(layout.get_clock_number({0, 1}) == 3);
        CHECK(layout.get_clock_number({1, 1}) == 2);

        CHECK(layout.is_incoming_clocked({0, 1}, {1, 1}));
        CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
        CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {0, 0}));
        CHECK(!layout.is_incoming_clocked({1, 1}, {1, 1}));

        CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
        CHECK(layout.is_outgoing_clocked({0, 1}, {0, 0}));
        CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {0, 1}));
        CHECK(!layout.is_outgoing_clocked({0, 0}, {1, 1}));
        CHECK(!layout.is_outgoing_clocked({1, 1}, {1, 1}));

        layout.assign_clock_number({1, 0}, 2);
        layout.assign_clock_number({0, 1}, 2);
        layout.assign_clock_number({1, 1}, 3);

        CHECK(!layout.is_regularly_clocked());

        CHECK(layout.get_clock_number({0, 0}) == 0);
        CHECK(layout.get_clock_number({1, 0}) == 2);
        CHECK(layout.get_clock_number({0, 1}) == 2);
        CHECK(layout.get_clock_number({1, 1}) == 3);
    }
}

TEST_CASE("Iteration over clocking zones", "[clocked-layout]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;

    const clk_lyt layout{clk_lyt::extent{3, 3, 1}, clocking::twoddwave()};

    CHECK(layout.incoming_clocked_zones({0, 0}).empty());
    CHECK(layout.outgoing_clocked_zones({2, 2}).empty());

    auto v1 = layout.incoming_clocked_zones({1, 1});
    auto s1 = std::set<clk_lyt::coordinate>{v1.cbegin(), v1.cend()};
    auto s2 = std::set<clk_lyt::coordinate>{{{1, 0}, {0, 1}}};

    CHECK(s1 == s2);

    layout.foreach_incoming_clocked_zone({1, 1}, [&s2](const auto& cz) { CHECK(s2.count(cz) > 0); });

    auto v3 = layout.outgoing_clocked_zones({1, 1});
    auto s3 = std::set<clk_lyt::coordinate>{v3.cbegin(), v3.cend()};
    auto s4 = std::set<clk_lyt::coordinate>{{{1, 2}, {2, 1}}};

    layout.foreach_outgoing_clocked_zone({1, 1}, [&s4](const auto& cz) { CHECK(s4.count(cz) > 0); });

    CHECK(s3 == s4);

    layout.foreach_outgoing_clocked_zone({1, 1}, [&s4](const auto& cz) { CHECK(s4.count(cz) > 0); });
}

TEST_CASE("Clocked layout properties", "[clocked-layout]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;

    SECTION("2DDWave Clocking")
    {
        const clk_lyt layout{clk_lyt::extent{3, 3, 1}, clocking::twoddwave()};

        CHECK(layout.in_degree({0, 0}) == static_cast<clk_lyt::degree_t>(0));
        CHECK(layout.in_degree({1, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({1, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clk_lyt::degree_t>(0));

        CHECK(layout.degree({0, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clk_lyt::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clk_lyt::degree_t>(2));
    }
    SECTION("USE Clocking")
    {
        const clk_lyt layout{clk_lyt::extent{3, 3, 1}, clocking::use()};

        CHECK(layout.in_degree({0, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({1, 0}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.in_degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.in_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));

        CHECK(layout.out_degree({1, 1}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.out_degree({1, 2}) == static_cast<clk_lyt::degree_t>(1));
        CHECK(layout.out_degree({2, 2}) == static_cast<clk_lyt::degree_t>(1));

        CHECK(layout.degree({0, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 0}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 0}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 1}) == static_cast<clk_lyt::degree_t>(4));
        CHECK(layout.degree({0, 2}) == static_cast<clk_lyt::degree_t>(2));
        CHECK(layout.degree({1, 2}) == static_cast<clk_lyt::degree_t>(3));
        CHECK(layout.degree({2, 2}) == static_cast<clk_lyt::degree_t>(2));
    }
}

TEST_CASE("Synchronization element layout traits", "[synchronization-element-layout]")
{
    using se_layout = gate_level_layout<cartesian_layout>;

    CHECK(requires(const se_layout& lyt) { lyt.num_se(); });
}

TEST_CASE("Deep copy synchronization element layout", "[synchronization-element-layout]")
{
    using se_layout = gate_level_layout<cartesian_layout>;

    se_layout original{{6, 6, 1}, clocking::twoddwave()};
    original.assign_synchronization_element({0, 0}, 1);
    original.assign_synchronization_element({1, 0}, 2);

    auto copy = original.clone();

    copy.resize({11, 11, 2});
    copy.replace_clocking_scheme(clocking::use());
    copy.assign_synchronization_element({0, 0}, 2);
    copy.assign_synchronization_element({1, 0}, 3);

    CHECK(original.width() == 6);
    CHECK(original.height() == 6);
    CHECK(original.layers() == 1);
    CHECK(original.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    CHECK(original.get_synchronization_element({0, 0}) == 1);
    CHECK(original.get_synchronization_element({1, 0}) == 2);

    CHECK(copy.width() == 11);
    CHECK(copy.height() == 11);
    CHECK(copy.layers() == 2);
    CHECK(copy.is_clocking_scheme(clocking::USE_NAME));
    CHECK(copy.get_synchronization_element({0, 0}) == 2);
    CHECK(copy.get_synchronization_element({1, 0}) == 3);
}

TEST_CASE("Shifted clocking with synchronization elements", "[synchronization-element-layout]")
{
    using se_layout = gate_level_layout<cartesian_layout>;

    se_layout layout{se_layout::extent{3, 3, 1}, clocking::twoddwave()};

    layout.assign_synchronization_element({1, 1}, 1);

    CHECK(layout.is_clocking_scheme(clocking::TWODDWAVE_NAME));
    CHECK(layout.is_regularly_clocked());
    CHECK(layout.num_clocks() == 4);

    CHECK(layout.get_clock_number({0, 0}) == 0);
    CHECK(layout.get_clock_number({1, 0}) == 1);
    CHECK(layout.get_clock_number({0, 1}) == 1);
    CHECK(layout.get_clock_number({1, 1}) == 2);
    CHECK(layout.get_clock_number({2, 1}) == 3);
    CHECK(layout.get_clock_number({1, 2}) == 3);
    CHECK(layout.get_clock_number({2, 2}) == 0);

    CHECK(layout.is_incoming_clocked({1, 0}, {0, 0}));
    CHECK(layout.is_incoming_clocked({0, 1}, {0, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {0, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 0}));
    CHECK(layout.is_incoming_clocked({1, 1}, {2, 1}));
    CHECK(layout.is_incoming_clocked({1, 1}, {1, 2}));

    CHECK(layout.is_outgoing_clocked({0, 0}, {1, 0}));
    CHECK(layout.is_outgoing_clocked({0, 0}, {0, 1}));
    CHECK(layout.is_outgoing_clocked({0, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 0}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({2, 1}, {1, 1}));
    CHECK(layout.is_outgoing_clocked({1, 2}, {1, 1}));
}

TEST_CASE("Iteration over synchronization elements", "[synchronization-element-layout]")
{
    using se_layout = gate_level_layout<cartesian_layout>;

    se_layout layout{se_layout::extent{3, 3, 1}, clocking::twoddwave()};

    layout.assign_synchronization_element({0, 1}, 1);
    layout.assign_synchronization_element({1, 0}, 1);
    layout.assign_synchronization_element({1, 2}, 1);
    layout.assign_synchronization_element({2, 1}, 1);

    CHECK(layout.incoming_clocked_zones({0, 0}).size() == 2);
    CHECK(layout.outgoing_clocked_zones({2, 2}).size() == 2);

    const auto v1 = layout.incoming_clocked_zones({1, 1});
    const auto s1 = std::set<se_layout::coordinate>{v1.cbegin(), v1.cend()};
    const auto s2 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s1 == s2);

    const auto v3 = layout.outgoing_clocked_zones({1, 1});
    const auto s3 = std::set<se_layout::coordinate>{v3.cbegin(), v3.cend()};
    const auto s4 = std::set<se_layout::coordinate>{{{1, 0}, {0, 1}, {1, 2}, {2, 1}}};

    CHECK(s3 == s4);
}

TEST_CASE("Synchronization element layout properties", "[synchronization-element-layout]")
{
    using se_layout = gate_level_layout<cartesian_layout>;

    se_layout layout{se_layout::extent{3, 3, 1}, clocking::twoddwave()};

    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 0}, 0);
    CHECK(layout.num_se() == 0);
    layout.assign_synchronization_element({0, 1}, 1);
    CHECK(layout.num_se() == 1);
    layout.assign_synchronization_element({1, 0}, 1);
    CHECK(layout.num_se() == 2);
    layout.assign_synchronization_element({1, 2}, 2);
    CHECK(layout.num_se() == 3);
    layout.assign_synchronization_element({2, 1}, 2);
    CHECK(layout.num_se() == 4);

    CHECK(layout.is_synchronization_element({0, 1}));
    CHECK(layout.is_synchronization_element({1, 0}));
    CHECK(layout.is_synchronization_element({1, 2}));
    CHECK(layout.is_synchronization_element({2, 1}));

    CHECK(!layout.is_synchronization_element({0, 0}));
    CHECK(!layout.is_synchronization_element({1, 1}));
    CHECK(!layout.is_synchronization_element({2, 0}));
    CHECK(!layout.is_synchronization_element({0, 2}));
    CHECK(!layout.is_synchronization_element({2, 2}));

    CHECK(layout.get_synchronization_element({0, 1}) == 1);
    CHECK(layout.get_synchronization_element({1, 0}) == 1);
    CHECK(layout.get_synchronization_element({1, 2}) == 2);
    CHECK(layout.get_synchronization_element({2, 1}) == 2);

    CHECK(layout.get_synchronization_element({0, 0}) == 0);
    CHECK(layout.get_synchronization_element({1, 1}) == 0);
    CHECK(layout.get_synchronization_element({2, 0}) == 0);
    CHECK(layout.get_synchronization_element({0, 2}) == 0);
    CHECK(layout.get_synchronization_element({2, 2}) == 0);

    CHECK(layout.in_degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.in_degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.in_degree({1, 1}) == static_cast<se_layout::degree_t>(4));

    CHECK(layout.out_degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.out_degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.out_degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.out_degree({2, 2}) == static_cast<se_layout::degree_t>(2));

    CHECK(layout.degree({0, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 0}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 0}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 1}) == static_cast<se_layout::degree_t>(4));
    CHECK(layout.degree({0, 2}) == static_cast<se_layout::degree_t>(2));
    CHECK(layout.degree({1, 2}) == static_cast<se_layout::degree_t>(3));
    CHECK(layout.degree({2, 2}) == static_cast<se_layout::degree_t>(2));
}

TEST_CASE("Elementary truth tables retain logical input order", "[gate-layout-editing]")
{
    using layout         = gate_level_layout<cartesian_layout>;
    using binary_creator = layout::object_id (layout::*)(layout::object_id, layout::object_id, const layout::tile&);
    const std::array<std::pair<binary_creator, uint64_t>, 10> creators{{{&layout::create_and, 0x8},
                                                                        {&layout::create_nand, 0x7},
                                                                        {&layout::create_or, 0xe},
                                                                        {&layout::create_nor, 0x1},
                                                                        {&layout::create_lt, 0x2},
                                                                        {&layout::create_ge, 0xd},
                                                                        {&layout::create_gt, 0x4},
                                                                        {&layout::create_le, 0xb},
                                                                        {&layout::create_xor, 0x6},
                                                                        {&layout::create_xnor, 0x9}}};
    layout                                                    lyt{{16, 4}};
    const auto                                                a = lyt.create_pi("a", {0, 0});
    const auto                                                b = lyt.create_pi("b", {1, 0});
    int32_t                                                   x{};
    for (const auto [create, literal] : creators)
    {
        const auto                 gate = (lyt.*create)(a, b, {x++, 1});
        kitty::dynamic_truth_table expected{2};
        /** @brief The truth-table word supplied to the constructor. */
        const std::array words{literal};
        kitty::create_from_words(expected, words.cbegin(), words.cend());
        CHECK(lyt.node_function(gate) == expected);
        CHECK(lyt.source({gate, 0}) == a);
        CHECK(lyt.source({gate, 1}) == b);
    }
    const auto wire = lyt.create_buf(a, {0, 2});
    const auto inv  = lyt.create_not(a, {1, 2});
    const auto maj  = lyt.create_maj(a, b, wire, {2, 2});
    CHECK(lyt.is_buf(wire));
    CHECK(lyt.is_inv(inv));
    CHECK(lyt.is_maj(maj));
    kitty::dynamic_truth_table identity{1};
    kitty::create_nth_var(identity, 0);
    const auto generic_wire = lyt.create_node({a}, identity, {3, 2});
    CHECK(lyt.is_wire(generic_wire));
    CHECK_FALSE(lyt.is_gate(generic_wire));
    uint32_t gates{};
    lyt.foreach_gate([&](const auto) { ++gates; });
    CHECK(gates == lyt.num_gates());
}

TEST_CASE("Removing terminals updates declared order and sparse names", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{5, 5}};
    const auto                          a      = lyt.create_pi("a", {0, 0});
    const auto                          b      = lyt.create_pi("b", {1, 0});
    const auto                          c      = lyt.create_pi("c", {2, 0});
    const auto                          output = lyt.create_po(c, "result", {2, 1});
    lyt.set_input_order(std::vector{c, a, b});
    lyt.remove(a);
    CHECK(lyt.num_pis() == 2);
    CHECK(lyt.pi_at(0) == c);
    CHECK(lyt.pi_at(1) == b);
    lyt.set_name(b, "");
    CHECK_FALSE(lyt.has_name(b));
    lyt.remove(output);
    CHECK(lyt.num_pos() == 0);
    CHECK(lyt.fanout_size(c) == 0);
}

TEST_CASE("Copied capabilities remain independent", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> original{{4, 4}, clocking::twoddwave()};
    original.assign_clock_number({1, 1}, 3);
    original.assign_synchronization_element({1, 1}, 2);
    original.obstruct_coordinate({2, 2});
    original.obstruct_connection({0, 0}, {1, 0});
    auto copy = original;
    copy.assign_clock_number({1, 1}, 0);
    copy.assign_synchronization_element({1, 1}, 4);
    copy.clear_obstructed_coordinates();
    copy.clear_obstructed_connections();
    CHECK(original.get_clock_number({1, 1}) == 3);
    CHECK(original.get_synchronization_element({1, 1}) == 2);
    CHECK(original.is_obstructed_coordinate({2, 2}));
    CHECK(original.is_obstructed_connection({0, 0}, {1, 0}));
}

TEST_CASE("Sparse clock metadata enumerates assigned zones independently of frame area", "[gate-layout-clocking]")
{
    gate_level_layout<cartesian_layout> lyt{{1'000'000, 1'000'000}};
    lyt.assign_clock_number({999'999, 999'999}, 2);
    lyt.assign_synchronization_element({-1, 4}, 7);
    uint32_t clocks{};
    lyt.get_clocking_scheme().foreach_override(
        [&](const auto x, const auto y, const auto number)
        {
            CHECK(x == 999'999);
            CHECK(y == 999'999);
            CHECK(number == 2);
            ++clocks;
        });
    uint32_t delays{};
    lyt.foreach_synchronization_element(
        [&](const auto& zone, const auto delay)
        {
            CHECK(zone == layout_base::coordinate{-1, 4});
            CHECK(delay == 7);
            ++delays;
        });
    CHECK(clocks == 1);
    CHECK(delays == 1);
}

TEST_CASE("Moved-from layouts recover from interrupted cache initialization", "[gate-layout-editing]")
{
    require_allocation_failure_support();
    using layout = gate_level_layout<cartesian_layout>;
    for (std::size_t failure = 0;; ++failure)
    {
        REQUIRE(failure < ALLOCATION_FAILURE_ATTEMPT_LIMIT);
        layout     source{{4, 4}};
        const auto original = source.create_pi("original", {0, 0});
        /** @brief Destination that retains the original object after moving the layout. */
        const layout destination{std::move(source)};
        bool         created{};
        allocation_budget = failure;
        try
        {
            // The layout contract permits moved-from reuse.
            // NOLINTNEXTLINE(bugprone-use-after-move,clang-analyzer-cplusplus.Move,hicpp-invalid-access-moved)
            source.create_pi("first", {0, 0});
            created = true;
        }
        catch (const std::bad_alloc&)
        {
            created = false;
        }
        catch (...)
        {
            allocation_budget.reset();
            throw;
        }
        allocation_budget.reset();
        source.clear_tile({0, 0});
        const auto a    = source.create_pi("a", {0, 0});
        const auto b    = source.create_pi("b", {1, 0});
        const auto gate = source.create_and(a, b, {1, 1});
        CHECK(source.node_function(a).num_vars() == 1);
        CHECK(source.node_function(gate).num_vars() == 2);
        CHECK(kitty::get_bit(source.node_function(gate), 3));
        CHECK_FALSE(kitty::get_bit(source.node_function(gate), 0));
        CHECK(destination.get_name(original) == "original");
        if (created)
        {
            break;
        }
    }
}

TEST_CASE("Object visitors accept move-only lvalues and temporaries", "[gate-layout-editing]")
{
    /** @brief Layout with enough live objects to test early termination. */
    gate_level_layout<cartesian_layout> lyt{{3, 1, 1}};
    lyt.create_pi("a", {0, 0});
    lyt.create_pi("b", {1, 0});
    lyt.create_pi("c", {2, 0});

    /** @brief Counts visits and stops after two objects. */
    struct move_only_visitor
    {
        /** @brief Owns the call count. */
        std::unique_ptr<uint32_t> calls;
        /** @brief Reports calls externally. */
        uint32_t& observed;
        /**
         * @brief Visits as an lvalue and stops after two objects.
         * @return Whether another object should be visited.
         */
        bool operator()(layout_object_id) &
        {
            observed = ++*calls;
            return observed < 2;
        }
    };

    /** @brief Visits reported by the lvalue callback. */
    uint32_t observed{};
    /** @brief Move-only callback passed as an lvalue. */
    move_only_visitor visitor{.calls = std::make_unique<uint32_t>(0), .observed = observed};
    lyt.foreach_node(visitor);
    CHECK(observed == 2);

    /** @brief Visits reported by the temporary callback. */
    uint32_t observed_temporary{};
    lyt.foreach_node(move_only_visitor{.calls = std::make_unique<uint32_t>(0), .observed = observed_temporary});
    CHECK(observed_temporary == 2);
}
