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

TEST_CASE("SiDB position in nanometer", "[sidb_nm_position]")
{
    SECTION("Default lattice constants, positive cell coordinates")
    {
        const sidb_cell_clk_lyt_siqad    layout{};
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 0, 0});
        CHECK(pos_x == params.lat_a * 0.1);
        CHECK(pos_y == 0);

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 1, 0});
        CHECK(pos2_x == 0);
        CHECK(pos2_y == params.lat_b * 0.1);

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 8, 1});
        CHECK(pos3_x == 0);
        CHECK(pos3_y == params.lat_b * 0.8 + params.lat_c * 0.1);

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 1});
        CHECK(pos4_x == params.lat_a * 0.1);
        CHECK(pos4_y == params.lat_b * 0.1);

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 1, 1});
        CHECK(pos5_x == params.lat_a * 0.1);
        CHECK(pos5_y == params.lat_b * 0.1 + params.lat_c * 0.1);

        const auto [pos6_x, pos6_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {1, 10, 1});
        CHECK(pos6_x == params.lat_a * 0.1);
        CHECK(round_to_n_decimal_places(pos6_y, 4) == round_to_n_decimal_places(params.lat_b + params.lat_c * 0.1, 4));
    }

    SECTION("Default lattice constants, negative cell coordinates")
    {
        const sidb_cell_clk_lyt_siqad    layout{};
        const sidb_simulation_parameters params{};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, 0, 0});
        CHECK(pos_x == -params.lat_a * 0.1);
        CHECK(pos_y == 0);

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, -1, 0});
        CHECK(pos2_x == 0);
        CHECK(pos2_y == -params.lat_b * 0.1);

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-5, -5});
        CHECK(pos3_x == -params.lat_a * 0.5);
        CHECK(pos3_y == -params.lat_b * 0.5);

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -1, 1});
        CHECK(pos4_x == -params.lat_a * 0.1);
        CHECK(round_to_n_decimal_places(pos4_y, 4) == -params.lat_b * 0.1 + params.lat_c * 0.1);

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -10, 1});
        CHECK(pos5_x == -params.lat_a * 0.1);
        CHECK(round_to_n_decimal_places(pos5_y, 4) == -params.lat_b + params.lat_c * 0.1);
    }

    SECTION("Special lattice constants, positive and negative cell coordinates")
    {
        const sidb_cell_clk_lyt_siqad    layout{};
        const sidb_simulation_parameters params{3, -0.32, 5.6, 5.0, 1, 2, 3};

        const auto [pos_x, pos_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, 0, 0});
        CHECK(pos_x == -0.1);
        CHECK(pos_y == 0);

        const auto [pos1_x, pos1_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, -1, 0});
        CHECK(pos1_x == 0);
        CHECK(pos1_y == -0.2);

        const auto [pos2_x, pos2_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 0, 1});
        CHECK(pos2_x == 0);
        CHECK(round_to_n_decimal_places(pos2_y, 2) == 0.3);

        const auto [pos3_x, pos3_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {0, 0, 1});
        CHECK(pos3_x == 0);
        CHECK(round_to_n_decimal_places(pos3_y, 2) == 0.3);

        const auto [pos4_x, pos4_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {10, 1, 1});
        CHECK(pos4_x == 1);
        CHECK(pos4_y == 0.5);

        const auto [pos5_x, pos5_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {-1, -1, 1});
        CHECK(pos5_x == -0.1);
        CHECK(pos5_y == 0.1);

        const auto [pos6_x, pos6_y] = sidb_nm_position<sidb_cell_clk_lyt_siqad>(params, {10, -10, 1});
        CHECK(pos6_x == 1.0);
        CHECK(round_to_n_decimal_places(pos6_y, 2) == -1.7);
    }
}
