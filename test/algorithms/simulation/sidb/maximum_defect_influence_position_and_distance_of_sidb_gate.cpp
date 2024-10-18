//
// Created by Jan Drewniok on 26.06.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/path_finding/distance.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance_of_sidb_gate.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/math_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

using namespace fiction;

TEST_CASE("Test influence distance function for Gate")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    const maximum_defect_influence_position_and_distance_of_sidb_gate_params defect_avoidance_params_vacancy{
        maximum_defect_influence_position_and_distance_of_sidb_gate_params{
            {sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9}, sidb_simulation_parameters{2}, {10, 0}}}};

    const maximum_defect_influence_position_and_distance_of_sidb_gate_params defect_avoidance_params_arsenic{
        maximum_defect_influence_position_and_distance_of_sidb_gate_params{
            {sidb_defect{sidb_defect_type::UNKNOWN, 1, 9.7, 2.1}, sidb_simulation_parameters{2}}}};

    const auto cell_distance_vacancy = maximum_defect_influence_position_and_distance_of_sidb_gate(
        lyt, std::vector<tt>{create_or_tt()}, defect_avoidance_params_vacancy);

    const auto cell_distance_arsenic = maximum_defect_influence_position_and_distance_of_sidb_gate(
        lyt, std::vector<tt>{create_or_tt()}, defect_avoidance_params_arsenic);

    CHECK(cell_distance_vacancy.first == siqad::coord_t{-4, 3, 1});
    CHECK(cell_distance_arsenic.first == siqad::coord_t{3, 6, 0});
    CHECK_THAT(std::abs(cell_distance_vacancy.second - 2.769), Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(cell_distance_arsenic.second - 2.376), Catch::Matchers::WithinAbs(0.00, 0.01));
}
