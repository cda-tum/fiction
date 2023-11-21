//
// Created by Jan Drewniok on 25.07.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/sidb_nm_position.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math_utils.hpp>

using namespace fiction;

TEST_CASE("SiDB position in nanometer for siqad coordinate", "[sidb_nm_position]")
{
    using namespace Catch::Matchers;

    SECTION("Default lattice constants, positive cell coordinates")
    {
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos_x, WithinAbs(params.lat_a * 0.1, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(params.lat_b * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 8, 1});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(params.lat_b * 0.8 + params.lat_c * 0.1, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 1});
        CHECK_THAT(pos4_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(params.lat_b * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 1, 1});
        CHECK_THAT(pos5_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(params.lat_b * 0.1 + params.lat_c * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 10, 1});
        CHECK_THAT(pos6_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(params.lat_b + params.lat_c * 0.1, 1E-5));
    }

    SECTION("Default lattice constants, negative cell coordinates")
    {
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.0, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, -1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(-params.lat_b * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-5, -5});
        CHECK_THAT(pos3_x, WithinAbs(-params.lat_a * 0.5, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(-params.lat_b * 0.5, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -1, 1});
        CHECK_THAT(pos4_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(-params.lat_b * 0.1 + params.lat_c * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -10, 1});
        CHECK_THAT(pos5_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(-params.lat_b + params.lat_c * 0.1, 1E-5));
    }

    SECTION("Special lattice constants, positive and negative cell coordinates")
    {
        const sidb_simulation_parameters params{3, -0.32, 5.6, 5.0, 1, 2, 3};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(-0.1, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.0, 1E-5));

        const auto [pos1_x, pos1_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, -1, 0});
        CHECK_THAT(pos1_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos1_y, WithinAbs(-0.2, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 0, 1});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(0.3, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 0, 1});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(0.3, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {10, 1, 1});
        CHECK_THAT(pos4_x, WithinAbs(1.0, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(0.5, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -1, 1});
        CHECK_THAT(pos5_x, WithinAbs(-0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {10, -10, 1});
        CHECK_THAT(pos6_x, WithinAbs(1.0, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(-1.7, 1E-5));
    }
}

TEST_CASE("SiDB position in nanometer for fiction coordinates", "[sidb_nm_position]")
{
    using namespace Catch::Matchers;

    using sidb_cell_clk_lyt_cube = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<cube::coord_t>>>;

    SECTION("Default lattice constants, positive cell coordinates")
    {
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {1, 0});
        CHECK_THAT(pos_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos_x, WithinAbs(params.lat_a * 0.1, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {0, 2});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(params.lat_b * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {0, 17});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(params.lat_b * 0.8 + params.lat_c * 0.1, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {1, 2});
        CHECK_THAT(pos4_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(params.lat_b * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {1, 3});
        CHECK_THAT(pos5_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(params.lat_b * 0.1 + params.lat_c * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<sidb_cell_clk_lyt>(params, {1, 21});
        CHECK_THAT(pos6_x, WithinAbs(params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(params.lat_b + params.lat_c * 0.1, 1E-5));
    }

    SECTION("Default lattice constants, negative cell coordinates")
    {
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {-1, 0});
        CHECK_THAT(pos_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.0, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {0, -2});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(-params.lat_b * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {-5, -10});
        CHECK_THAT(pos3_x, WithinAbs(-params.lat_a * 0.5, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(-params.lat_b * 0.5, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {-1, -3});
        CHECK_THAT(pos4_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(-params.lat_b * 0.1 + params.lat_c * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_cube>(params, {-1, -21});
        CHECK_THAT(pos5_x, WithinAbs(-params.lat_a * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(-params.lat_b + params.lat_c * 0.1, 1E-5));
    }
}
