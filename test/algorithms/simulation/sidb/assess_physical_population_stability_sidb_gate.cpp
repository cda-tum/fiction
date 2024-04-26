//
// Created by Jan Drewniok on 04.02.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability_sidb_gate.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/utils/truth_table_utils.hpp>
#include <fiction/types.hpp>

using namespace fiction;

using layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[assess-physical-population-stability]")
{
    layout lyt{};
    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({29, 7, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, 8, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({17, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 9, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("Precision of distance_corresponding_to_potential is two")
    {
        const auto params = assess_physical_population_stability_params_sidb_gate{
                assess_physical_population_stability_params{sidb_simulation_parameters{2, -0.32}, 2}};
        const auto result = assess_physical_population_stability_sidb_gate(lyt, std::vector<tt>{create_or_tt()},
                                                                           params, -1);
//        REQUIRE(result.size() == 1);
//        const auto &population_stability_detail = result[0];
//        CHECK(population_stability_detail.critical_cell == siqad::coord_t{1, 1, 0});
//        REQUIRE(population_stability_detail.transition_from_to_with_cell_and_required_pot.size() == 1);
//        CHECK(population_stability_detail.transition_from_to_with_cell_and_required_pot
//                      .at(transition_type::NEGATIVE_TO_NEUTRAL)
//                      .first == siqad::coord_t{1, 1, 0});
//        CHECK(population_stability_detail.transition_from_to_with_cell_and_required_pot
//                      .at(transition_type::NEGATIVE_TO_NEUTRAL)
//                      .second == 0.29);
//
//        REQUIRE_THAT(
//                population_stability_detail.distance_corresponding_to_potential.at(
//                        transition_type::NEGATIVE_TO_NEUTRAL),
//                Catch::Matchers::WithinAbs(0.77, 1e-5));
    }
}
