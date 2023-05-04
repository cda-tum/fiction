//
// Created by Jan Drewniok on 04.05.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/random_layout_generator.hpp>
#include <fiction/io/read_sqd_layout.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout QuickSim simulation", "[quicksim]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<offset::ucoord_t>>>))
{
    std::stringstream layout_stream{};
    SECTION("empty parameters")
    {
        random_layout_params<TestType> params{};

        generate_random_layout<TestType>(params, layout_stream);
        const auto read_layout = read_sqd_layout<TestType>(layout_stream);

        CHECK(read_layout.num_cells() == 0);
        CHECK(read_layout.x() == 0);
        CHECK(read_layout.y() == 0);
    }

    SECTION("given aspect ratio")
    {
        random_layout_params<TestType> params{{10, 10}};

        generate_random_layout<TestType>(params, layout_stream);
        const auto read_layout = read_sqd_layout<TestType>(layout_stream);

        CHECK(read_layout.num_cells() == 0);
        CHECK(read_layout.x() == 0);
        CHECK(read_layout.y() == 0);
    }

    SECTION("given aspect ratio and number of SiDBs")
    {
        random_layout_params<TestType> params{{10, 10}, 10};

        generate_random_layout<TestType>(params, layout_stream);
        const auto read_layout = read_sqd_layout<TestType>(layout_stream);

        CHECK(read_layout.num_cells() == 10);
        CHECK(read_layout.x() < 11);
        CHECK(read_layout.y() < 11);
    }

    SECTION("given aspect ratio, number of SiDBs, and allow positive charges")
    {
        random_layout_params<TestType> params{{9, 9}, 100, false};

        generate_random_layout<TestType>(params, layout_stream);
        const auto read_layout = read_sqd_layout<TestType>(layout_stream);

        CHECK(read_layout.num_cells() == 100);
        CHECK(read_layout.x() == 9);
        CHECK(read_layout.y() == 9);
    }

    SECTION("given aspect ratio, number of SiDBs, and allow positive charges")
    {
        random_layout_params<TestType> params{{9, 9}, 10};

        generate_random_layout<TestType>(params, layout_stream);
        const auto read_layout = read_sqd_layout<TestType>(layout_stream);

        CHECK(read_layout.num_cells() == 10);
        CHECK(read_layout.x() < 10);
        CHECK(read_layout.y() < 10);
        // check if all cells are not closer than two cells (Euclidean distance).
        read_layout.foreach_cell(
            [&read_layout](const auto& cell_one)
            {
                read_layout.foreach_cell(
                    [&read_layout, &cell_one](const auto& cell_two)
                    {
                        if (cell_one != cell_two)
                        {
                            CHECK(euclidean_distance<TestType>(read_layout, cell_one, cell_two) >= 2);
                        }
                    });
            });
    }
}
