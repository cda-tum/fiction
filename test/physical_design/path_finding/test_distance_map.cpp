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
 * @brief Tests for `fiction/physical_design/path_finding/distance_map.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_layout.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/path_finding/distance_map.hpp>

#include <cstdint>
#include <cstdlib>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design::path_finding;

TEST_CASE("Distance map", "[distance-map]")
{
    /** @brief Distance representation shared by the map and lookup. */
    using dist = uint64_t;
    /** @brief Each scheme exercises a distinct directed routing graph. */
    const auto clock_scheme = GENERATE(clocking::twoddwave(), clocking::use(), clocking::res(), clocking::cfe());
    CAPTURE(clock_scheme.name());

    /** @brief Five-by-five clocked layout with one layer. */
    const cartesian_gate_layout layout{cartesian_gate_layout::extent{5, 5, 1}, clock_scheme};

    /** @brief Precomputed distances for every ordered coordinate pair. */
    const auto dist_map = initialize_distance_map(layout, a_star_distance_functor<cartesian_gate_layout>{layout});
    /** @brief Dense lookup over the precomputed coordinate indices. */
    const auto dist_map_func = distance_map_functor<dist>{layout.get_extent(), dist_map};

    REQUIRE(dist_map.size() == layout.area() * layout.layers());
    layout.foreach_coordinate(
        [&layout, &dist_map, &dist_map_func](const auto& c1, const auto src)
        {
            REQUIRE(dist_map[src].size() == layout.area() * layout.layers());
            layout.foreach_coordinate(
                [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const auto tgt)
                {
                    CAPTURE(c1, c2);
                    CHECK(dist_map[src][tgt] == a_star_distance(layout, c1, c2));
                    CHECK(dist_map[src][tgt] == dist_map_func(c1, c2));
                });
        });
}

TEST_CASE("Distance maps index every layer and reject coordinates outside the frame", "[distance-map]")
{
    /** Small geometry with two rows and two layers. */
    const cartesian_layout layout{{2, 2, 2}};
    /** Coordinate distances include layer displacement. */
    const distance_functor<uint64_t> distance{
        [](const auto& source, const auto& target)
        { return manhattan_distance(source, target) + static_cast<uint64_t>(std::abs(source.z - target.z)); }};
    /** Precompute every coordinate pair in the frame. */
    const auto distances = initialize_distance_map(layout, distance);
    /** Lookup uses the same coordinate indices as initialization. */
    const distance_map_functor<uint64_t> lookup{layout.get_extent(), distances};

    REQUIRE(distances.size() == layout.area() * layout.layers());
    layout.foreach_coordinate(
        [&](const auto& source, const auto source_index)
        {
            REQUIRE(distances[source_index].size() == layout.area() * layout.layers());
            layout.foreach_coordinate(
                [&](const auto& target, const auto target_index)
                {
                    CHECK(distances[source_index][target_index] == distance(source, target));
                    CHECK(lookup(source, target) == distance(source, target));
                });
        });
    CHECK_THROWS_AS(lookup({2, 0, 0}, {0, 0, 0}), std::out_of_range);
    CHECK_THROWS_AS(lookup({0, -1, 1}, {0, 0, 0}), std::out_of_range);
    CHECK_THROWS_AS(lookup({0, 0, 2}, {0, 0, 0}), std::out_of_range);
}

TEST_CASE("Sparse distance map", "[distance-map]")
{
    /** @brief Distance representation shared by the map and lookup. */
    using dist = uint64_t;
    /** @brief Each scheme exercises a distinct directed routing graph. */
    const auto clock_scheme = GENERATE(clocking::twoddwave(), clocking::use(), clocking::res(), clocking::cfe());
    CAPTURE(clock_scheme.name());

    /** @brief Five-by-five clocked layout with one layer. */
    const cartesian_gate_layout layout{cartesian_gate_layout::extent{5, 5, 1}, clock_scheme};

    /** @brief Precomputed distances for every ordered coordinate pair. */
    const auto dist_map =
        initialize_sparse_distance_map(layout, a_star_distance_functor<cartesian_gate_layout>{layout});
    /** @brief Sparse lookup over the precomputed coordinate pairs. */
    const auto dist_map_func = sparse_distance_map_functor<dist>{dist_map};

    layout.foreach_coordinate(
        [&layout, &dist_map, &dist_map_func](const auto& c1)
        {
            layout.foreach_coordinate(
                [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                {
                    CAPTURE(c1, c2);
                    CHECK(dist_map.at({c1, c2}) == a_star_distance(layout, c1, c2));
                    CHECK(dist_map.at({c1, c2}) == dist_map_func(c1, c2));
                });
        });
}

TEST_CASE("Smart distance cache functor", "[distance-map]")
{
    /** @brief Distance representation shared by the map and lookup. */
    using dist = uint64_t;
    /** @brief Each scheme exercises a distinct directed routing graph. */
    const auto clock_scheme = GENERATE(clocking::twoddwave(), clocking::use(), clocking::res(), clocking::cfe());
    CAPTURE(clock_scheme.name());

    /** @brief Five-by-five clocked layout with one layer. */
    const cartesian_gate_layout layout{cartesian_gate_layout::extent{5, 5, 1}, clock_scheme};

    /** @brief Cache shared by the cold and warm traversals. */
    const auto dist_map_func =
        smart_distance_cache_functor<dist>{a_star_distance_functor<cartesian_gate_layout>{layout}};

    /** @brief Two passes query every miss and then every cached value. */
    for (auto pass = 0; pass < 2; ++pass)
    {
        CAPTURE(pass);
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CAPTURE(c1, c2);
                        CHECK(dist_map_func(c1, c2) == a_star_distance(layout, c1, c2));
                    });
            });
    }
}

TEST_CASE("Smart distance cache computes each directed pair once", "[distance-map]")
{
    /** @brief Counts evaluations of the supplied distance callback. */
    unsigned calls{};
    /** @brief Asymmetric values distinguish the two directed coordinate pairs. */
    const smart_distance_cache_functor<uint64_t> distance{[&](const auto& source, const auto& target)
                                                          {
                                                              ++calls;
                                                              return static_cast<uint64_t>(10 * source.x + target.x);
                                                          }};

    CHECK(distance({1, 0}, {2, 0}) == 12);
    CHECK(calls == 1);
    CHECK(distance({1, 0}, {2, 0}) == 12);
    CHECK(calls == 1);
    CHECK(distance({2, 0}, {1, 0}) == 21);
    CHECK(calls == 2);
    CHECK(distance({1, 0}, {2, 0}) == 12);
    CHECK(distance({2, 0}, {1, 0}) == 21);
    CHECK(calls == 2);
}
