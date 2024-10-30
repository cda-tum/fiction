//
// Created by Jan Drewniok on 07.02.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/defect_avoidance_distance.hpp>
#include <fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/write_defect_influence_operational_domain.hpp>
#include <fiction/io/write_sqd_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("novel designed AND Gate influence distance function which fails again",
          "[defect-influence-operational-domain]")
{
    sidb_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    // canvas SiDBs
    // -------------
    lyt.assign_cell_type({17, 8, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 8, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 11, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 11, 0}, sidb_technology::cell_type::NORMAL);
    // -------------

    lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    write_sqd_layout(
        lyt, "/Users/jandrewniok/CLionProjects/fiction_fork/experiments/quicktrace/plots/and/and.sqd");

    const auto cube_lyt = convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);

    const sidb_defect sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};
    const maximum_defect_influence_position_and_distance_params sim_params{sidb_defect,
                                                                           sidb_simulation_parameters{2, -0.32}};
    defect_operational_domain_params params{sim_params,
                                                      is_operational_params{sim_params.simulation_parameters}};

    SECTION("Grid Search")
    {
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto defect_influence_domain = defect_operational_domain_grid_search(
            cube_lyt, std::vector<tt>{create_or_tt()}, 1, params, &stats);
        CHECK_THAT(defect_avoidance_distance(cube_lyt, defect_influence_domain).minimum_defect_clearance,
                   Catch::Matchers::WithinAbs(12.579477930, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Random Sampling")
    {
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto defect_influence_domain = defect_operational_domain_random_sampling(
            cube_lyt, std::vector<tt>{create_or_tt()}, 100, params, &stats);
        CHECK(defect_influence_domain.operational_values.size() == 100);
        CHECK(defect_avoidance_distance(cube_lyt, defect_influence_domain).minimum_defect_clearance <= 12.579477930);
    }

    SECTION("Contour Tracing")
    {
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto defect_influence_domain =
            defect_operational_domain_quicktrace(cube_lyt, std::vector<tt>{create_or_tt()}, 5, params, &stats);
        CHECK_THAT(defect_avoidance_distance(cube_lyt, defect_influence_domain).minimum_defect_clearance,
                   Catch::Matchers::WithinAbs(12.579477930, physical_constants::POP_STABILITY_ERR));
    }
}
