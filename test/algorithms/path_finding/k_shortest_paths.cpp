//
// Created by marcel on 23.02.22.
//

#include "catch.hpp"

#include <fiction/algorithms/path_finding/k_shortest_paths.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/obstruction_layout.hpp>

using namespace fiction;

TEST_CASE("Yen's 2x2 clocked layouts", "[k-shortest-paths]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using path    = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{1, 1}, twoddwave_clocking<clk_lyt>()};

        SECTION("k = 1")
        {
            constexpr const auto k = 1;

            SECTION("(0,0) to (1,1)")  // two valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {1, 1}, k);

                CHECK(collection.size() == 1);
                CHECK(collection[0].source() == coordinate<clk_lyt>{0, 0});
                CHECK(collection[0].target() == coordinate<clk_lyt>{1, 1});
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {1, 1}, {0, 0}, k);

                CHECK(collection.empty());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
        SECTION("k = 2")
        {
            constexpr const auto k = 2;

            SECTION("(0,0) to (1,1)")  // two valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {1, 1}, k);

                CHECK(collection.size() == 2);
                CHECK(collection.contains({{0, 0}, {1, 0}, {1, 1}}));
                CHECK(collection.contains({{0, 0}, {0, 1}, {1, 1}}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {1, 1}, {0, 0}, k);

                CHECK(collection.empty());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
        SECTION("k = 3")
        {
            constexpr const auto k = 3;

            SECTION("(0,0) to (1,1)")  // two valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {1, 1}, k);

                CHECK(collection.size() == 2);
                CHECK(collection.contains({{0, 0}, {1, 0}, {1, 1}}));
                CHECK(collection.contains({{0, 0}, {0, 1}, {1, 1}}));
            }
            SECTION("(1,1) to (0,0)")  // no valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {1, 1}, {0, 0}, k);

                CHECK(collection.empty());
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{1, 1}, use_clocking<clk_lyt>()};

        SECTION("k = 1")
        {
            constexpr const auto k = 1;

            SECTION("(0,0) to (0,1)")  // one valid path
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 1}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}));
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
        SECTION("k = 2")
        {
            constexpr const auto k = 2;

            SECTION("(0,0) to (0,1)")  // one valid path
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 1}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}));
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
        SECTION("k = 3")
        {
            constexpr const auto k = 3;

            SECTION("(0,0) to (0,1)")  // one valid path
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 1}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}, {1, 0}, {1, 1}, {0, 1}}}));
            }
            SECTION("(0,0) to (0,0)")  // source and target are identical
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {0, 0}, k);

                CHECK(collection.size() == 1);
                CHECK(collection.contains({{{0, 0}}}));
            }
        }
    }
}

TEST_CASE("Yen's 4x4 clocked layouts", "[k-shortest-paths]")
{
    using clk_lyt = clocked_layout<cartesian_layout<offset::ucoord_t>>;
    using path    = layout_coordinate_path<clk_lyt>;

    SECTION("2DDWave")
    {
        clk_lyt layout{{3, 3}, twoddwave_clocking<clk_lyt>()};

        SECTION("k = 1")
        {
            constexpr const auto k = 1;

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 5")
        {
            constexpr const auto k = 5;

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 10")
        {
            constexpr const auto k = 10;

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 20")
        {
            constexpr const auto k = 20;

            SECTION("(0,0) to (3,3) without obstruction")  // 20 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
    }
    SECTION("USE")
    {
        clk_lyt layout{{3, 3}, use_clocking<clk_lyt>()};

        SECTION("k = 1")
        {
            constexpr const auto k = 1;

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 2")
        {
            constexpr const auto k = 2;

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 3")
        {
            constexpr const auto k = 3;

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
        SECTION("k = 4")
        {
            constexpr const auto k = 4;

            SECTION("(0,0) to (3,3) without obstruction")  // 4 valid paths
            {
                const auto collection = yen_k_shortest_paths<path>(layout, {0, 0}, {3, 3}, k);

                CHECK(collection.size() == k);
            }
        }
    }
}

TEST_CASE("Yen's 4x4 gate-level layouts with coordinate obstruction", "[k-shortest-paths]")
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

            const auto collection =
                yen_k_shortest_paths<coord_path>(obstr_lyt, {0, 0}, {3, 3}, 1);  // only one path possible

            REQUIRE(collection.size() == 1);
            const auto& path = collection[0];
            REQUIRE(path.size() == 7);
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

            const auto collection =
                yen_k_shortest_paths<coord_path>(obstr_lyt, {0, 0}, {3, 3}, 1);  // only one path possible

            REQUIRE(collection.size() == 1);
            const auto& path = collection[0];
            REQUIRE(path.size() == 7);
            CHECK(path[1] == coordinate<gate_lyt>{1, 0});
            CHECK(path[2] == coordinate<gate_lyt>{1, 1});
            CHECK(path[3] == coordinate<gate_lyt>{1, 2});
            CHECK(path[4] == coordinate<gate_lyt>{2, 2});
            CHECK(path[5] == coordinate<gate_lyt>{3, 2});
        }
    }
}

TEST_CASE("Yen's 4x4 gate-level layouts with connection obstruction", "[k-shortest-paths]")
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

            const auto collection =
                yen_k_shortest_paths<coord_path>(obstr_lyt, {0, 0}, {3, 3}, 1);  // only one path possible

            REQUIRE(collection.size() == 1);
            const auto& path = collection[0];
            REQUIRE(path.size() == 7);
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

            const auto collection =
                yen_k_shortest_paths<coord_path>(obstr_lyt, {0, 0}, {3, 3}, 1);  // only one path possible

            REQUIRE(collection.size() == 1);
            const auto& path = collection[0];
            REQUIRE(path.size() == 7);
            CHECK(path[1] == coordinate<gate_lyt>{1, 0});
            CHECK(path[2] == coordinate<gate_lyt>{1, 1});
            CHECK(path[3] == coordinate<gate_lyt>{1, 2});
            CHECK(path[4] == coordinate<gate_lyt>{2, 2});
            CHECK(path[5] == coordinate<gate_lyt>{3, 2});
        }
    }
}
