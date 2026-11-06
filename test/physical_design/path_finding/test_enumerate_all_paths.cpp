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
 * @brief Tests for `fiction/physical_design/path_finding/enumerate_all_paths.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/layouts/obstructions.hpp>
#include <fiction/physical_design/path_finding/enumerate_all_paths.hpp>
#include <fiction/physical_design/routing_utils.hpp>

#include <algorithm>
#include <cstddef>
#include <set>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design;
using namespace fiction::physical_design::path_finding;

/**
 * @brief Checks complete path validity without relying on the enumeration order.
 * @tparam Lyt Layout type.
 * @param layout Layout in which paths are routed.
 * @param paths Enumerated paths from (0,0) to (3,3).
 */
template <typename Lyt>
void check_four_by_four_paths(const Lyt& layout, const path_collection& paths)
{
    const path_set unique{paths.cbegin(), paths.cend()};
    CHECK(unique.size() == paths.size());
    for (std::size_t index{}; index < paths.size(); ++index)
    {
        CAPTURE(index);
        const auto& path = paths[index];
        REQUIRE_FALSE(path.empty());
        CHECK(path.front() == layout_base::coordinate{0, 0});
        CHECK(path.back() == layout_base::coordinate{3, 3});
        const std::set<layout_base::coordinate> coordinates{path.cbegin(), path.cend()};
        CHECK(coordinates.size() == path.size());
        for (std::size_t step{}; step < path.size(); ++step)
        {
            CAPTURE(step);
            CHECK(layout.is_within_bounds(path[step]));
            if (step > 0)
            {
                CHECK(layout.is_adjacent_of(path[step - 1], path[step]));
                if constexpr (requires { layout.is_outgoing_clocked(path[step - 1], path[step]); })
                {
                    CHECK(layout.is_outgoing_clocked(path[step - 1], path[step]));
                }
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 2x2 layouts", "[enumerate-all-paths]")
{
    using lyt = cartesian_layout;

    SECTION("coordinate paths")
    {
        const lyt layout{lyt::extent{2, 2, 1}};

        SECTION("(0,0) to (1,1)")  // two valid paths
        {
            const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {1, 1}});

            CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(
                                       path_collection{layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1}}},
                                                       layout_coordinate_path{{{0, 0}, {1, 0}, {1, 1}}}}));
        }
        SECTION("(1,1) to (0,0)")  // two valid paths
        {
            const auto collection = enumerate_all_paths(layout, {.source = {1, 1}, .target = {0, 0}});

            CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(
                                       path_collection{layout_coordinate_path{{{1, 1}, {0, 1}, {0, 0}}},
                                                       layout_coordinate_path{{{1, 1}, {1, 0}, {0, 0}}}}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {0, 0}});

            CHECK_THAT(collection,
                       Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{{{0, 0}}}}));
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{2, 2, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (1,1)")  // two valid paths
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {1, 1}});

                CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(
                                           path_collection{layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1}}},
                                                           layout_coordinate_path{{{0, 0}, {1, 0}, {1, 1}}}}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto collection = enumerate_all_paths(layout, {.source = {1, 1}, .target = {0, 0}});

                CHECK(collection.empty());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {0, 0}});

                CHECK_THAT(collection,
                           Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{{{0, 0}}}}));
            }
        }
        SECTION("USE")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{2, 2, 1}, clocking::use()};

            SECTION("(0,0) to (0,1)")  // one valid path
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {0, 1}});

                CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(
                                           path_collection{layout_coordinate_path{{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}}));
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {0, 0}});

                CHECK_THAT(collection,
                           Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{{{0, 0}}}}));
            }
        }
    }
}

TEST_CASE("Enumerate cyclic clocking with a chord", "[enumerate-all-paths]")
{
    cartesian_gate_layout layout{cartesian_gate_layout::extent{3, 3, 1}, clocking::open()};
    layout.assign_clock_number({0, 0}, 0);
    layout.assign_clock_number({1, 0}, 1);
    layout.assign_clock_number({2, 0}, 2);
    layout.assign_clock_number({2, 1}, 3);
    layout.assign_clock_number({2, 2}, 0);
    layout.assign_clock_number({1, 2}, 1);
    layout.assign_clock_number({0, 2}, 2);
    layout.assign_clock_number({0, 1}, 3);
    layout.assign_clock_number({1, 1}, 0);

    const path_collection expected{layout_coordinate_path{{{0, 1}, {0, 0}, {1, 0}, {2, 0}, {2, 1}}},
                                   layout_coordinate_path{{{0, 1}, {1, 1}, {1, 0}, {2, 0}, {2, 1}}}};
    for (auto i = 0; i < 2; ++i)
    {
        const auto paths = enumerate_all_paths(layout, {.source = {0, 1}, .target = {2, 1}});
        CHECK_THAT(paths, Catch::Matchers::UnorderedRangeEquals(expected));
    }
}

TEST_CASE("Enumerate all paths on 4x4 layouts", "[enumerate-all-paths]")
{
    using lyt = cartesian_layout;

    SECTION("coordinate paths")
    {
        const lyt layout{lyt::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) without obstruction")  // 184 valid paths
        {
            const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {3, 3}});

            CHECK(collection.size() == 184);
            check_four_by_four_paths(layout, collection);
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {3, 3}});

                CHECK(collection.size() == 20);
                check_four_by_four_paths(layout, collection);
            }
        }
        SECTION("USE")
        {
            const gate_layout<lyt> layout{gate_layout<lyt>::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = enumerate_all_paths(layout, {.source = {0, 0}, .target = {3, 3}});

                CHECK(collection.size() == 4);
                check_four_by_four_paths(layout, collection);
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with coordinate obstruction", "[enumerate-all-paths]")
{

    SECTION("coordinate paths")
    {
        const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // 108 valid paths
        {
            auto         obstr_lyt = static_cast<cartesian_layout>(layout);
            obstructions search_obstructions{};

            // mark coordinate as obstructed
            search_obstructions.obstruct_coordinate({3, 0});  // blocks 75 paths

            const auto collection =
                enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, {}, search_obstructions);

            CHECK(collection.size() == 108);
            check_four_by_four_paths(obstr_lyt, collection);
            for (const auto& path : collection)
            {
                CHECK(std::ranges::find(path, layout_base::coordinate{3, 0}) == path.cend());
            }
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // 19 valid paths
            {
                auto obstr_lyt = layout;

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 1 path

                const auto collection = enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}});

                CHECK(collection.size() == 19);
                check_four_by_four_paths(obstr_lyt, collection);
                for (const auto& path : collection)
                {
                    CHECK(std::ranges::find(path, layout_base::coordinate{3, 0}) == path.cend());
                }
            }
        }
        SECTION("USE")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // 1 valid path
            {
                auto obstr_lyt = layout;

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

                const auto collection = enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}});

                CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{
                                           {{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {3, 3}}}}));
            }
        }
    }
}

TEST_CASE("Enumerate all paths with coordinate obstruction but crossings enabled", "[enumerate-all-paths]")
{

    // enable crossings
    const enumerate_all_paths_params params{.crossings = true};

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

                    const auto collection =
                        enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {2, 2}}, params);

                    CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(path_collection{
                                               layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1}, {2, 2}}}}));
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

                    const auto collection =
                        enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {2, 2}}, params);

                    CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(path_collection{
                                               layout_coordinate_path{{{0, 0}, {1, 0}, {1, 1, 1}, {1, 2}, {2, 2}}}}));
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

                    const auto collection =
                        enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, params);

                    CHECK_THAT(
                        collection,
                        Catch::Matchers::UnorderedRangeEquals(path_collection{
                            layout_coordinate_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {3, 1}, {3, 2}, {3, 3}}},
                            layout_coordinate_path{{{0, 0}, {0, 1}, {0, 2}, {1, 2, 1}, {2, 2, 1}, {3, 2}, {3, 3}}}}));
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

                    const auto collection =
                        enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 2}}, params);

                    CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{
                                               {{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {2, 2}, {3, 2}}}}));
                }
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with connection obstruction", "[enumerate-all-paths]")
{

    SECTION("coordinate paths")
    {
        const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}};

        SECTION("(0,0) to (3,3) with connection obstruction")  // 108 valid paths
        {
            auto         obstr_lyt = static_cast<cartesian_layout>(layout);
            obstructions search_obstructions{};

            // create a connection obstruction
            search_obstructions.obstruct_connection({2, 0}, {3, 0});  // blocks 75 paths

            const auto collection =
                enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, {}, search_obstructions);

            CHECK(collection.size() == 108);
            check_four_by_four_paths(obstr_lyt, collection);
            for (const auto& path : collection)
            {
                CHECK(std::adjacent_find(
                          path.cbegin(), path.cend(), [](const auto from, const auto to)
                          { return from == layout_base::coordinate{2, 0} && to == layout_base::coordinate{3, 0}; }) ==
                      path.cend());
            }
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::twoddwave()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // 19 valid paths
            {
                const auto&  obstr_lyt = layout;
                obstructions search_obstructions{};

                // create a connection obstruction
                search_obstructions.obstruct_connection({2, 0}, {3, 0});  // blocks 1 path

                const auto collection =
                    enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, {}, search_obstructions);

                CHECK(collection.size() == 19);
                check_four_by_four_paths(obstr_lyt, collection);
                for (const auto& path : collection)
                {
                    CHECK(std::adjacent_find(path.cbegin(), path.cend(),
                                             [](const auto from, const auto to)
                                             {
                                                 return from == layout_base::coordinate{2, 0} &&
                                                        to == layout_base::coordinate{3, 0};
                                             }) == path.cend());
                }
            }
        }
        SECTION("USE")
        {
            const cartesian_gate_layout layout{cartesian_gate_layout::extent{4, 4, 1}, clocking::use()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // 1 valid path
            {
                const auto&  obstr_lyt = layout;
                obstructions search_obstructions{};

                // create a PI as obstruction
                search_obstructions.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

                const auto collection =
                    enumerate_all_paths(obstr_lyt, {.source = {0, 0}, .target = {3, 3}}, {}, search_obstructions);

                CHECK_THAT(collection, Catch::Matchers::UnorderedRangeEquals(path_collection{layout_coordinate_path{
                                           {{0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2}, {3, 2}, {3, 3}}}}));
            }
        }
    }
}

TEST_CASE("Path enumeration rejects endpoints outside the layout", "[enumerate-all-paths]")
{
    /** @brief Two-by-two geometry for endpoint validation. */
    const cartesian_layout layout{{2, 2}};
    CHECK_THROWS_AS(enumerate_all_paths(layout, {.source = {2, 0}, .target = {0, 0}}), std::invalid_argument);
    CHECK_THROWS_AS(enumerate_all_paths(layout, {.source = {0, 0}, .target = {-1, 0}}), std::invalid_argument);
}
