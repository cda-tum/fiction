//
// Created by marcel on 17.02.22.
//

#include "catch.hpp"

#include <fiction/algorithms/path_finding/a_star.hpp>
#include <fiction/algorithms/path_finding/cost.hpp>
#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("2x2 clocked layouts", "[A*]")
{
    using clk_lyt    = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using coord_path = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

        SECTION("(0,0) to (1,1)")  // path of length 3
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {1, 1}});
            const auto dist = a_star_distance(layout, {0, 0}, {1, 1});

            CHECK(path.size() == 3);
            CHECK(dist == 3);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{1, 1});
            // path could either go east-south or south-east
            CHECK((path[1] == coordinate<clk_lyt>{1, 0} || path[1] == coordinate<clk_lyt>{0, 1}));
        }
        SECTION("(1,1) to (0,0)")  // no valid paths
        {
            const auto path = a_star<coord_path>(layout, {{1, 1}, {0, 0}});
            const auto dist = a_star_distance(layout, {1, 1}, {0, 0});

            CHECK(path.empty());
            CHECK(dist == 0);
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {0, 0}});
            const auto dist = a_star_distance(layout, {0, 0}, {0, 0});

            CHECK(path.size() == 1);
            CHECK(dist == 1);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{0, 0});
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{1, 1}, use_clocking<clk_lyt>()};

        SECTION("(0,0) to (0,1)")  // path of length 4
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {0, 1}});
            const auto dist = a_star_distance(layout, {0, 0}, {0, 1});

            CHECK(path.size() == 4);
            CHECK(dist == 4);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{0, 1});
            CHECK(path[1] == coordinate<clk_lyt>{1, 0});
            CHECK(path[2] == coordinate<clk_lyt>{1, 1});
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {0, 0}});
            const auto dist = a_star_distance(layout, {0, 0}, {0, 0});

            CHECK(path.size() == 1);
            CHECK(dist == 1);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{0, 0});
        }
    }
}

TEST_CASE("4x4 clocked layouts", "[A*]")
{
    using clk_lyt    = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using coord_path = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {3, 3}});

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{3, 3});
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {3, 3}});

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{3, 3});
        }
    }
}

TEST_CASE("4x4 gate-level layouts with coordinate obstruction", "[A*]")
{
    using gate_lyt   = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using coord_path = layout_coordinate_path<gate_lyt>;

    SECTION("2DDWave")
    {
        gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create some PIs as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});
            obstr_lyt.create_pi("obstruction", {3, 1});
            obstr_lyt.create_pi("obstruction", {1, 2});
            obstr_lyt.create_pi("obstruction", {2, 2});
            // effectively blocking (3,2) as well

            const auto path = a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<gate_lyt>{0, 0});
            CHECK(path.target() == coordinate<gate_lyt>{3, 3});
            CHECK(path[1] == coordinate<gate_lyt>{0, 1});
            CHECK(path[2] == coordinate<gate_lyt>{0, 2});
            CHECK(path[3] == coordinate<gate_lyt>{0, 3});
            CHECK(path[4] == coordinate<gate_lyt>{1, 3});
            CHECK(path[5] == coordinate<gate_lyt>{2, 3});
        }
    }
    SECTION("USE")
    {
        gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

            const auto path = a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<gate_lyt>{0, 0});
            CHECK(path.target() == coordinate<gate_lyt>{3, 3});
            CHECK(path[1] == coordinate<gate_lyt>{1, 0});
            CHECK(path[2] == coordinate<gate_lyt>{1, 1});
            CHECK(path[3] == coordinate<gate_lyt>{1, 2});
            CHECK(path[4] == coordinate<gate_lyt>{2, 2});
            CHECK(path[5] == coordinate<gate_lyt>{3, 2});
        }
    }
}

TEST_CASE("4x4 gate-level layouts with connection obstruction", "[A*]")
{
    using gate_lyt   = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using coord_path = layout_coordinate_path<gate_lyt>;

    SECTION("2DDWave")
    {
        gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create some connection obstructions
            obstr_lyt.obstruct_connection({0, 0}, {1, 0});
            obstr_lyt.obstruct_connection({0, 1}, {1, 1});
            obstr_lyt.obstruct_connection({0, 2}, {1, 2});
            // leaving only one valid path via (0,4)

            const auto path = a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<gate_lyt>{0, 0});
            CHECK(path.target() == coordinate<gate_lyt>{3, 3});
            CHECK(path[1] == coordinate<gate_lyt>{0, 1});
            CHECK(path[2] == coordinate<gate_lyt>{0, 2});
            CHECK(path[3] == coordinate<gate_lyt>{0, 3});
            CHECK(path[4] == coordinate<gate_lyt>{1, 3});
            CHECK(path[5] == coordinate<gate_lyt>{2, 3});
        }
    }
    SECTION("USE")
    {
        gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

            const auto path = a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<gate_lyt>{0, 0});
            CHECK(path.target() == coordinate<gate_lyt>{3, 3});
            CHECK(path[1] == coordinate<gate_lyt>{1, 0});
            CHECK(path[2] == coordinate<gate_lyt>{1, 1});
            CHECK(path[3] == coordinate<gate_lyt>{1, 2});
            CHECK(path[4] == coordinate<gate_lyt>{2, 2});
            CHECK(path[5] == coordinate<gate_lyt>{3, 2});
        }
    }
}

TEST_CASE("10x10 clocked layouts with varying distance functions", "[A*]")
{
    using clk_lyt    = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using coord_path = layout_coordinate_path<clk_lyt>;

    SECTION("Manhattan distance")
    {
        SECTION("RES")
        {
            clk_lyt layout{{9, 9}, res_clocking<clk_lyt>()};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path =
                    a_star<coord_path, clk_lyt>(layout, {{0, 0}, {9, 9}}, manhattan_distance_functor<clk_lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{9, 9});
            }
        }
        SECTION("ESP")
        {
            clk_lyt layout{{9, 9}, esr_clocking<clk_lyt>()};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path = a_star<coord_path>(layout, {{0, 0}, {9, 9}}, manhattan_distance_functor<clk_lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{9, 9});
            }
        }
    }
    SECTION("Euclidean distance")
    {
        SECTION("RES")
        {
            clk_lyt layout{{9, 9}, res_clocking<clk_lyt>()};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path =
                    a_star<coord_path, clk_lyt>(layout, {{0, 0}, {9, 9}}, euclidean_distance_functor<clk_lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{9, 9});
            }
        }
        SECTION("ESP")
        {
            clk_lyt layout{{9, 9}, esr_clocking<clk_lyt>()};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path = a_star<coord_path>(layout, {{0, 0}, {9, 9}}, euclidean_distance_functor<clk_lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{9, 9});
            }
        }
    }
}

TEST_CASE("4x4 clocked layouts with varying cost functions", "[A*]")
{
    using clk_lyt    = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using coord_path = layout_coordinate_path<clk_lyt>;

    clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

    SECTION("Unit cost")
    {
        const auto path = a_star<coord_path, clk_lyt>(layout, {{0, 0}, {3, 3}}, manhattan_distance_functor<clk_lyt>(),
                                                      unit_cost_functor<clk_lyt>());

        CHECK(path.size() == 7);
        CHECK(path.source() == coordinate<clk_lyt>{0, 0});
        CHECK(path.target() == coordinate<clk_lyt>{3, 3});
    }
    SECTION("Random cost")
    {
        const auto path = a_star<coord_path, clk_lyt>(layout, {{0, 0}, {3, 3}}, manhattan_distance_functor<clk_lyt>(),
                                                      random_cost_functor<clk_lyt>());

        CHECK(path.size() == 7);
        CHECK(path.source() == coordinate<clk_lyt>{0, 0});
        CHECK(path.target() == coordinate<clk_lyt>{3, 3});
    }
}

TEST_CASE("A* path finding with the A* distance functor (don't do this!)", "[A*]")
{
    using clk_lyt    = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using coord_path = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {3, 3}}, a_star_distance_functor<clk_lyt>());

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{3, 3});
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = a_star<coord_path>(layout, {{0, 0}, {3, 3}}, a_star_distance_functor<clk_lyt>());

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<clk_lyt>{0, 0});
            CHECK(path.target() == coordinate<clk_lyt>{3, 3});
        }
    }
}
