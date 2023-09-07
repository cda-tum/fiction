//
// Created by marcel on 19.05.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/path_finding/jump_point_search.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/utils/routing_utils.hpp>

#include <cmath>

using namespace fiction;

TEST_CASE("JPS on 2x2 layouts", "[JPS]")
{
    using lyt        = cartesian_layout<offset::ucoord_t>;
    using coord_path = layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{1, 1}};

        SECTION("(0,0) to (1,1)")  // path of length 3
        {
            const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {1, 1}});

            CHECK(path.size() == 3);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{1, 1});
            // path could either go east-south or south-east
            CHECK((path[1] == coordinate<lyt>{1, 0} || path[1] == coordinate<lyt>{0, 1}));
        }
        SECTION("(1,1) to (0,0)")  // path of length 3
        {
            const auto path = jump_point_search<coord_path>(layout, {{1, 1}, {0, 0}});

            CHECK(path.size() == 3);
            CHECK(path.source() == coordinate<lyt>{1, 1});
            CHECK(path.target() == coordinate<lyt>{0, 0});
            // path could either go west-north or north-west
            CHECK((path[1] == coordinate<lyt>{1, 0} || path[1] == coordinate<lyt>{0, 1}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {0, 0}});

            CHECK(path.size() == 1);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{0, 0});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (1,1)")  // path of length 3
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {1, 1}});

                CHECK(path.size() == 3);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{1, 1});
                // path could either go east-south or south-east
                CHECK((path[1] == coordinate<clk_lyt>{1, 0} || path[1] == coordinate<clk_lyt>{0, 1}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto path = jump_point_search<coord_path>(layout, {{1, 1}, {0, 0}});

                CHECK(path.empty());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {0, 0}});

                CHECK(path.size() == 1);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 0});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{1, 1}, use_clocking<clk_lyt>()};

            SECTION("(0,0) to (0,1)")  // path of length 4
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {0, 1}});

                CHECK(path.size() == 4);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 1});
                CHECK(path[1] == coordinate<clk_lyt>{1, 0});
                CHECK(path[2] == coordinate<clk_lyt>{1, 1});
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {0, 0}});

                CHECK(path.size() == 1);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 0});
            }
        }
    }
}

TEST_CASE("JPS on 4x4 layouts", "[JPS]")
{
    using lyt        = cartesian_layout<offset::ucoord_t>;
    using coord_path = layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {3, 3}});

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{3, 3});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {3, 3}});

                CHECK(path.size() == 7);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = jump_point_search<coord_path>(layout, {{0, 0}, {3, 3}});

                CHECK(path.size() == 7);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
    }
}

TEST_CASE("JPS on 4x4 gate-level layouts with coordinate obstruction", "[JPS]")
{
    using gate_lyt   = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using coord_path = layout_coordinate_path<gate_lyt>;

    SECTION("2DDWave")
    {
        const gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create some PIs as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});
            obstr_lyt.create_pi("obstruction", {3, 1});
            obstr_lyt.create_pi("obstruction", {1, 2});
            obstr_lyt.create_pi("obstruction", {2, 2});
            // effectively blocking (3,2) as well

            const auto path = jump_point_search<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
        const gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

            const auto path = jump_point_search<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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

TEST_CASE("JPS on 4x4 gate-level layouts with connection obstruction", "[JPS]")
{
    using gate_lyt   = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using coord_path = layout_coordinate_path<gate_lyt>;

    SECTION("coordinate paths")
    {
        const gate_lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
        {
            obstruction_layout obstr_lyt{static_cast<cartesian_layout<>>(layout)};

            // create some connection obstructions
            obstr_lyt.obstruct_connection({0, 0}, {1, 0});
            obstr_lyt.obstruct_connection({0, 1}, {1, 1});
            obstr_lyt.obstruct_connection({0, 2}, {1, 2});
            // leaving only one valid path via (0,4)

            const auto path = jump_point_search<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                obstruction_layout obstr_lyt{layout};

                // create some connection obstructions
                obstr_lyt.obstruct_connection({0, 0}, {1, 0});
                obstr_lyt.obstruct_connection({0, 1}, {1, 1});
                obstr_lyt.obstruct_connection({0, 2}, {1, 2});
                // leaving only one valid path via (0,4)

                const auto path = jump_point_search<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
            const gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

                const auto path = jump_point_search<coord_path>(obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
}

TEST_CASE("JPS on 10x10 layouts with varying distance functions", "[JPS]")
{
    using lyt        = cartesian_layout<offset::ucoord_t>;
    using clk_lyt    = clocked_layout<lyt>;
    using coord_path = layout_coordinate_path<lyt>;

    SECTION("Manhattan distance")
    {
        SECTION("coordinate paths")
        {
            const lyt layout{{9, 9}};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path =
                    jump_point_search<coord_path>(layout, {{0, 0}, {9, 9}}, manhattan_distance_functor<lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<lyt>{0, 0});
                CHECK(path.target() == coordinate<lyt>{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const clk_lyt layout{{9, 9}, res_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = jump_point_search<coord_path, clk_lyt>(layout, {{0, 0}, {9, 9}},
                                                                             manhattan_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
            SECTION("ESP")
            {
                const clk_lyt layout{{9, 9}, esr_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path =
                        jump_point_search<coord_path>(layout, {{0, 0}, {9, 9}}, manhattan_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
        }
    }
    SECTION("Euclidean distance")
    {
        SECTION("coordinate paths")
        {
            const lyt layout{{9, 9}};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path =
                    jump_point_search<coord_path>(layout, {{0, 0}, {9, 9}}, euclidean_distance_functor<lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<lyt>{0, 0});
                CHECK(path.target() == coordinate<lyt>{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const clk_lyt layout{{9, 9}, res_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = jump_point_search<coord_path, clk_lyt>(layout, {{0, 0}, {9, 9}},
                                                                             euclidean_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
            SECTION("ESP")
            {
                const clk_lyt layout{{9, 9}, esr_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path =
                        jump_point_search<coord_path>(layout, {{0, 0}, {9, 9}}, euclidean_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
        }
    }
}
