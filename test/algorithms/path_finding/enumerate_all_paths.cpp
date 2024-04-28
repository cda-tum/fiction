//
// Created by marcel on 01.02.22.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/path_finding/enumerate_all_paths.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>

using namespace fiction;

TEST_CASE("Enumerate all paths on 2x2 layouts", "[enumerate-all-paths]")
{
    using lyt  = cartesian_layout<offset::ucoord_t>;
    using path = layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{1, 1}};

        SECTION("(0,0) to (1,1)")  // two valid paths
        {
            const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {1, 1}});

            CHECK(collection.size() == 2);
            CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
            CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
        }
        SECTION("(1,1) to (0,0)")  // two valid paths
        {
            const auto collection = enumerate_all_paths<path>(layout, {{1, 1}, {0, 0}});

            CHECK(collection.size() == 2);
            CHECK(collection.contains({{{1, 1}, {0, 1}, {0, 0}}}));
            CHECK(collection.contains({{{1, 1}, {1, 0}, {0, 0}}}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {0, 0}});

            CHECK(collection.size() == 1);
            CHECK(collection.contains({{{0, 0}}}));
            CHECK(collection[0].source() == coordinate<lyt>{0, 0});
            CHECK(collection[0].target() == coordinate<lyt>{0, 0});
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (1,1)")  // two valid paths
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {1, 1}});

                CHECK(collection.size() == 2);
                CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
                CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto collection = enumerate_all_paths<path>(layout, {{1, 1}, {0, 0}});

                CHECK(collection.empty());
                CHECK(!collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
                CHECK(!collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {0, 0}});

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
                CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
                CHECK(collection[0].target() == coordinate<clk_lyt>{0, 0});
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{1, 1}, use_clocking<clk_lyt>()};

            SECTION("(0,0) to (0,1)")  // one valid path
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {0, 1}});

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}));
                CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
                CHECK(collection[0].target() == coordinate<clk_lyt>{0, 1});
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {0, 0}});

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
                CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
                CHECK(collection[0].target() == coordinate<clk_lyt>{0, 0});
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 layouts", "[enumerate-all-paths]")
{
    using lyt  = cartesian_layout<offset::ucoord_t>;
    using path = layout_coordinate_path<lyt>;

    SECTION("coordinate paths")
    {
        const lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) without obstruction")  // 184 valid paths
        {
            const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 184);
        }
    }
    SECTION("clocking paths")
    {
        using clk_lyt = clocked_layout<lyt>;

        SECTION("2DDWave")
        {
            const clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 20);
            }
        }
        SECTION("USE")
        {
            const clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = enumerate_all_paths<path>(layout, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 4);
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with coordinate obstruction", "[enumerate-all-paths]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using path     = layout_coordinate_path<gate_lyt>;

    SECTION("coordinate paths")
    {
        const gate_lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // 108 valid paths
        {
            obstruction_layout obstr_lyt{static_cast<cartesian_layout<>>(layout)};

            // mark coordinate as obstructed
            obstr_lyt.obstruct_coordinate({3, 0});  // blocks 75 paths

            const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 108);
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // 19 valid paths
            {
                obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 1 path

                const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 19);
            }
        }
        SECTION("USE")
        {
            const gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with coordinate obstruction")  // 1 valid path
            {
                obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

                const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 1);
            }
        }
    }
}

TEST_CASE("Enumerate all paths with coordinate obstruction but crossings enabled", "[enumerate-all-paths]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using path     = layout_coordinate_path<gate_lyt>;

    // enable crossings
    const enumerate_all_paths_params params{true};

    SECTION("Single crossing")
    {
        SECTION("3x3")
        {
            SECTION("2DDWave")
            {
                const gate_lyt layout{{2, 2, 1}, twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    obstruction_layout obstr_lyt{layout};

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {1, 0});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {1, 2});  // obstructs 1 coordinate

                    const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {2, 2}}, params);

                    CHECK(collection.size() == 1);
                    CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1}, {2, 2}}}));
                }
            }
            SECTION("USE")
            {
                const gate_lyt layout{{2, 2, 1}, use_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (2,2) with obstruction and crossings")  // 1 valid path
                {
                    obstruction_layout obstr_lyt{layout};

                    // create a path as obstruction
                    const auto pi = obstr_lyt.create_pi("obstruction PI", {2, 1});  // obstructs 1 coordinate
                    const auto w  = obstr_lyt.create_buf(pi, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w, "obstruction PO", {0, 1});  // obstructs 1 coordinate

                    const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {2, 2}}, params);

                    CHECK(collection.size() == 1);
                    CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1, 1}, {1, 2}, {2, 2}}}));
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
                const gate_lyt layout{{3, 3, 1}, twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (3,3) with obstruction and crossings")  // 2 valid paths
                {
                    obstruction_layout obstr_lyt{layout};

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w11 = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    const auto w12 = obstr_lyt.create_buf(w11, {1, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w12, "obstruction PO", {1, 3});  // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w21 = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    const auto w22 = obstr_lyt.create_buf(w21, {2, 2});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w22, "obstruction PO", {2, 3});  // obstructs 1 coordinate

                    const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}}, params);

                    CHECK(collection.size() == 2);
                    CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {3, 1}, {3, 2}, {3, 3}}}));
                    CHECK(collection.contains({{{0, 0}, {0, 1}, {0, 2}, {1, 2, 1}, {2, 2, 1}, {3, 2}, {3, 3}}}));
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
                const gate_lyt layout{{3, 2, 1}, twoddwave_clocking<gate_lyt>()};  // create a crossing layer

                SECTION("(0,0) to (3,2) with obstruction and crossings")  // 1 valid paths
                {
                    obstruction_layout obstr_lyt{layout};

                    // create two paths as obstruction
                    const auto pi1 = obstr_lyt.create_pi("obstruction PI 1", {1, 0});  // obstructs 1 coordinate
                    const auto w1  = obstr_lyt.create_buf(pi1, {1, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w1, "obstruction PO", {1, 2});   // obstructs 1 coordinate

                    const auto pi2 = obstr_lyt.create_pi("obstruction PI 1", {2, 0});  // obstructs 1 coordinate
                    const auto w2  = obstr_lyt.create_buf(pi2, {2, 1});  // obstruction that can be crossed over
                    obstr_lyt.create_po(w2, "obstruction PO", {3, 1});   // obstructs 1 coordinate

                    const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 2}}, params);

                    CHECK(collection.size() == 1);
                    CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1, 1}, {2, 1, 1}, {2, 2}, {3, 2}}}));
                }
            }
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with connection obstruction", "[enumerate-all-paths]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using path     = layout_coordinate_path<gate_lyt>;

    SECTION("coordinate paths")
    {
        const gate_lyt layout{{3, 3}};

        SECTION("(0,0) to (3,3) with connection obstruction")  // 108 valid paths
        {
            obstruction_layout obstr_lyt{static_cast<cartesian_layout<>>(layout)};

            // create a connection obstruction
            obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 75 paths

            const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 108);
        }
    }
    SECTION("clocking paths")
    {
        SECTION("2DDWave")
        {
            const gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // 19 valid paths
            {
                obstruction_layout obstr_lyt{layout};

                // create a connection obstruction
                obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 1 path

                const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 19);
            }
        }
        SECTION("USE")
        {
            const gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

            SECTION("(0,0) to (3,3) with connection obstruction")  // 1 valid path
            {
                obstruction_layout obstr_lyt{layout};

                // create a PI as obstruction
                obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

                const auto collection = enumerate_all_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

                CHECK(collection.size() == 1);
            }
        }
    }
}
