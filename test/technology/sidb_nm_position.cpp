//
// Created by Jan Drewniok on 25.07.23.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/technology/sidb_nm_position.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("SiDB position in nanometer for siqad coordinates", "[sidb-nm-position]")
{
    using namespace Catch::Matchers;

    using lattice = sidb_100_cell_clk_lyt_siqad;

    SECTION("Cell-level layout without lattice layout")
    {
        const auto [pos_x, pos_y] = sidb_nm_position<lattice>({1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice>({0, 1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice>({0, 8, 1});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(sidb_100_lattice::LAT_B * 0.8 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice>({1, 1});
        CHECK_THAT(pos4_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice>({1, 1, 1});
        CHECK_THAT(pos5_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<lattice>({1, 10, 1});
        CHECK_THAT(pos6_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(sidb_100_lattice::LAT_B + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));
    }

    SECTION("Default lattice constants, positive cell coordinates")
    {
        const auto [pos_x, pos_y] = sidb_nm_position<lattice>({1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice>({0, 1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice>({0, 8, 1});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(sidb_100_lattice::LAT_B * 0.8 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice>({1, 1});
        CHECK_THAT(pos4_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice>({1, 1, 1});
        CHECK_THAT(pos5_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<lattice>({1, 10, 1});
        CHECK_THAT(pos6_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(sidb_100_lattice::LAT_B + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));
    }

    SECTION("Default (Si-100) lattice constants, negative cell coordinates")
    {
        const auto [pos_x, pos_y] = sidb_nm_position<lattice>({-1, 0, 0});
        CHECK_THAT(pos_x, WithinAbs(-0.384, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.0, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice>({0, -1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(-0.768, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice>({-5, -5});
        CHECK_THAT(pos3_x, WithinAbs(-0.384 * 5, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(-0.768 * 5, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice>({1, 1, 0});
        CHECK_THAT(pos4_x, WithinAbs(0.384, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(0.768, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice>({1, 1, 1});
        CHECK_THAT(pos5_x, WithinAbs(0.384, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(0.993, 1E-5));
    }

    SECTION("Si-111 lattice constants")
    {
        using lattice_111 = sidb_111_cell_clk_lyt_siqad;

        const auto [pos0_x, pos0_y] = sidb_nm_position<lattice_111>({0, 0, 0});
        CHECK_THAT(pos0_x, WithinAbs(0, 1E-5));
        CHECK_THAT(pos0_y, WithinAbs(0, 1E-5));

        const auto [pos_x, pos_y] = sidb_nm_position<lattice_111>({0, 0, 1});
        CHECK_THAT(pos_x, WithinAbs(0.33255, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.19200, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice_111>({0, -1, 0});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(-0.384, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice_111>({-5, -5});
        CHECK_THAT(pos3_x, WithinAbs(-6.65 * 0.5, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(-3.84 * 0.5, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice_111>({-1, 0, 0});
        CHECK_THAT(pos4_x, WithinAbs(-0.665, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(0.0, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice_111>({1, 1, 1});
        CHECK_THAT(pos5_x, WithinAbs(0.99755, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(0.576, 1E-5));
    }
}

TEST_CASE("SiDB position in nanometer for fiction coordinates", "[sidb-nm-position]")
{
    using namespace Catch::Matchers;

    using lattice = sidb_100_cell_clk_lyt_cube;

    SECTION("Default lattice constants, positive cell coordinates")
    {
        const auto [pos_x, pos_y] = sidb_nm_position<lattice>({1, 0});
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice>({0, 2});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice>({0, 17});
        CHECK_THAT(pos3_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(sidb_100_lattice::LAT_B * 0.8 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice>({1, 2});
        CHECK_THAT(pos4_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice>({1, 3});
        CHECK_THAT(pos5_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<lattice>({1, 21});
        CHECK_THAT(pos6_x, WithinAbs(sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(sidb_100_lattice::LAT_B + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));
    }

    SECTION("Default lattice constants, negative cell coordinates")
    {
        const auto [pos_x, pos_y] = sidb_nm_position<lattice>({-1, 0});
        CHECK_THAT(pos_x, WithinAbs(-sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos_y, WithinAbs(0.0, 1E-5));

        const auto [pos2_x, pos2_y] = sidb_nm_position<lattice>({0, -2});
        CHECK_THAT(pos2_x, WithinAbs(0.0, 1E-5));
        CHECK_THAT(pos2_y, WithinAbs(-sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos3_x, pos3_y] = sidb_nm_position<lattice>({-5, -10});
        CHECK_THAT(pos3_x, WithinAbs(-sidb_100_lattice::LAT_A * 0.5, 1E-5));
        CHECK_THAT(pos3_y, WithinAbs(-sidb_100_lattice::LAT_B * 0.5, 1E-5));

        const auto [pos4_x, pos4_y] = sidb_nm_position<lattice>({-1, -3});
        CHECK_THAT(pos4_x, WithinAbs(-sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos4_y, WithinAbs(-2 * sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos5_x, pos5_y] = sidb_nm_position<lattice>({-1, -21});
        CHECK_THAT(pos5_x, WithinAbs(-sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos5_y, WithinAbs(-11 * sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));

        const auto [pos6_x, pos6_y] = sidb_nm_position<lattice>({-1, -2});
        CHECK_THAT(pos6_x, WithinAbs(-sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos6_y, WithinAbs(-sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos7_x, pos7_y] = sidb_nm_position<lattice>({-2, -4});
        CHECK_THAT(pos7_x, WithinAbs(-2 * sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos7_y, WithinAbs(-2 * sidb_100_lattice::LAT_B * 0.1, 1E-5));

        const auto [pos8_x, pos8_y] = sidb_nm_position<lattice>({-2, -1});
        CHECK_THAT(pos8_x, WithinAbs(-2 * sidb_100_lattice::LAT_A * 0.1, 1E-5));
        CHECK_THAT(pos8_y, WithinAbs(-sidb_100_lattice::LAT_B * 0.1 + sidb_100_lattice::LAT_C.second * 0.1, 1E-5));
    }
}
