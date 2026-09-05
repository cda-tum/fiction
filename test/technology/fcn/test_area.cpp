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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/technology/fcn/area.hpp>
#include <fiction/technology/inml/technology.hpp>
#include <fiction/technology/qca/technology.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/defect.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>

using namespace fiction;
using namespace fiction::fcn;
using namespace fiction::inml;
using namespace fiction::layouts;
using namespace fiction::qca;
using namespace fiction::sidb;

TEST_CASE("Area computation for different technologies", "[area]")
{
    SECTION("QCA")
    {
        const qca_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = area<qca_cell_clk_lyt>(lyt, area_params<qca_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(9604.0, 0.0001));

        area_stats stats{};
        area<qca_cell_clk_lyt>(lyt, area_params<qca_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(98.0, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(98.0, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(9604.0, 0.0001));

        const bounding_box_2d bb{lyt};
        const auto            area_bb = area<qca_cell_clk_lyt>(bb, area_params<qca_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(324.0, 0.0001));
    }

    SECTION("iNML")
    {
        const inml_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = area<inml_cell_clk_lyt>(lyt, area_params<inml_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(174000.0, 0.0001));

        area_stats stats{};
        area<inml_cell_clk_lyt>(lyt, area_params<inml_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(290.0, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(600.0, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(174000.0, 0.0001));

        const bounding_box_2d bb{lyt};
        const auto            area_bb = area<inml_cell_clk_lyt>(bb, area_params<inml_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(5000.0, 0.0001));
    }

    SECTION("SiDB")
    {
        const sidb_cell_clk_lyt lyt{{4, 4}};

        const auto area_nm2 = area<sidb_cell_clk_lyt>(lyt, area_params<sidb_technology>{});
        CHECK_THAT(area_nm2, Catch::Matchers::WithinAbs(2.359296, 0.000001));

        area_stats stats{};
        area<sidb_cell_clk_lyt>(lyt, area_params<sidb_technology>{}, &stats);

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(2.359296, 0.000001));

        const bounding_box_2d bb{lyt};
        const auto            area_bb = area<sidb_cell_clk_lyt>(bb, area_params<sidb_technology>{});
        CHECK_THAT(area_bb, Catch::Matchers::WithinAbs(0.0, 0.000001));
    }
    SECTION("SiDB layout over a crystal lattice")
    {
        layout lyt{};

        CHECK_THAT(area(lyt), Catch::Matchers::WithinAbs(0.0, 0.000001));

        // four columns and four single-SiDB rows span the same box as the 5 x 5 cell-level layout above
        lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::NORMAL);
        lyt.assign_cell_type({4, 2, 0}, sidb_technology::cell_type::NORMAL);

        area_stats stats{};
        CHECK_THAT(area(lyt, area_params<sidb_technology>{}, &stats), Catch::Matchers::WithinAbs(2.359296, 0.000001));

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.area, Catch::Matchers::WithinAbs(2.359296, 0.000001));

        // defects extend the bounding box
        lyt.assign_defect({4, 4, 1}, model::defect{model::defect_type::DB, -1});
        CHECK_THAT(area(lyt, area_params<sidb_technology>{}, &stats), Catch::Matchers::WithinAbs(5.308416, 0.000001));

        CHECK_THAT(stats.width, Catch::Matchers::WithinAbs(1.536, 0.0001));
        CHECK_THAT(stats.height, Catch::Matchers::WithinAbs(3.456, 0.0001));
    }
}
