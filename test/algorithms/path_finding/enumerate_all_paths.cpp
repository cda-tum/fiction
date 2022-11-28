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

TEST_CASE("Enumerate all paths on 2x2 clocked layouts", "[enumerate-all-paths]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using path    = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

        SECTION("(0,0) to (1,1)")  // two valid paths
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {1, 1}});

            CHECK(collection.size() == 2);
            CHECK(collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
            CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
        }
        SECTION("(1,1) to (0,0)")  // no valid paths
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{1, 1}, {0, 0}});

            CHECK(collection.empty());
            CHECK(!collection.contains({{{0, 0}, {0, 1}, {1, 1}}}));
            CHECK(!collection.contains({{{0, 0}, {1, 0}, {1, 1}}}));
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {0, 0}});

            CHECK(collection.size() == 1);
            CHECK(collection.contains({{{0, 0}}}));
            CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
            CHECK(collection[0].target() == coordinate<clk_lyt>{0, 0});
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{1, 1}, use_clocking<clk_lyt>()};

        SECTION("(0,0) to (0,1)")  // one valid path
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {0, 1}});

            CHECK(collection.size() == 1);
            CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}));
            CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
            CHECK(collection[0].target() == coordinate<clk_lyt>{0, 1});
        }
        SECTION("(0,0) to (0,0)")  // source and target are identical
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {0, 0}});

            CHECK(collection.size() == 1);
            CHECK(collection.contains({{{0, 0}}}));
            CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
            CHECK(collection[0].target() == coordinate<clk_lyt>{0, 0});
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 clocked layouts", "[enumerate-all-paths]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using path    = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 20);
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

        SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
        {
            const auto collection = enumerate_all_clocking_paths<path>(layout, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 4);
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with coordinate obstruction", "[enumerate-all-paths]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using path     = layout_coordinate_path<gate_lyt>;

    SECTION("2DDWave")
    {
        gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // 19 valid paths
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 1 path

            const auto collection = enumerate_all_clocking_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 19);
        }
    }
    SECTION("USE")
    {
        gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with coordinate obstruction")  // 1 valid path
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.create_pi("obstruction", {3, 0});  // blocks 3 paths

            const auto collection = enumerate_all_clocking_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 1);
        }
    }
}

TEST_CASE("Enumerate all paths on 4x4 gate-level layouts with connection obstruction", "[enumerate-all-paths]")
{
    using gate_lyt = gate_level_layout<clocked_layout<cartesian_layout<offset::ucoord_t>>>;
    using path     = layout_coordinate_path<gate_lyt>;

    SECTION("2DDWave")
    {
        gate_lyt layout{{3, 3}, twoddwave_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with connection obstruction")  // 19 valid paths
        {
            obstruction_layout obstr_lyt{layout};

            // create a connection obstruction
            obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 1 path

            const auto collection = enumerate_all_clocking_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 19);
        }
    }
    SECTION("USE")
    {
        gate_lyt layout{{3, 3}, use_clocking<gate_lyt>()};

        SECTION("(0,0) to (3,3) with connection obstruction")  // 1 valid path
        {
            obstruction_layout obstr_lyt{layout};

            // create a PI as obstruction
            obstr_lyt.obstruct_connection({2, 0}, {3, 0});  // blocks 3 paths

            const auto collection = enumerate_all_clocking_paths<path>(obstr_lyt, {{0, 0}, {3, 3}});

            CHECK(collection.size() == 1);
        }
    }
}
