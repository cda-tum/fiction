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
 * @brief Catch2 benchmarks for `fiction/physical_design/path_finding/distance_map.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/path_finding/distance_map.hpp>

#include <cstdint>
#include <stdexcept>

using namespace fiction;
using namespace fiction::layouts;
using namespace fiction::physical_design::path_finding;

/**
 * @brief Sums distances between every pair of coordinates in the layout frame.
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 * @param layout Layout whose coordinates are measured.
 * @param dist_func Distance calculation.
 * @return Sum of the measured distances.
 * @throws std::exception Propagates exceptions from the distance calculation.
 */
template <typename Lyt, typename Dist>
Dist sum_distances(const Lyt& layout, const distance_functor<Lyt, Dist>& dist_func)
{
    Dist sum = 0;
    layout.foreach_coordinate(
        [&layout, &dist_func, &sum](const auto& c1)
        {
            layout.foreach_coordinate([&layout, &dist_func, &sum, &c1](const auto& c2)
                                      { sum += dist_func(layout, c1, c2); });
        });

    return sum;
}

TEST_CASE("Benchmark distance maps", "[benchmark]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;
    using dist    = uint64_t;

    /** @brief Six-by-six frame used by the distance measurements. */
    const clk_lyt layout{clk_lyt::extent{6, 6}, clocking::use()};
    CHECK(layout.area() == 36);

    BENCHMARK("without distance maps")
    {
        return sum_distances(layout, a_star_distance_functor<clk_lyt, dist>{});
    };

    const auto dist_map      = initialize_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
    const auto dist_map_func = distance_map_functor<clk_lyt, dist>{dist_map};

    BENCHMARK("distance_map")
    {
        return sum_distances(layout, dist_map_func);
    };

    const auto sparse_dist_map      = initialize_sparse_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
    const auto sparse_dist_map_func = sparse_distance_map_functor<clk_lyt, dist>{sparse_dist_map};

    BENCHMARK("sparse_distance_map")
    {
        return sum_distances(layout, sparse_dist_map_func);
    };
}

TEST_CASE("Benchmark smart distance cache", "[benchmark]")
{
    using clk_lyt = gate_level_layout<cartesian_layout>;
    using dist    = uint64_t;

    /** @brief Six-by-six frame used by the distance measurements. */
    const clk_lyt layout{clk_lyt::extent{6, 6}, clocking::use()};
    CHECK(layout.area() == 36);

    BENCHMARK("smart_distance_cache (cold start)")
    {
        const auto dist_map_func =
            smart_distance_cache_functor<clk_lyt, dist>{layout, a_star_distance_functor<clk_lyt, dist>{}};

        return sum_distances(layout, dist_map_func);
    };

    // warm up the cache
    const auto dist_map_func =
        smart_distance_cache_functor<clk_lyt, dist>{layout, a_star_distance_functor<clk_lyt, dist>{}};
    sum_distances(layout, dist_map_func);

    BENCHMARK("smart_distance_cache (warm start)")
    {
        return sum_distances(layout, dist_map_func);
    };
}

TEST_CASE("Distance summation propagates distance-functor exceptions", "[distance-map]")
{
    /** @brief Single-coordinate frame whose distance call must propagate exceptions. */
    const cartesian_layout layout{{1, 1}};
    /** @brief Distance function that rejects every calculation. */
    const distance_functor<cartesian_layout, uint64_t> throwing_distance{
        [](const auto&, const auto&, const auto&) -> uint64_t { throw std::invalid_argument{"Distance unavailable"}; }};
    CHECK_THROWS_AS(sum_distances(layout, throwing_distance), std::invalid_argument);
}
