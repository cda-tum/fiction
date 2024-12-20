//
// Created by Jan Drewniok on 04.05.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp>
#include <fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <cstdint>

using namespace fiction;

TEST_CASE("Random cube::coord_t layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{};

        const auto lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(lyt.num_cells() == 0);
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
    }

    SECTION("given corner coordinates, wrong order")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{{{5, 7, 2}, {-10, -10, 0}}};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 0);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given corner coordinates")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{{{-10, -10, 0}, {5, 7, 2}}};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 0);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given two identical coordinates")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{{{-10, -10, 1}, {-10, -10, 1}}, 1};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 1);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == -10);
                CHECK(cell.y == -10);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{{{-10, -10, 0}, {5, 7, 1}}, 10};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x < 6);
                CHECK(cell.x > -11);
                CHECK(cell.y < 8);
                CHECK(cell.y > -11);
                CHECK(cell.z < 21);
                CHECK(cell.z > -1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{
            {{0, 0, 0}, {90, 90, 0}},
            100,
            generate_random_sidb_layout_params<cube::coord_t>::positive_charges::ALLOWED};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 100);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{
            {{0, 0, 0}, {200, 200, 0}},
            50,
            generate_random_sidb_layout_params<cube::coord_t>::positive_charges::FORBIDDEN};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_cube{}, params);

        CHECK(result_lyt.num_cells() == 50);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x <= 200);
                CHECK(cell.y <= 200);
            });

        CHECK(!can_positive_charge_occur(result_lyt, sidb_simulation_parameters{}));
    }

    SECTION("given previous layouts")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{
            {{-5, -2}, {20, 20}},
            6,
            generate_random_sidb_layout_params<cube::coord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            3};
        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_cell_clk_lyt_cube{}, params);
        CHECK(result_lyts.size() == 3);

        for (const auto& lyt : result_lyts)
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 20);
                    CHECK(cell.x > -6);
                    CHECK(cell.y <= 20);
                    CHECK(cell.y > -3);
                });
        }
    }

    SECTION("Check uniqueness of two layouts")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{
            {{0, 0}, {20, 20}},
            8,
            generate_random_sidb_layout_params<cube::coord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            2};

        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_cell_clk_lyt_cube{}, params);

        REQUIRE(result_lyts.size() == 2);

        const auto& first_lyt  = result_lyts.front();
        const auto& second_lyt = result_lyts.back();

        CHECK(!are_cell_layouts_identical(first_lyt, second_lyt));
    }

    SECTION("Check all pairwise distances")
    {
        const generate_random_sidb_layout_params<cube::coord_t> params{
            {{0, 0}, {40, 40}},
            10,
            generate_random_sidb_layout_params<cube::coord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            10};
        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_cell_clk_lyt_cube{}, params);
        REQUIRE(result_lyts.size() == 10);

        for (const auto& lyt : result_lyts)
        {
            CHECK(!can_positive_charge_occur(lyt, sidb_simulation_parameters{}));
        }
    }
}

TEST_CASE("Random offset::ucoord_t layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{};

        const auto lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(lyt.num_cells() == 0);
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
    }

    SECTION("given corner coordinates")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{{{1, 1, 0}, {5, 7, 2}}};

        const auto result_lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(result_lyt.num_cells() == 0);
        result_lyt.foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given two identical coordinates")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{{{5, 5, 1}, {5, 5, 1}}, 1};

        const auto result_lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(result_lyt.num_cells() == 1);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{{{1, 1, 0}, {50, 7, 1}}, 10};

        const auto result_lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 51);
                CHECK(cell.x > 0);
                CHECK(cell.y < 8);
                CHECK(cell.y > 0);
                CHECK(cell.z < 21);
                CHECK(cell.z >= 0);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{
            {{0, 0, 0}, {90, 90, 0}},
            100,
            generate_random_sidb_layout_params<offset::ucoord_t>::positive_charges::ALLOWED};

        const auto result_lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(result_lyt.num_cells() == 100);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{
            {{0, 0, 0}, {200, 200, 0}},
            100,
            generate_random_sidb_layout_params<offset::ucoord_t>::positive_charges::FORBIDDEN};

        const auto result_lyt = generate_random_sidb_layout(sidb_100_cell_clk_lyt{}, params);

        CHECK(result_lyt.num_cells() == 100);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x <= 200);
                CHECK(cell.y <= 200);
            });

        CHECK(!can_positive_charge_occur(result_lyt, sidb_simulation_parameters{}));
    }

    SECTION("given previous layouts")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{
            {{0, 0}, {40, 40}},
            10,
            generate_random_sidb_layout_params<offset::ucoord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            3};
        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_100_cell_clk_lyt{}, params);
        CHECK(result_lyts.size() == 3);

        for (const auto& lyt : result_lyts)
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 40);
                    CHECK(cell.y <= 40);
                });
        }
    }

    SECTION("Check uniqueness of two layouts")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{
            {{0, 0}, {20, 20}},
            10,
            generate_random_sidb_layout_params<offset::ucoord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            2};
        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_100_cell_clk_lyt{}, params);
        REQUIRE(result_lyts.size() == 2);

        const auto& first_lyt  = result_lyts.front();
        const auto& second_lyt = result_lyts.back();

        CHECK(!are_cell_layouts_identical(first_lyt, second_lyt));
    }

    SECTION("Check correct use of skeleton layout when generating only one random layout")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{{{0, 0}, {9, 9}}, 10};
        sidb_100_cell_clk_lyt                                      skeleton_layout{};
        skeleton_layout.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::technology::NORMAL);
        skeleton_layout.assign_cell_type({9, 1}, sidb_100_cell_clk_lyt::technology::NORMAL);
        skeleton_layout.assign_cell_type({5, 0}, sidb_100_cell_clk_lyt::technology::NORMAL);
        const auto result_lyt = generate_random_sidb_layout(skeleton_layout, params);

        CHECK(result_lyt.num_cells() == 13);
    }

    SECTION("Check correct use of skeleton layout when generating multiple random layouts")
    {
        const generate_random_sidb_layout_params<offset::ucoord_t> params{
            {{0, 0}, {40, 40}},
            10,
            generate_random_sidb_layout_params<offset::ucoord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            2};

        sidb_100_cell_clk_lyt skeleton_layout{};

        skeleton_layout.assign_cell_type({0, 0}, sidb_100_cell_clk_lyt::technology::NORMAL);
        skeleton_layout.assign_cell_type({3, 0}, sidb_100_cell_clk_lyt::technology::NORMAL);
        skeleton_layout.assign_cell_type({9, 1}, sidb_100_cell_clk_lyt::technology::NORMAL);

        const auto result_lyts = generate_multiple_random_sidb_layouts<sidb_100_cell_clk_lyt>(skeleton_layout, params);

        REQUIRE(result_lyts.size() == 2);
        CHECK(result_lyts.front().num_cells() == 13);
        CHECK(result_lyts.back().num_cells() == 13);
    }
}

TEST_CASE("Random siqad::coord_t layout generation", "[random-sidb-layout-generator]")
{
    SECTION("empty parameters")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{};

        const auto lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(lyt.num_cells() == 0);
        CHECK(lyt.x() == 0);
        CHECK(lyt.y() == 0);
    }

    SECTION("given two identical coordinates")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{{{5, 5, 1}, {5, 5, 1}}, 1};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(result_lyt.num_cells() == 1);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("given corner coordinates")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{{{1, 1, 0}, {5, 7, 1}}};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(result_lyt.num_cells() == 0);
        result_lyt.foreach_cell(
            [&](const auto& cell)
            {
                CHECK(cell.x == 0);
                CHECK(cell.y == 0);
                CHECK(cell.z == 0);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{{{1, 1, 0}, {50, 7, 1}}, 10};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 51);
                CHECK(cell.x > 0);
                CHECK(cell.y < 8);
                CHECK(cell.y > 0);
                CHECK(cell.z <= 1);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{
            {{0, 0, 0}, {90, 90, 0}},
            100,
            generate_random_sidb_layout_params<siqad::coord_t>::positive_charges::ALLOWED};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(result_lyt.num_cells() == 100);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
            });
    }

    SECTION("given corner coordinates and number of placed SiDBs, and forbid positive charges")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{
            {{0, 0, 0}, {90, 90, 0}},
            10,
            generate_random_sidb_layout_params<siqad::coord_t>::positive_charges::FORBIDDEN};

        const auto result_lyt = generate_random_sidb_layout(sidb_cell_clk_lyt_siqad{}, params);

        CHECK(result_lyt.num_cells() == 10);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x < 91);
                CHECK(cell.y < 91);
                CHECK(cell.z <= 1);
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
                            CHECK(euclidean_distance<sidb_cell_clk_lyt_siqad>(result_lyt, cell_one, cell_two) >= 2);
                        }
                    });
            });
    }

    SECTION("given previous layouts")
    {
        const generate_random_sidb_layout_params<siqad::coord_t> params{
            {{0, 0, 1}, {20, 20, 1}},
            10,
            generate_random_sidb_layout_params<siqad::coord_t>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            static_cast<uint64_t>(10E6),
            3};
        const auto result_lyts = generate_multiple_random_sidb_layouts(sidb_cell_clk_lyt_siqad{}, params);
        CHECK(result_lyts.size() == 3);

        for (const auto& lyt : result_lyts)
        {
            lyt.foreach_cell(
                [](const auto& cell)
                {
                    CHECK(cell.x <= 20);
                    CHECK(cell.x >= 0);
                    CHECK(cell.y <= 20);
                    CHECK(cell.y >= 0);
                    CHECK(cell.z <= 1);
                });
        }
    }
}

TEMPLATE_TEST_CASE("Random siqad::coord_t layout generation with defects", "[random-sidb-layout-generator]",
                   sidb_defect_cell_clk_lyt_siqad, sidb_defect_surface<sidb_111_cell_clk_lyt_siqad>)
{
    SECTION("given two identical coordinates")
    {
        const generate_random_sidb_layout_params<cell<TestType>> params{{{5, 5, 1}, {5, 5, 1}}, 1};

        const auto result_lyt = generate_random_sidb_layout(TestType{}, params);

        CHECK(result_lyt.num_cells() == 1);
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell.x == 5);
                CHECK(cell.y == 5);
                CHECK(cell.z == 1);
            });
    }

    SECTION("region including only one cell and there is a defect")
    {
        // it is not possible to generate a random layout since the position where a SiDB could be placed is occupied by
        // a defect.
        const generate_random_sidb_layout_params<cell<TestType>> params{
            {{2, 1, 1}, {2, 1, 1}},
            1,
            generate_random_sidb_layout_params<cell<TestType>>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            5u};

        auto defect_layout = TestType{};
        defect_layout.assign_sidb_defect({2, 1, 1}, sidb_defect{sidb_defect_type::DB, -1, 5.6, 5});

        const auto result_lyt = generate_random_sidb_layout(defect_layout, params);

        CHECK(result_lyt.num_cells() == 0);
        CHECK(result_lyt.num_defects() == 1);
    }

    SECTION("region including only one cell and there is no defect")
    {
        // it is not possible to generate a random layout since the position where a SiDB could be placed is occupied by
        // a defect.
        const generate_random_sidb_layout_params<cell<TestType>> params{
            {{2, 1, 1}, {2, 1, 1}},
            1,
            generate_random_sidb_layout_params<cell<TestType>>::positive_charges::FORBIDDEN,
            sidb_simulation_parameters{},
            5u};

        auto defect_layout = TestType{};
        defect_layout.assign_sidb_defect({3, 1, 1}, sidb_defect{sidb_defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_sidb_defect({4, 1, 1}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});

        const auto result_lyt = generate_random_sidb_layout(defect_layout, params);

        CHECK(result_lyt.num_cells() == 1);
        CHECK(result_lyt.num_defects() == 2);

        CHECK(result_lyt.get_cell_type({2, 1, 1}) == TestType::technology::cell_type::LOGIC);
        CHECK(result_lyt.get_sidb_defect({3, 1, 1}) == sidb_defect{sidb_defect_type::DB, -1, 5.6, 5});
        CHECK(result_lyt.get_sidb_defect({4, 1, 1}) == sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
    }

    SECTION("given corner coordinates and number of placed SiDBs, and allow positive charges")
    {
        const generate_random_sidb_layout_params<cell<TestType>> params{
            {{0, 0, 0}, {10, 2, 0}},
            10,
            generate_random_sidb_layout_params<cell<TestType>>::positive_charges::ALLOWED,
            sidb_simulation_parameters{}};

        auto defect_layout = TestType{};
        defect_layout.assign_sidb_defect({2, 2, 0}, sidb_defect{sidb_defect_type::DB, -1, 5.6, 5});
        defect_layout.assign_sidb_defect({4, 1, 0}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
        defect_layout.assign_sidb_defect({5, 1, 0}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 9});
        defect_layout.assign_sidb_defect({7, 1, 0}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 2.6, 7});
        defect_layout.assign_sidb_defect({2, 1, 0}, sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 4});

        const auto result_lyt = generate_random_sidb_layout(defect_layout, params);

        CHECK(result_lyt.num_cells() == 10);
        CHECK(result_lyt.num_defects() == 5);

        // check if all cells are not closer than two cells (Euclidean distance).
        result_lyt.foreach_cell(
            [](const auto& cell)
            {
                CHECK(cell != siqad::coord_t{2, 2, 0});
                CHECK(cell != siqad::coord_t{4, 1, 0});
                CHECK(cell != siqad::coord_t{5, 1, 0});
                CHECK(cell != siqad::coord_t{7, 1, 0});
                CHECK(cell != siqad::coord_t{2, 1, 0});
            });
    }
}

TEST_CASE("Random cube::coord_t layout generation with defects", "[random-sidb-layout-generator]")
{
    using lyt = sidb_defect_cell_clk_lyt_cube;

    const generate_random_sidb_layout_params<cube::coord_t> params{
        {siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{0, 0, 0}),
         siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 2, 0})},
        10,
        generate_random_sidb_layout_params<cube::coord_t>::positive_charges::ALLOWED,
        sidb_simulation_parameters{}};

    lyt layout{};

    layout.assign_sidb_defect(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 2, 0}),
                              sidb_defect{sidb_defect_type::DB, -1, 5.6, 5});
    layout.assign_sidb_defect(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 1, 0}),
                              sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 7});
    layout.assign_sidb_defect(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{5, 1, 0}),
                              sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 9});
    layout.assign_sidb_defect(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{7, 1, 0}),
                              sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 2.6, 7});
    layout.assign_sidb_defect(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 1, 0}),
                              sidb_defect{sidb_defect_type::SINGLE_DIHYDRIDE, 1, 7.6, 4});

    const auto result_lyt = generate_random_sidb_layout(layout, params);

    CHECK(result_lyt.num_cells() == 10);
    CHECK(result_lyt.num_defects() == 5);

    // check if all cells are not closer than two cells (Euclidean distance).
    result_lyt.foreach_cell(
        [](const auto& cell)
        {
            CHECK(cell != siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 2, 0}));
            CHECK(cell != siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 1, 0}));
            CHECK(cell != siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{5, 1, 0}));
            CHECK(cell != siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{7, 1, 0}));
            CHECK(cell != siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 1, 0}));
        });
}
