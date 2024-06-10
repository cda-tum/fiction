//
// Created by marcel on 12.07.23.
//

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/path_finding/distance_map.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <cstdint>

using namespace fiction;

template <typename Lyt, typename Dist>
Dist sum_distances(const Lyt& layout, const distance_functor<Lyt, Dist>& dist_func) noexcept
{
    Dist sum = 0;
    layout.foreach_coordinate(
        [&layout, &dist_func, &sum](const auto& c1) {
            layout.foreach_coordinate([&layout, &dist_func, &sum, &c1](const auto& c2)
                                      { sum += dist_func(layout, c1, c2); });
        });

    return sum;
}

TEST_CASE("Benchmark distance maps", "[benchmark]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using dist    = uint64_t;

    const clk_lyt layout{aspect_ratio<clk_lyt>{5, 5}, use_clocking<clk_lyt>()};

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
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using dist    = uint64_t;

    const clk_lyt layout{aspect_ratio<clk_lyt>{5, 5}, use_clocking<clk_lyt>()};

    BENCHMARK("smart_distance_cache (cold start)")
    {
        const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};

        return sum_distances(layout, dist_map_func);
    };

    // warm up the cache
    const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};
    sum_distances(layout, dist_map_func);

    BENCHMARK("smart_distance_cache (warm start)")
    {
        return sum_distances(layout, dist_map_func);
    };
}
