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

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/gate_level_layout.hpp>

#include <kitty/constructors.hpp>
#include <mockturtle/traits.hpp>

#include <array>
#include <set>
#include <stdexcept>
#include <vector>

using namespace fiction;
using namespace fiction::layouts;

TEST_CASE("Object identity survives placement and stale IDs reject reuse", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{8, 8}};
    const auto                          a    = lyt.create_pi("a", {0, 0});
    const auto                          b    = lyt.create_pi("b", {1, 0});
    const auto                          gate = lyt.create_lt(a, b, {1, 1});
    const auto                          id   = gate.object;
    lyt.move_node(a.object, {-2, 3, 7});
    CHECK(lyt.get_tile(a.object) == layout_base::coordinate{-2, 3, 7});
    CHECK(lyt.source({id, 0}) == a);
    CHECK(lyt.source({id, 1}) == b);
    CHECK_THROWS_AS(lyt.move_node(id, lyt.get_tile(b.object)), std::invalid_argument);
    CHECK(lyt.get_tile(id) == layout_base::coordinate{1, 1});
    CHECK_THROWS_AS(lyt.create_pi("occupied", {1, 1}), std::invalid_argument);
    CHECK(lyt.size() == 3);
    lyt.remove(a.object);
    CHECK_FALSE(lyt.source({id, 0}).has_value());
    CHECK(lyt.source({id, 1}) == b);
    const auto replacement = lyt.create_pi("replacement", {0, 0});
    CHECK(replacement.object != a.object);
    CHECK_FALSE(lyt.contains(a.object));
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
    copy.move_node(gate.object, {2, 2});
    copy.disconnect({gate.object, 0});
    copy.set_input_order(std::vector{b.object, a.object});
    copy.resize({9, 9});
    CHECK(original.get_tile(gate.object) == layout_base::coordinate{1, 1});
    CHECK(original.source({gate.object, 0}) == a);
    CHECK(original.pi_at(0) == a.object);
    CHECK(copy.pi_at(0) == b.object);
    CHECK(original.width() != copy.width());
    CHECK_THROWS_AS(copy.set_input_order(std::vector{a.object, a.object}), std::invalid_argument);
    CHECK(copy.pi_at(0) == b.object);
}

TEST_CASE("Connections expose declared ports despite physical violations", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{1, 1}, clocking::twoddwave()};
    const auto                          a    = lyt.create_pi("a", {-5, 0});
    const auto                          gate = lyt.create_buf(a, {99, 0, 3});
    CHECK(lyt.source({gate.object, 0}) == a);
    std::vector<gate_level_layout<cartesian_layout>::input_port> sinks{};
    lyt.foreach_sink(a, [&](const auto port) { sinks.push_back(port); });
    CHECK(sinks == std::vector{gate_level_layout<cartesian_layout>::input_port{gate.object, 0}});
    CHECK_THROWS_AS(lyt.connect({a.object, 1}, {gate.object, 0}), std::out_of_range);
    CHECK_THROWS_AS(lyt.connect(a, {gate.object, 1}), std::out_of_range);
    CHECK(lyt.source({gate.object, 0}) == a);
    CHECK_FALSE(lyt.find_object({0, 0}).has_value());
}

TEST_CASE("Empty layouts own no implicit constants and require placement", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{};
    CHECK(lyt.size() == 0);
    CHECK_FALSE(mockturtle::is_network_type_v<decltype(lyt)>);
    static_assert(!std::is_invocable_v<decltype(&decltype(lyt)::create_pi), decltype(lyt)&, const std::string&>);
    CHECK(lyt.size() == 0);
}

TEST_CASE("Moved layouts leave reusable empty sources", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> source{{4, 4}};
    const auto                          removed = source.create_pi("removed", {0, 0});
    source.create_pi("kept", {1, 0});
    source.remove(removed.object);
    auto destination = std::move(source);
    REQUIRE(source.is_empty());
    const auto reused = source.create_pi("reused", {0, 0});
    CHECK(source.contains(reused.object));
    CHECK(destination.num_pis() == 1);
    source = std::move(destination);
    REQUIRE(destination.is_empty());
    CHECK(destination.create_pi("new", {2, 0}).object.generation != 0);
    CHECK(source.get_input_name(0) == "kept");
}

TEST_CASE("Deletion disconnects self loops and duplicate destination inputs", "[gate-layout-editing]")
{
    gate_level_layout<cartesian_layout> lyt{{4, 4}};
    const auto                          input = lyt.create_pi("a", {0, 0});
    const auto                          gate  = lyt.create_and(input, input, {1, 0});
    CHECK(lyt.fanout_size(input.object) == 2);
    lyt.disconnect({gate.object, 0});
    CHECK(lyt.fanout_size(input.object) == 1);
    CHECK(lyt.source({gate.object, 1}) == input);
    lyt.connect(gate, {gate.object, 0});
    lyt.remove(gate.object);
    CHECK(lyt.fanout_size(input.object) == 0);
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
    using layout = gate_level_layout<cartesian_layout>;
    using binary_creator =
        layout::output_port (layout::*)(layout::output_port, layout::output_port, const layout::tile&);
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
        kitty::create_from_words(expected, &literal, &literal + 1);
        CHECK(lyt.node_function(gate.object) == expected);
        CHECK(lyt.source({gate.object, 0}) == a);
        CHECK(lyt.source({gate.object, 1}) == b);
    }
    const auto wire = lyt.create_buf(a, {0, 2});
    const auto inv  = lyt.create_not(a, {1, 2});
    const auto maj  = lyt.create_maj(a, b, wire, {2, 2});
    CHECK(lyt.is_buf(wire.object));
    CHECK(lyt.is_inv(inv.object));
    CHECK(lyt.is_maj(maj.object));
    kitty::dynamic_truth_table identity{1};
    kitty::create_nth_var(identity, 0);
    const auto generic_wire = lyt.create_node({a}, identity, {3, 2});
    CHECK(lyt.is_wire(generic_wire.object));
    CHECK_FALSE(lyt.is_gate(generic_wire.object));
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
    lyt.set_input_order(std::vector{c.object, a.object, b.object});
    lyt.remove(a.object);
    CHECK(lyt.num_pis() == 2);
    CHECK(lyt.pi_at(0) == c.object);
    CHECK(lyt.pi_at(1) == b.object);
    lyt.set_name(b, "");
    CHECK_FALSE(lyt.has_name(b));
    lyt.remove(output.object);
    CHECK(lyt.num_pos() == 0);
    CHECK(lyt.fanout_size(c.object) == 0);
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
