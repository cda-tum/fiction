//
// Created by marcel on 27.10.21.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/layouts/bounding_box.hpp>
#include <fiction/technology/area.hpp>
#include <fiction/types.hpp>

using namespace fiction;

TEST_CASE("Area computation for different technologies", "[area]")
{
    SECTION("QCA")
    {
        qca_cell_clk_lyt lyt{{4, 4}};

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
        inml_cell_clk_lyt lyt{{4, 4}};

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
        sidb_cell_clk_lyt lyt{{4, 4}};

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
}
