//
// Created by marcel on 18.06.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/path_finding/distance_map.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>

using namespace fiction;

TEST_CASE("Distance map", "[distance-map]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{{4, 4}, twoddwave_clocking<clk_lyt>()};

        const auto dist_map      = initialize_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{{4, 4}, use_clocking<clk_lyt>()};

        const auto dist_map      = initialize_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{{4, 4}, res_clocking<clk_lyt>()};

        const auto dist_map      = initialize_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{{4, 4}, cfe_clocking<clk_lyt>()};

        const auto dist_map      = initialize_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
}

TEST_CASE("Sparse distance map", "[distance-map]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{{4, 4}, twoddwave_clocking<clk_lyt>()};

        const auto dist_map      = initialize_sparse_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{{4, 4}, use_clocking<clk_lyt>()};

        const auto dist_map      = initialize_sparse_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{{4, 4}, res_clocking<clk_lyt>()};

        const auto dist_map      = initialize_sparse_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{{4, 4}, cfe_clocking<clk_lyt>()};

        const auto dist_map      = initialize_sparse_distance_map(layout, a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
}

TEST_CASE("Smart distance cache functor", "[distance-map]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{{4, 4}, twoddwave_clocking<clk_lyt>()};

        const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{{4, 4}, use_clocking<clk_lyt>()};

        const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{{4, 4}, res_clocking<clk_lyt>()};

        const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{{4, 4}, cfe_clocking<clk_lyt>()};

        const auto dist_map_func = smart_distance_cache_functor<clk_lyt, dist>{layout, &a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate([&layout, &dist_map_func, &c1](const auto& c2)
                                          { CHECK(dist_map_func(layout, c1, c2) == a_star_distance(layout, c1, c2)); });
            });
    }
}
