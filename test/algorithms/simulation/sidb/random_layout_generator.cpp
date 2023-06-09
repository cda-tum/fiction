//
// Created by Jan Drewniok on 04.05.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/simulation/sidb/random_layout_generator.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <sstream>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout QuickSim simulation", "[generate_random_layout]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>))
{
    SECTION("empty parameters")
    {
        const random_layout_params<TestType> params{};

        const auto lyt = generate_random_layout<TestType>(params);

        CHECK(lyt.num_cells() == 0);
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
    }

    SECTION("given aspect ratio")
    {
        const random_layout_params<TestType> params{{10, 10}};

        const auto result_lyt = generate_random_layout<TestType>(params);

        CHECK(result_lyt.num_cells() == 0);
        result_lyt.foreach_cell(
            [&result_lyt](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.x == 0);
            });
    }

    SECTION("given aspect ratio and number of SiDBs")
    {
        const random_layout_params<TestType> params{{10, 10}, 10};

        const auto result_lyt = generate_random_layout<TestType>(params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 11);
                CHECK(cell.x < 11);
            });
    }

    SECTION("given aspect ratio, number of SiDBs, and allow positive charges")
    {
        const random_layout_params<TestType> params{{9, 9}, 100, false};

        const auto result_lyt = generate_random_layout<TestType>(params);

        CHECK(result_lyt.num_cells() == 100);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 10);
                CHECK(cell.x < 10);
            });
    }

    SECTION("given aspect ratio, number of SiDBs, and allow positive charges")
    {
        const random_layout_params<TestType> params{{9, 9}, 10};

        const auto result_lyt = generate_random_layout<TestType>(params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 10);
                CHECK(cell.x < 10);
            });
        // check if all cells are not closer than two cells (Euclidean distance).
        result_lyt.foreach_cell(
            [&result_lyt](const auto& cell_one)
            {
                result_lyt.foreach_cell(
                    [&cell_one, &result_lyt](const auto& cell_two)
                    {
                        if (cell_one != cell_two)
                        {
                            CHECK(euclidean_distance<TestType>(result_lyt, cell_one, cell_two) >= 2);
                        }
                    });
            });
    }

    SECTION("given previous layouts")
    {
        const random_layout_params<TestType> params{{9, 9}, 10};
        const auto                           result_lyts = generate_multiple_random_layout<TestType>(params, 3);
        CHECK(result_lyts.size() == 3);

        for (const auto& lyt : result_lyts)
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x < 10);
                    CHECK(cell.x < 10);
                });
        }
    }

    SECTION("Check uniqueness of two layouts")
    {
        const random_layout_params<TestType> params{{9, 9}, 10};
        const auto                           result_lyts = generate_multiple_random_layout<TestType>(params, 2);
        REQUIRE(result_lyts.size() == 2);

        const auto& first_lyt  = result_lyts.front();
        const auto& second_lyt = result_lyts.back();

        first_lyt.foreach_cell(
            [&second_lyt](const auto& cell_first)
            { second_lyt.foreach_cell([&cell_first](const auto& cell_second) { CHECK(cell_first != cell_second); }); });
    }
}
