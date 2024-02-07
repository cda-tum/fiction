//
// Created by Jan Drewniok on 07.02.24.
//

#include <catch2/catch_template_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/defect_influence_of_sidb_gate_contour_tracing.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/io/write_defect_operational_domain.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

using namespace fiction;

TEST_CASE("Bestagon AND Gate influence distance function", "[defect-influence-of-sidb-gate-contour-tracing]")
{
    sidb_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({23, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 11, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({20, 14, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 13, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    const auto cube_lyt = convert_to_fiction_coordinates<sidb_cell_clk_lyt_cube>(lyt);

    const sidb_defect                              defect{sidb_defect_type::UNKNOWN, -1, 5.6, 5.0};
    const maximum_defect_influence_distance_params sim_params{defect, sidb_simulation_parameters{2, -0.32}, {20, 20}};
    const defect_influence_params params{sim_params, is_operational_params{sim_params.physical_params}};

    SECTION("input 00")
    {
        defect_influence_stats stats{};
        const auto             defect_influence_domain =
                defect_influence_of_sidb_gate_contour_tracing(cube_lyt, std::vector<tt>{create_and_tt()}, 100, params, &stats);

        write_operational_domain_params write_params{};
        write_params.operational_tag     = "1";
        write_params.non_operational_tag = "0";
        write_operational_domain(defect_influence_domain,
                                 "/Users/jandrewniok/CLionProjects/fiction_copy/fiction/experiments/test_defect_op.csv",
                                 write_params);
    }
    SECTION("input 01") {}
    SECTION("input 10") {}
    SECTION("input 11") {}
}
