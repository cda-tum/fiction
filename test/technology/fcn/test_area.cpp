/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/technology/fcn/area.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/technology/fcn/area.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Area computation for different technologies", "[area]")
{
    SECTION("QCA")
    {
        qca_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = fcn::area<qca_cell_clk_lyt>(lyt, fcn::area_params<qca::qca_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(9604.0, 0.0001));

        fcn::area_stats stats{};
        fcn::area<qca_cell_clk_lyt>(lyt, fcn::area_params<qca::qca_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(98.0, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(98.0, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(9604.0, 0.0001));

        const layouts::bounding_box_2d bb{lyt};
        const auto area_bb = fcn::area<qca_cell_clk_lyt>(bb, fcn::area_params<qca::qca_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(324.0, 0.0001));
    }

    SECTION("iNML")
    {
        inml_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = fcn::area<inml_cell_clk_lyt>(lyt, fcn::area_params<inml::inml_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(174000.0, 0.0001));

        fcn::area_stats stats{};
        fcn::area<inml_cell_clk_lyt>(lyt, fcn::area_params<inml::inml_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(290.0, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(600.0, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(174000.0, 0.0001));

        const layouts::bounding_box_2d bb{lyt};
        const auto area_bb = fcn::area<inml_cell_clk_lyt>(bb, fcn::area_params<inml::inml_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(5000.0, 0.0001));
    }

    SECTION("SiDB")
    {
        sidb_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = fcn::area<sidb_cell_clk_lyt>(lyt, fcn::area_params<sidb::sidb_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(2.359296, 0.000001));

        fcn::area_stats stats{};
        fcn::area<sidb_cell_clk_lyt>(lyt, fcn::area_params<sidb::sidb_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(2.359296, 0.000001));

        const layouts::bounding_box_2d bb{lyt};
        const auto area_bb = fcn::area<sidb_cell_clk_lyt>(bb, fcn::area_params<sidb::sidb_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(0.0, 0.000001));
    }
}
