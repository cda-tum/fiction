/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

//
// Created by marcel on 18.06.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/path_finding/distance_map.hpp>

using namespace fiction;

TEST_CASE("Distance map", "[distance-map]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::twoddwave<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::use<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::res<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::cfe<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1, const unsigned src)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1, src](const auto& c2, const unsigned tgt)
                    {
                        CHECK(dist_map[src][tgt] == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map[src][tgt] == dist_map_func(layout, c1, c2));
                    });
            });
    }
}

TEST_CASE("Sparse distance map", "[distance-map]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::twoddwave<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_sparse_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::use<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_sparse_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::res<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_sparse_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::cfe<clk_lyt>()};

        const auto dist_map = physical_design::path_finding::initialize_sparse_distance_map(
            layout, physical_design::path_finding::a_star_distance_functor<clk_lyt, dist>{});
        const auto dist_map_func = physical_design::path_finding::sparse_distance_map_functor<clk_lyt, dist>{dist_map};

        layout.foreach_coordinate(
            [&layout, &dist_map, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map.at({c1, c2}) == physical_design::path_finding::a_star_distance(layout, c1, c2));
                        CHECK(dist_map.at({c1, c2}) == dist_map_func(layout, c1, c2));
                    });
            });
    }
}

TEST_CASE("Smart distance cache functor", "[distance-map]")
{
    using clk_lyt = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;
    using dist    = uint64_t;

    SECTION("2DDWave clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::twoddwave<clk_lyt>()};

        const auto dist_map_func = physical_design::path_finding::smart_distance_cache_functor<clk_lyt, dist>{
            layout, &physical_design::path_finding::a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });
    }
    SECTION("USE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::use<clk_lyt>()};

        const auto dist_map_func = physical_design::path_finding::smart_distance_cache_functor<clk_lyt, dist>{
            layout, &physical_design::path_finding::a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });
    }
    SECTION("RES clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::res<clk_lyt>()};

        const auto dist_map_func = physical_design::path_finding::smart_distance_cache_functor<clk_lyt, dist>{
            layout, &physical_design::path_finding::a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });
    }
    SECTION("CFE clocking")
    {
        const clk_lyt layout{aspect_ratio<clk_lyt>{4, 4}, layouts::clocking::cfe<clk_lyt>()};

        const auto dist_map_func = physical_design::path_finding::smart_distance_cache_functor<clk_lyt, dist>{
            layout, &physical_design::path_finding::a_star_distance<clk_lyt, dist>};

        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });

        // check that the cached distances are correct
        layout.foreach_coordinate(
            [&layout, &dist_map_func](const auto& c1)
            {
                layout.foreach_coordinate(
                    [&layout, &dist_map_func, &c1](const auto& c2)
                    {
                        CHECK(dist_map_func(layout, c1, c2) ==
                              physical_design::path_finding::a_star_distance(layout, c1, c2));
                    });
            });
    }
}
