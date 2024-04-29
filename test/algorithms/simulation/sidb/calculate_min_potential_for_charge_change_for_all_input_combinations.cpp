//
// Created by Jan Drewniok on 04.02.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/calculate_min_potential_for_charge_change_for_all_input_combinations.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

using layout = sidb_cell_clk_lyt_siqad;

TEST_CASE("Single SiDB", "[assess-physical-population-stability-sidb-gate]")
{
    const auto lyt = blueprints::bestagon_and_gate<layout>();

    const auto params = calculate_min_potential_for_charge_change_for_all_input_combinations_params{
        assess_physical_population_stability_params{sidb_simulation_parameters{2, -0.32}, 2}};

    SECTION("Minimal potential required to conduct a charge change from neutral to negative")
    {
        const auto min_potential = calculate_min_potential_for_charge_change_for_all_input_combinations(
            lyt, std::vector<tt>{create_and_tt()}, params, -1);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.020652, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Minimal potential required to conduct a charge change from negative to neutral")
    {
        const auto min_potential = calculate_min_potential_for_charge_change_for_all_input_combinations(
            lyt, std::vector<tt>{create_and_tt()}, params, 1);

        CHECK_THAT(min_potential, Catch::Matchers::WithinAbs(0.087417, physical_constants::POP_STABILITY_ERR));
    }
}
