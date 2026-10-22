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
 * @brief Tests for `fiction/physical_design/path_finding/a_star.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author OpenAI Codex
 */

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/layout_base.hpp>
#include <fiction/layouts/obstructions.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::physical_design::path_finding;

TEST_CASE("A* preserves priority after improving a queued coordinate", "[A*]")
{
    /** @brief Six-tile geometry with two competing routes. */
    const cartesian_layout layout{{2, 3}};
    /** @brief Eastbound costs for each row. */
    const std::array<uint32_t, 3> east{17, 25, 22};
    /** @brief Southbound costs for the first two rows. */
    const std::array<uint32_t, 4> south{14, 2, 2, 15};
    /** @brief Directed costs; reverse steps cost 30. */
    const cost_functor<uint32_t> cost{[&](const auto& source, const auto& target)
                                      {
                                          if (target.x > source.x)
                                          {
                                              return east[static_cast<std::size_t>(source.y)];
                                          }
                                          if (target.y > source.y)
                                          {
                                              return south[static_cast<std::size_t>(2 * source.y + source.x)];
                                          }
                                          return uint32_t{30};
                                      }};
    /** @brief Admissible heuristic for the weighted search. */
    const distance_functor<uint32_t> zero{[](const auto&, const auto&) { return uint32_t{0}; }};
    /** @brief Minimum-cost route, whose edge costs sum to 34. */
    const auto path = a_star(layout, {{0, 0}, {1, 2}}, zero, cost);
    /** @brief Accumulated cost of the returned route. */
    uint32_t total_cost{};
    REQUIRE_FALSE(path.empty());
    for (std::size_t i = 1; i < path.size(); ++i)
    {
        total_cost += cost(path[i - 1], path[i]);
    }
    CHECK(total_cost == 34);
}

TEST_CASE("A* on 2x2 layouts", "[A*]")
{
    using lyt = cartesian_layout;

    SECTION("coordinate paths")
    {
        const lyt layout{lyt::extent{2, 2, 1}};

        SECTION("(0,0) to (1,1)")  // path of length 3
        {
            const auto path = a_star(layout, {.source = {0, 0}, .target = {1, 1}});
            const auto dist = a_star_distance(layout, {0, 0}, {1, 1});

            CHECK(path.size() == 3);
            CHECK(dist == 2);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{1, 1});
            // path could either go east-south or south-east
            CHECK((path[1] == fiction::layouts::layout_base::coordinate{1, 0} ||
                   path[1] == fiction::layouts::layout_base::coordinate{0, 1}));
        }
        SECTION("(1,1) to (0,0)")  // path of length 3
        {
            const auto path = a_star(layout, {.source = {1, 1}, .target = {0, 0}});
            const auto dist = a_star_distance(layout, {1, 1}, {0, 0});

            CHECK(path.size() == 3);
            CHECK(dist == 2);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{1, 1});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{0, 0});
            // path could either go west-north or north-west
            CHECK((path[1] == fiction::layouts::layout_base::coordinate{1, 0} ||
                   path[1] == fiction::layouts::layout_base::coordinate{0, 1}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto path = a_star(layout, {.source = {0, 0}, .target = {0, 0}});
            const auto dist = a_star_distance(layout, {0, 0}, {0, 0});

            CHECK(path.size() == 1);
            CHECK(dist == 0);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{0, 0});
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{2, 2, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (1,1)")  // path of length 3
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {1, 1}});
                const auto dist = a_star_distance(layout, {0, 0}, {1, 1});

                CHECK(path.size() == 3);
                CHECK(dist == 2);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{1, 1});
                // path could either go east-south or south-east
                CHECK((path[1] == fiction::layouts::layout_base::coordinate{1, 0} ||
                       path[1] == fiction::layouts::layout_base::coordinate{0, 1}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto path = a_star(layout, {.source = {1, 1}, .target = {0, 0}});
                const auto dist = a_star_distance(layout, {1, 1}, {0, 0});

                CHECK(path.empty());
                CHECK(dist == std::numeric_limits<uint64_t>::max());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {0, 0}});
                const auto dist = a_star_distance(layout, {0, 0}, {0, 0});

                CHECK(path.size() == 1);
                CHECK(dist == 0);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{0, 0});
            }
        }
        SECTION("USE")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{2, 2, 1}, clocking::use()};

            SECTION("(0,0) to (0,1)")  // path of length 4
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {0, 1}});
                const auto dist = a_star_distance(layout, {0, 0}, {0, 1});

                CHECK(path.size() == 4);
                CHECK(dist == 3);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{0, 1});
                CHECK(path[1] == fiction::layouts::layout_base::coordinate{1, 0});
                CHECK(path[2] == fiction::layouts::layout_base::coordinate{1, 1});
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {0, 0}});
                const auto dist = a_star_distance(layout, {0, 0}, {0, 0});

                CHECK(path.size() == 1);
                CHECK(dist == 0);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{0, 0});
            }
        }
    }
}

TEST_CASE("A* on 4x4 layouts", "[A*]")
{
    using lyt = cartesian_layout;

    SECTION("coordinate paths")
    {
        const lyt layout{lyt::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = a_star(layout, {.source = {0, 0}, .target = {3, 3}});
            const auto dist = a_star_distance(layout, {0, 0}, {3, 3});

            CHECK(path.size() == 7);
            CHECK(dist == 6);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {3, 3}});
                const auto dist = a_star_distance(layout, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            }
        }
        SECTION("USE")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {3, 3}});
                const auto dist = a_star_distance(layout, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            }
        }
    }
}

TEST_CASE("A* on 4x4 layouts with varying cost functions", "[A*]")
{

    const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::use()};

    SECTION("Unit cost")
    {
        const auto path = a_star<cartesian_gate_layout>(layout, {.source = {0, 0}, .target = {3, 3}},
                                                        manhattan_distance_functor(), unit_cost_functor<>());

        CHECK(path.size() == 7);
        CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
        CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
    }
    SECTION("Random cost")
    {
        const auto path = a_star<cartesian_gate_layout>(layout, {.source = {0, 0}, .target = {3, 3}},
                                                        manhattan_distance_functor(), random_cost_functor<>());

        CHECK(path.size() == 7);
        CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
        CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
    }
}

TEST_CASE("A* on 10x10 layouts with varying distance functions", "[A*]")
{
    using lyt = cartesian_layout;

    SECTION("Manhattan distance")
    {
        SECTION("coordinate paths")
        {
            const lyt layout{lyt::extent{10, 10, 1}};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {9, 9}}, manhattan_distance_functor());

                CHECK(path.size() == 19);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const gate_layout<lyt> layout{gate_layout<lyt>::extent{10, 10, 1}, clocking::res()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = a_star<gate_layout<lyt>>(layout, {.source = {0, 0}, .target = {9, 9}},
                                                               manhattan_distance_functor());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                    CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
                }
            }
            SECTION("ESP")
            {
                const gate_layout<lyt> layout{gate_layout<lyt>::extent{10, 10, 1}, clocking::esr()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path =
                        a_star(layout, {.source = {0, 0}, .target = {9, 9}}, manhattan_distance_functor());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                    CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
                }
            }
        }
    }
    SECTION("Euclidean distance")
    {
        SECTION("coordinate paths")
        {
            const lyt layout{lyt::extent{10, 10, 1}};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {9, 9}}, euclidean_distance_functor());

                CHECK(path.size() == 19);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const gate_layout<lyt> layout{gate_layout<lyt>::extent{10, 10, 1}, clocking::res()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = a_star<gate_layout<lyt>>(layout, {.source = {0, 0}, .target = {9, 9}},
                                                               euclidean_distance_functor());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                    CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
                }
            }
            SECTION("ESP")
            {
                const gate_layout<lyt> layout{gate_layout<lyt>::extent{10, 10, 1}, clocking::esr()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path =
                        a_star(layout, {.source = {0, 0}, .target = {9, 9}}, euclidean_distance_functor());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                    CHECK(path.target() == fiction::layouts::layout_base::coordinate{9, 9});
                }
            }
        }
    }
}

TEST_CASE("A* on 4x4 gate-level layouts with coordinate obstruction", "[A*]")
{

    SECTION("coordinate paths")
    {
        const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            auto         obstr_lyt = static_cast<cartesian_layout>(layout);
            obstructions search_obstructions{};

            // create some PIs as obstruction
            search_obstructions.obstruct_coordinate({3, 0});
            search_obstructions.obstruct_coordinate({3, 1});
            search_obstructions.obstruct_coordinate({1, 2});
            search_obstructions.obstruct_coordinate({2, 2});
            // effectively blocking (3,2) as well

            const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                     unit_cost_functor<>(), {}, search_obstructions);  // only one path possible
            const auto dist = a_star_distance(obstr_lyt, {0, 0}, {3, 3}, search_obstructions);

            CHECK(path.size() == 7);
            CHECK(dist == 6);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            CHECK(path[1] == fiction::layouts::layout_base::coordinate{0, 1});
            CHECK(path[2] == fiction::layouts::layout_base::coordinate{0, 2});
            CHECK(path[3] == fiction::layouts::layout_base::coordinate{0, 3});
            CHECK(path[4] == fiction::layouts::layout_base::coordinate{1, 3});
            CHECK(path[5] == fiction::layouts::layout_base::coordinate{2, 3});
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
            {
                auto obstr_lyt = layout;

                // create some PIs as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});
                obstr_lyt.create_pi("obstruction", {3, 1});
                obstr_lyt.create_pi("obstruction", {1, 2});
                obstr_lyt.create_pi("obstruction", {2, 2});
                // effectively blocking (3,2) as well

                const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                         unit_cost_functor<>());  // only one path possible
                const auto dist = a_star_distance(obstr_lyt, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
                CHECK(path[1] == fiction::layouts::layout_base::coordinate{0, 1});
                CHECK(path[2] == fiction::layouts::layout_base::coordinate{0, 2});
                CHECK(path[3] == fiction::layouts::layout_base::coordinate{0, 3});
                CHECK(path[4] == fiction::layouts::layout_base::coordinate{1, 3});
                CHECK(path[5] == fiction::layouts::layout_base::coordinate{2, 3});
            }
        }
        SECTION("USE")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
            {
                auto obstr_lyt = layout;

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

                const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                         unit_cost_functor<>());  // only one path possible
                const auto dist = a_star_distance(obstr_lyt, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
                CHECK(path[1] == fiction::layouts::layout_base::coordinate{1, 0});
                CHECK(path[2] == fiction::layouts::layout_base::coordinate{1, 1});
                CHECK(path[3] == fiction::layouts::layout_base::coordinate{1, 2});
                CHECK(path[4] == fiction::layouts::layout_base::coordinate{2, 2});
                CHECK(path[5] == fiction::layouts::layout_base::coordinate{3, 2});
            }
        }
    }
}

TEST_CASE("A* on 4x4 gate-level layouts with connection obstruction", "[A*]")
{

    SECTION("coordinate paths")
    {
        const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
        {
            auto         obstr_lyt = static_cast<cartesian_layout>(layout);
            obstructions search_obstructions{};

            // create some connection obstructions
            search_obstructions.obstruct_connection({0, 0}, {1, 0});
            search_obstructions.obstruct_connection({0, 1}, {1, 1});
            search_obstructions.obstruct_connection({0, 2}, {1, 2});
            // leaving only one valid path via (0,4)

            const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                     unit_cost_functor<>(), {}, search_obstructions);  // only one path possible

            CHECK(path.size() == 7);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            CHECK(path[1] == fiction::layouts::layout_base::coordinate{0, 1});
            CHECK(path[2] == fiction::layouts::layout_base::coordinate{0, 2});
            CHECK(path[3] == fiction::layouts::layout_base::coordinate{0, 3});
            CHECK(path[4] == fiction::layouts::layout_base::coordinate{1, 3});
            CHECK(path[5] == fiction::layouts::layout_base::coordinate{2, 3});
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                const auto&  obstr_lyt = layout;
                obstructions search_obstructions{};

                // create some connection obstructions
                search_obstructions.obstruct_connection({0, 0}, {1, 0});
                search_obstructions.obstruct_connection({0, 1}, {1, 1});
                search_obstructions.obstruct_connection({0, 2}, {1, 2});
                // leaving only one valid path via (0,4)

                const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                         unit_cost_functor<>(), {}, search_obstructions);  // only one path possible

                CHECK(path.size() == 7);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
                CHECK(path[1] == fiction::layouts::layout_base::coordinate{0, 1});
                CHECK(path[2] == fiction::layouts::layout_base::coordinate{0, 2});
                CHECK(path[3] == fiction::layouts::layout_base::coordinate{0, 3});
                CHECK(path[4] == fiction::layouts::layout_base::coordinate{1, 3});
                CHECK(path[5] == fiction::layouts::layout_base::coordinate{2, 3});
            }
        }
        SECTION("USE")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                const auto&  obstr_lyt = layout;
                obstructions search_obstructions{};

                // create a PI as obstruction
                search_obstructions.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

                const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, manhattan_distance_functor(),
                                         unit_cost_functor<>(), {}, search_obstructions);  // only one path possible

                CHECK(path.size() == 7);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
                CHECK(path[1] == fiction::layouts::layout_base::coordinate{1, 0});
                CHECK(path[2] == fiction::layouts::layout_base::coordinate{1, 1});
                CHECK(path[3] == fiction::layouts::layout_base::coordinate{1, 2});
                CHECK(path[4] == fiction::layouts::layout_base::coordinate{2, 2});
                CHECK(path[5] == fiction::layouts::layout_base::coordinate{3, 2});
            }
        }
    }
}

TEST_CASE("A* with coordinate obstruction but crossings enabled", "[A*]")
{

    using dist = manhattan_distance_functor;
    using cost = unit_cost_functor<uint8_t>;

    // enable crossings
    const a_star_params params{true};

    SECTION("Single crossing")
    {
        SECTION("3x3")
        {
            SECTION("2DDWave")
            {
                const cartesian_gate_layout layout{cartesian_gate_layout::extent{3, 3, 2},
                                                   clocking::twoddwave()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    auto obstr_lyt = layout;

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {1, 0});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {1, 2});  // obstructs 1 coordinate

                    const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {2, 2}}, dist(), cost(), params);

                    CHECK(path == layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1}, {2, 2}}});
                }
            }
            SECTION("USE")
            {
                const cartesian_gate_layout layout{cartesian_gate_layout::extent{3, 3, 2},
                                                   clocking::use()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    auto obstr_lyt = layout;

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {2, 1});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {0, 1});  // obstructs 1 coordinate

                    const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {2, 2}}, dist(), cost(), params);

                    CHECK(path == layout_coordinate_path{{{0, 0}, {1, 0}, {1, 1, 1}, {1, 2}, {2, 2}}});
                }
            }
        }
    }
    SECTION("Two consecutive crossings")
    {
        SECTION("4x4")
        {
            SECTION("2DDWave")
            {
                const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 2},
                                                   clocking::twoddwave()};  // create a crossing layer

                SECTION("(0,0) to (3,3) with obstruction and crossings")  // 2 valid paths
                {
                    auto obstr_lyt = layout;

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w11 = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    const auto w12 = obstr_lyt.create_buf(w11, {1, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w12, "obstruction PO", {1, 3});  // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w21 = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    const auto w22 = obstr_lyt.create_buf(w21, {2, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w22, "obstruction PO", {2, 3});  // obstructs 1 coordinate

                    const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, dist(), cost(), params);

                    CHECK((path ==
                               layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {3, 1}, {3, 2}, {3, 3}}} ||
                           path ==
                               layout_coordinate_path{{{0, 0}, {0, 1}, {0, 2}, {1, 2, 1}, {2, 2, 1}, {3, 2}, {3, 3}}}));
                }
            }
        }
    }
    SECTION("Straight and corner crossing")
    {
        SECTION("4x3")
        {
            SECTION("2DDWave")
            {
                const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 3, 2},
                                                   clocking::twoddwave()};  // create a crossing layer

                SECTION("(0,0) to (3,2) with obstruction and crossings")  // 1 valid paths
                {
                    auto obstr_lyt = layout;

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w1  = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w1, "obstruction PO", {1, 2});   // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w2  = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w2, "obstruction PO", {3, 1});   // obstructs 1 coordinate

                    const auto path = a_star(obstr_lyt, {.source = {0, 0}, .target = {3, 2}}, dist(), cost(), params);

                    CHECK(path == layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {2, 2}, {3, 2}}});
                }
            }
        }
    }
}

TEST_CASE("A* path finding with the A* distance functor (don't do this!)", "[A*]")
{
    using lyt = cartesian_layout;

    SECTION("coordinate paths")
    {
        const lyt layout{lyt::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path =
                a_star(layout, {.source = {0, 0}, .target = {3, 3}}, a_star_distance_functor<lyt>(layout));

            CHECK(path.size() == 7);
            CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
            CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {3, 3}},
                                         a_star_distance_functor<gate_layout<lyt>>(layout));

                CHECK(path.size() == 7);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            }
        }
        SECTION("USE")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = a_star(layout, {.source = {0, 0}, .target = {3, 3}},
                                         a_star_distance_functor<gate_layout<lyt>>(layout));

                CHECK(path.size() == 7);
                CHECK(path.source() == fiction::layouts::layout_base::coordinate{0, 0});
                CHECK(path.target() == fiction::layouts::layout_base::coordinate{3, 3});
            }
        }
    }
}

TEST_CASE("A* invokes distance and cost functor overrides", "[A*]")
{
    /** @brief Heuristic that counts dynamic dispatch calls. */
    struct counting_distance : distance_functor<uint64_t>
    {
        /** @brief Builds a zero-cost base callback. */
        counting_distance() :
                distance_functor<uint64_t>{[](const layout_base::coordinate&, const layout_base::coordinate&)
                                           { return uint64_t{}; }}
        {}
        /** @brief Number of calls to the overridden heuristic. */
        mutable unsigned calls{};
        /** @brief Supplies the Manhattan heuristic. @param source Source. @param target Target. @return Distance. */
        uint64_t operator()(const layout_base::coordinate& source, const layout_base::coordinate& target) const override
        {
            ++calls;
            return manhattan_distance(source, target);
        }
    };
    /** @brief Edge cost that counts dynamic dispatch calls. */
    struct counting_cost : cost_functor<uint8_t>
    {
        /** @brief Builds a unit-cost base callback. */
        counting_cost() : cost_functor<uint8_t>{unit_cost<uint8_t>} {}
        /** @brief Number of calls to the overridden cost. */
        mutable unsigned calls{};
        /** @brief Supplies unit edge costs. @param source Source. @param target Target. @return Unit cost. */
        uint8_t operator()(const layout_base::coordinate& source, const layout_base::coordinate& target) const override
        {
            ++calls;
            return unit_cost(source, target);
        }
    };
    const cartesian_layout  layout{{3, 3}};
    const counting_distance heuristic{};
    const counting_cost     edge_cost{};
    const auto              path = a_star(layout, {.source = {0, 0}, .target = {2, 2}}, heuristic, edge_cost);
    CHECK(path.size() == 5);
    CHECK(heuristic.calls > 0);
    CHECK(edge_cost.calls > 0);
}

TEST_CASE("A* distance evaluations reject endpoints outside the layout", "[A*]")
{
    /** @brief Two-by-two geometry for endpoint validation. */
    const cartesian_layout layout{{2, 2}};
    /** @brief Borrowed-layout distance evaluator constructed before invocation. */
    const a_star_distance_functor<cartesian_layout> distance{layout};
    CHECK_THROWS_AS(a_star(layout, {.source = {2, 0}, .target = {0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS(a_star(layout, {.source = {0, 0}, .target = {-1, 0}}), std::invalid_argument);
    CHECK_THROWS_AS(a_star_distance(layout, {2, 0}, {0, 0}), std::invalid_argument);
    CHECK_THROWS_AS(a_star_distance(layout, {0, 0}, {-1, 0}), std::invalid_argument);
    CHECK_THROWS_AS(distance({2, 0}, {0, 0}), std::invalid_argument);
    CHECK_THROWS_AS(distance({0, 0}, {-1, 0}), std::invalid_argument);
}
