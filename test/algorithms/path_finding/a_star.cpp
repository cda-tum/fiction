//
// Created by marcel on 17.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>
#include <fiction/physical_design/path_finding/a_star.hpp>
#include <fiction/physical_design/path_finding/cost.hpp>
#include <fiction/physical_design/path_finding/distance.hpp>
#include <fiction/physical_design/utils/routing_utils.hpp>

#include <cmath>
#include <cstdint>
#include <limits>

using namespace fiction;

TEST_CASE("A* on 2x2 layouts", "[A*]")
{
    using lyt        = layouts::cartesian_layout<layouts::coords::offset>;
    using coord_path = physical_design::utils::layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{1, 1}};

        SECTION("(0,0) to (1,1)")  // path of length 3
        {
            const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {1, 1}});
            const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {1, 1});

            CHECK(path.size() == 3);
            CHECK(dist == 2);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{1, 1});
            // path could either go east-south or south-east
            CHECK((path[1] == coordinate<lyt>{1, 0} || path[1] == coordinate<lyt>{0, 1}));
        }
        SECTION("(1,1) to (0,0)")  // path of length 3
        {
            const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{1, 1}, {0, 0}});
            const auto dist = physical_design::path_finding::a_star_distance(layout, {1, 1}, {0, 0});

            CHECK(path.size() == 3);
            CHECK(dist == 2);
            CHECK(path.source() == coordinate<lyt>{1, 1});
            CHECK(path.target() == coordinate<lyt>{0, 0});
            // path could either go west-north or north-west
            CHECK((path[1] == coordinate<lyt>{1, 0} || path[1] == coordinate<lyt>{0, 1}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {0, 0}});
            const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {0, 0});

            CHECK(path.size() == 1);
            CHECK(dist == 0);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{0, 0});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = layouts::clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{1, 1}, layouts::twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (1,1)")  // path of length 3
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {1, 1}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {1, 1});

                CHECK(path.size() == 3);
                CHECK(dist == 2);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{1, 1});
                // path could either go east-south or south-east
                CHECK((path[1] == coordinate<clk_lyt>{1, 0} || path[1] == coordinate<clk_lyt>{0, 1}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{1, 1}, {0, 0}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {1, 1}, {0, 0});

                CHECK(path.empty());
                CHECK(dist == std::numeric_limits<uint64_t>::max());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {0, 0}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {0, 0});

                CHECK(path.size() == 1);
                CHECK(dist == 0);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 0});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{1, 1}, layouts::use_clocking<clk_lyt>()};

            SECTION("(0,0) to (0,1)")  // path of length 4
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {0, 1}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {0, 1});

                CHECK(path.size() == 4);
                CHECK(dist == 3);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 1});
                CHECK(path[1] == coordinate<clk_lyt>{1, 0});
                CHECK(path[2] == coordinate<clk_lyt>{1, 1});
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {0, 0}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {0, 0});

                CHECK(path.size() == 1);
                CHECK(dist == 0);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{0, 0});
            }
        }
    }
}

TEST_CASE("A* on 4x4 layouts", "[A*]")
{
    using lyt        = layouts::cartesian_layout<layouts::coords::offset>;
    using coord_path = physical_design::utils::layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {3, 3}});
            const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {3, 3});

            CHECK(path.size() == 7);
            CHECK(dist == 6);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{3, 3});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = layouts::clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{3, 3}, layouts::twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {3, 3}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{3, 3}, layouts::use_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(layout, {{0, 0}, {3, 3}});
                const auto dist = physical_design::path_finding::a_star_distance(layout, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
    }
}

TEST_CASE("A* on 4x4 gate-level layouts with coordinate obstruction", "[A*]")
{
    using gate_lyt =
        layouts::gate_level_layout<layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
    using obst_lyt   = layouts::obstruction_layout<gate_lyt>;
    using coord_path = physical_design::utils::layout_coordinate_path<obst_lyt>;

    SECTION("coordinate paths")
    {
        const gate_lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
        {
            layouts::obstruction_layout obstr_lyt{static_cast<layouts::cartesian_layout<>>(layout)};

            // create some PIs as obstruction
            obstr_lyt.obstruct_coordinate({3, 0});
            obstr_lyt.obstruct_coordinate({3, 1});
            obstr_lyt.obstruct_coordinate({1, 2});
            obstr_lyt.obstruct_coordinate({2, 2});
            // effectively blocking (3,2) as well

            const auto path = physical_design::path_finding::a_star<coord_path>(
                obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible
            const auto dist = physical_design::path_finding::a_star_distance(obstr_lyt, {0, 0}, {3, 3});

            CHECK(path.size() == 7);
            CHECK(dist == 6);
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
            const gate_lyt layout{{3, 3}, layouts::twoddwave_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
            {
                layouts::obstruction_layout obstr_lyt{layout};

                // create some PIs as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});
                obstr_lyt.create_pi("obstruction", {3, 1});
                obstr_lyt.create_pi("obstruction", {1, 2});
                obstr_lyt.create_pi("obstruction", {2, 2});
                // effectively blocking (3,2) as well

                const auto path = physical_design::path_finding::a_star<coord_path>(
                    obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible
                const auto dist = physical_design::path_finding::a_star_distance(obstr_lyt, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
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
            const gate_lyt layout{{3, 3}, layouts::use_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // path of length 7
            {
                layouts::obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

                const auto path = physical_design::path_finding::a_star<coord_path>(
                    obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible
                const auto dist = physical_design::path_finding::a_star_distance(obstr_lyt, {0, 0}, {3, 3});

                CHECK(path.size() == 7);
                CHECK(dist == 6);
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

TEST_CASE("A* with coordinate obstruction but crossings enabled", "[A*]")
{
    using gate_lyt =
        layouts::gate_level_layout<layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
    using obst_lyt   = layouts::obstruction_layout<gate_lyt>;
    using coord_path = physical_design::utils::layout_coordinate_path<obst_lyt>;

    using dist =
        physical_design::path_finding::manhattan_distance_functor<layouts::obstruction_layout<gate_lyt>, uint64_t>;
    using cost = physical_design::path_finding::unit_cost_functor<layouts::obstruction_layout<gate_lyt>, uint8_t>;

    // enable crossings
    const physical_design::path_finding::a_star_params params{true};

    SECTION("Single crossing")
    {
        SECTION("3x3")
        {
            SECTION("2DDWave")
            {
                const gate_lyt layout{{2, 2, 1}, layouts::twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    layouts::obstruction_layout obstr_lyt{layout};

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {1, 0});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {1, 2});  // obstructs 1 coordinate

                    const auto path = physical_design::path_finding::a_star<coord_path>(obstr_lyt, {{0, 0}, {2, 2}},
                                                                                        dist(), cost(), params);

                    CHECK(path == coord_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1}, {2, 2}}});
                }
            }
            SECTION("USE")
            {
                const gate_lyt layout{{2, 2, 1}, layouts::use_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    layouts::obstruction_layout obstr_lyt{layout};

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {2, 1});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {0, 1});  // obstructs 1 coordinate

                    const auto path = physical_design::path_finding::a_star<coord_path>(obstr_lyt, {{0, 0}, {2, 2}},
                                                                                        dist(), cost(), params);

                    CHECK(path == coord_path{{{0, 0}, {1, 0}, {1, 1, 1}, {1, 2}, {2, 2}}});
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
                const gate_lyt layout{{3, 3, 1}, layouts::twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (3,3) with obstruction and crossings")  // 2 valid paths
                {
                    layouts::obstruction_layout obstr_lyt{layout};

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w11 = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    const auto w12 = obstr_lyt.create_buf(w11, {1, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w12, "obstruction PO", {1, 3});  // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w21 = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    const auto w22 = obstr_lyt.create_buf(w21, {2, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w22, "obstruction PO", {2, 3});  // obstructs 1 coordinate

                    const auto path = physical_design::path_finding::a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 3}},
                                                                                        dist(), cost(), params);

                    CHECK((path == coord_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {3, 1}, {3, 2}, {3, 3}}} ||
                           path == coord_path{{{0, 0}, {0, 1}, {0, 2}, {1, 2, 1}, {2, 2, 1}, {3, 2}, {3, 3}}}));
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
                const gate_lyt layout{{3, 2, 1}, layouts::twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (3,2) with obstruction and crossings")  // 1 valid paths
                {
                    layouts::obstruction_layout obstr_lyt{layout};

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w1  = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w1, "obstruction PO", {1, 2});   // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w2  = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w2, "obstruction PO", {3, 1});   // obstructs 1 coordinate

                    const auto path = physical_design::path_finding::a_star<coord_path>(obstr_lyt, {{0, 0}, {3, 2}},
                                                                                        dist(), cost(), params);

                    CHECK(path == coord_path{{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {2, 2}, {3, 2}}});
                }
            }
        }
    }
}

TEST_CASE("A* on 4x4 gate-level layouts with connection obstruction", "[A*]")
{
    using gate_lyt =
        layouts::gate_level_layout<layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>>;
    using coord_path = physical_design::utils::layout_coordinate_path<gate_lyt>;

    SECTION("coordinate paths")
    {
        const gate_lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
        {
            layouts::obstruction_layout obstr_lyt{static_cast<layouts::cartesian_layout<>>(layout)};

            // create some connection obstructions
            obstr_lyt.obstruct_connection({0, 0}, {1, 0});
            obstr_lyt.obstruct_connection({0, 1}, {1, 1});
            obstr_lyt.obstruct_connection({0, 2}, {1, 2});
            // leaving only one valid path via (0,4)

            const auto path = physical_design::path_finding::a_star<coord_path>(
                obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
            const gate_lyt layout{{3, 3}, layouts::twoddwave_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                layouts::obstruction_layout obstr_lyt{layout};

                // create some connection obstructions
                obstr_lyt.obstruct_connection({0, 0}, {1, 0});
                obstr_lyt.obstruct_connection({0, 1}, {1, 1});
                obstr_lyt.obstruct_connection({0, 2}, {1, 2});
                // leaving only one valid path via (0,4)

                const auto path = physical_design::path_finding::a_star<coord_path>(
                    obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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
            const gate_lyt layout{{3, 3}, layouts::use_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // path of length 7
            {
                layouts::obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

                const auto path = physical_design::path_finding::a_star<coord_path>(
                    obstr_lyt, {{0, 0}, {3, 3}});  // only one path possible

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

TEST_CASE("A* on 10x10 layouts with varying distance functions", "[A*]")
{
    using lyt        = layouts::cartesian_layout<layouts::coords::offset>;
    using clk_lyt    = layouts::clocked_layout<lyt>;
    using coord_path = physical_design::utils::layout_coordinate_path<lyt>;

    SECTION("Manhattan distance")
    {
        SECTION("coordinate paths")
        {
            const lyt layout{{9, 9}};

            SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(
                    layout, {{0, 0}, {9, 9}}, physical_design::path_finding::manhattan_distance_functor<lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<lyt>{0, 0});
                CHECK(path.target() == coordinate<lyt>{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const clk_lyt layout{{9, 9}, layouts::res_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = physical_design::path_finding::a_star<coord_path, clk_lyt>(
                        layout, {{0, 0}, {9, 9}}, physical_design::path_finding::manhattan_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
            SECTION("ESP")
            {
                const clk_lyt layout{{9, 9}, layouts::esr_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = physical_design::path_finding::a_star<coord_path>(
                        layout, {{0, 0}, {9, 9}}, physical_design::path_finding::manhattan_distance_functor<clk_lyt>());

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
                const auto path = physical_design::path_finding::a_star<coord_path>(
                    layout, {{0, 0}, {9, 9}}, physical_design::path_finding::euclidean_distance_functor<lyt>());

                CHECK(path.size() == 19);
                CHECK(path.source() == coordinate<lyt>{0, 0});
                CHECK(path.target() == coordinate<lyt>{9, 9});
            }
        }
        SECTION("clocking paths")
        {
            SECTION("RES")
            {
                const clk_lyt layout{{9, 9}, layouts::res_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = physical_design::path_finding::a_star<coord_path, clk_lyt>(
                        layout, {{0, 0}, {9, 9}}, physical_design::path_finding::euclidean_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
            SECTION("ESP")
            {
                const clk_lyt layout{{9, 9}, layouts::esr_clocking<clk_lyt>()};

                SECTION("(0,0) to (9,9) without obstruction")  // path of length 19
                {
                    const auto path = physical_design::path_finding::a_star<coord_path>(
                        layout, {{0, 0}, {9, 9}}, physical_design::path_finding::euclidean_distance_functor<clk_lyt>());

                    CHECK(path.size() == 19);
                    CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                    CHECK(path.target() == coordinate<clk_lyt>{9, 9});
                }
            }
        }
    }
}

TEST_CASE("A* on 4x4 layouts with varying cost functions", "[A*]")
{
    using clk_lyt    = layouts::clocked_layout<layouts::cartesian_layout<layouts::coords::offset>>;
    using coord_path = physical_design::utils::layout_coordinate_path<clk_lyt>;

    const clk_lyt layout{{3, 3}, layouts::use_clocking<clk_lyt>()};

    SECTION("Unit cost")
    {
        const auto path = physical_design::path_finding::a_star<coord_path, clk_lyt>(
            layout, {{0, 0}, {3, 3}}, physical_design::path_finding::manhattan_distance_functor<clk_lyt>(),
            physical_design::path_finding::unit_cost_functor<clk_lyt>());

        CHECK(path.size() == 7);
        CHECK(path.source() == coordinate<clk_lyt>{0, 0});
        CHECK(path.target() == coordinate<clk_lyt>{3, 3});
    }
    SECTION("Random cost")
    {
        const auto path = physical_design::path_finding::a_star<coord_path, clk_lyt>(
            layout, {{0, 0}, {3, 3}}, physical_design::path_finding::manhattan_distance_functor<clk_lyt>(),
            physical_design::path_finding::random_cost_functor<clk_lyt>());

        CHECK(path.size() == 7);
        CHECK(path.source() == coordinate<clk_lyt>{0, 0});
        CHECK(path.target() == coordinate<clk_lyt>{3, 3});
    }
}

TEST_CASE("A* path finding with the A* distance functor (don't do this!)", "[A*]")
{
    using lyt        = layouts::cartesian_layout<layouts::coords::offset>;
    using coord_path = physical_design::utils::layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
        {
            const auto path = physical_design::path_finding::a_star<coord_path>(
                layout, {{0, 0}, {3, 3}}, physical_design::path_finding::a_star_distance_functor<lyt>());

            CHECK(path.size() == 7);
            CHECK(path.source() == coordinate<lyt>{0, 0});
            CHECK(path.target() == coordinate<lyt>{3, 3});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = layouts::clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{3, 3}, layouts::twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(
                    layout, {{0, 0}, {3, 3}}, physical_design::path_finding::a_star_distance_functor<clk_lyt>());

                CHECK(path.size() == 7);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{3, 3}, layouts::use_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // path of length 7
            {
                const auto path = physical_design::path_finding::a_star<coord_path>(
                    layout, {{0, 0}, {3, 3}}, physical_design::path_finding::a_star_distance_functor<clk_lyt>());

                CHECK(path.size() == 7);
                CHECK(path.source() == coordinate<clk_lyt>{0, 0});
                CHECK(path.target() == coordinate<clk_lyt>{3, 3});
            }
        }
    }
}
