//
// Created by Jan Drewniok on 07.02.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/defect_avoidance_distance.hpp>
#include <fiction/algorithms/simulation/sidb/defect_operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
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
    const auto lyt = blueprints::bestagon_and<sidb_cell_clk_lyt_siqad>();

    const auto cube_lyt = convert_layout_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);

    const sidb_defect sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10.6, 5.9};
    const maximum_defect_influence_position_and_distance_params sim_params{sidb_defect,
                                                                           sidb_simulation_parameters{2, -0.32}};
    defect_operational_domain_params params{sim_params, is_operational_params{sim_params.simulation_parameters}};

    SECTION("Grid Search")
    {
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto                      defect_influence_domain =
            defect_operational_domain_grid_search(cube_lyt, std::vector<tt>{create_and_tt()}, 3, params, &stats);
        CHECK(calculate_defect_clearance(cube_lyt, defect_influence_domain).minimum_defect_clearance <=
              6.2126117696183147);
    }

    SECTION("Random Sampling")
    {
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto                      defect_influence_domain =
            defect_operational_domain_random_sampling(cube_lyt, std::vector<tt>{create_and_tt()}, 100, params, &stats);
        CHECK(defect_influence_domain.operational_values.size() == 100);
        CHECK(calculate_defect_clearance(cube_lyt, defect_influence_domain).minimum_defect_clearance <= 12.579477930);
    }

    SECTION("QuickTrace")
    {
        // 6.21261176961831474 nm is the exact value.
        params.defect_influence_params.additional_scanning_area = {20, 20};
        defect_operational_domain_stats stats{};
        const auto                      defect_influence_domain =
            defect_operational_domain_quicktrace(cube_lyt, std::vector<tt>{create_and_tt()}, 5, params, &stats);
        CHECK_THAT(calculate_defect_clearance(cube_lyt, defect_influence_domain).minimum_defect_clearance,
                   Catch::Matchers::WithinAbs(6.21261176961831474, physical_constants::POP_STABILITY_ERR));
    }
}
