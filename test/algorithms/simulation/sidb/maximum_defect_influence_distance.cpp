//
// Created by Jan Drewniok on 26.06.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_distance.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/utils/math_utils.hpp>

#include <thread>

using namespace fiction;

TEST_CASE("Test influence distance function", "[maximum_defect_influence_distance]")
{
    SECTION("empty layout")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, 10.6, 5.9};
        maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{defect,
                                                                                     sidb_simulation_parameters{}};
        const sidb_cell_clk_lyt_siqad                                     lyt{};

        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance<sidb_cell_clk_lyt_siqad>(lyt, sim_params, &defect_stats);
        CHECK(defect_stats.maximum_defect_influence_distance == 0);
        CHECK(defect_stats.maximum_influence_defect_position == coordinate<sidb_cell_clk_lyt_siqad>());
    }

    SECTION("layout with one SiDB")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{defect,
                                                                                     sidb_simulation_parameters{},
                                                                                     {50, 6}};

        sidb_cell_clk_lyt_siqad                                           lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance(lyt, sim_params, &defect_stats);
        CHECK_THAT(round_to_n_decimal_places(defect_stats.maximum_defect_influence_distance, 6),
                   Catch::Matchers::WithinAbs(0.665060, physical_constants::POP_STABILITY_ERR));
        CHECK((((defect_stats.maximum_influence_defect_position.x == -1) &&
                (defect_stats.maximum_influence_defect_position.y == -1) &&
                (defect_stats.maximum_influence_defect_position.z == 1)) ||
               ((defect_stats.maximum_influence_defect_position.x == 1) &&
                (defect_stats.maximum_influence_defect_position.y == -1) &&
                (defect_stats.maximum_influence_defect_position.z == 1))));
    }

    SECTION("layout with one SiDB, negative defect, smaller lambda_tf")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 1};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{
            defect, sidb_simulation_parameters{}};
        sidb_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance(lyt, sim_params, &defect_stats);
        CHECK_THAT(round_to_n_decimal_places(defect_stats.maximum_defect_influence_distance, 4) -
                       round_to_n_decimal_places(sidb_nanometer_distance(lyt, {0, 0, 0}, {-1, 0, 1}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("layout with one SiDB, negative defect, large lambda_tf")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 20};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{defect,
                                                                                           sidb_simulation_parameters{},
                                                                                           {2, 2}};
        sidb_cell_clk_lyt_siqad                                                 lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance(lyt, sim_params, &defect_stats);
        CHECK_THAT(round_to_n_decimal_places(defect_stats.maximum_defect_influence_distance, 4) -
                       round_to_n_decimal_places(sidb_nanometer_distance(lyt, {0, 0, 0}, {0, 1, 0}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("layout with one pertuber and one DB pair, negative defect")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{
            defect, sidb_simulation_parameters{}};
        sidb_cell_clk_lyt_siqad lyt{};
        lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({6, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance(lyt, sim_params, &defect_stats);
        CHECK_THAT(round_to_n_decimal_places(defect_stats.maximum_defect_influence_distance, 4) -
                       round_to_n_decimal_places(sidb_nanometer_distance(lyt, {6, 0, 0}, {10, 0, 0}), 4),
                   Catch::Matchers::WithinAbs(0.0, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("QuickExact simulation of a Y-shape SiDB OR gate with input 01")
    {
        const sidb_defect defect{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r,
                                 sidb_simulation_parameters{}.lambda_tf};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params{defect,
                                                                                     sidb_simulation_parameters{}};
        sidb_cell_clk_lyt_siqad                                           lyt{};

        lyt.assign_cell_type({10, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({8, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        lyt.assign_cell_type({2, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({6, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 4, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 5, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
        lyt.assign_cell_type({4, 7, 1}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats{};
        maximum_defect_influence_distance(lyt, sim_params, &defect_stats);
        CHECK(defect_stats.maximum_influence_defect_position.x == 12);
        CHECK(defect_stats.maximum_influence_defect_position.y == 4);
        CHECK(defect_stats.maximum_influence_defect_position.z == 1);

        // number of threads given by the hardware
        const sidb_defect high_screening{sidb_defect_type::UNKNOWN, -1, sidb_simulation_parameters{}.epsilon_r, 1};
        const maximum_defect_influence_distance_params<sidb_cell_clk_lyt_siqad> sim_params_high_screening{
            high_screening, sidb_simulation_parameters{}};

        maximum_defect_influence_distance_stats<sidb_cell_clk_lyt_siqad> defect_stats_high_screening{};
        maximum_defect_influence_distance(lyt, sim_params_high_screening, &defect_stats_high_screening);

        CHECK(defect_stats_high_screening.maximum_defect_influence_distance <
              defect_stats.maximum_defect_influence_distance);
    }
}
