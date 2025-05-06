//
// Created by Jan Drewniok on 12.09.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/physical_design/design_sidb_gates.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <thread>
#include <vector>

using namespace fiction;

TEST_CASE("Use FO2 Bestagon gate without SiDB at {17, 11, 0} and generate original one", "[design-sidb-gates]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({18, 6, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({19, 7, 0}, sidb_technology::cell_type::NORMAL);

    // canvas SiDBs
    // SiDB, originally part of the Bestagon fo2 gate, is excluded.
    // lyt.assign_cell_type({17, 11, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({21, 11, 1}, sidb_technology::cell_type::LOGIC);
    lyt.assign_cell_type({18, 13, 0}, sidb_technology::cell_type::LOGIC);
    // ----------------------------

    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("replace the output perturbers by equivalent negatively charged defects")
    {
        design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>> params{
            is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::CLUSTERCOMPLETE},
            design_sidb_gates_params<
                cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
            {{17, 11, 0}, {17, 11, 0}},
            1};

        sidb_defect_surface defect_layout{lyt};
        defect_layout.assign_cell_type({36, 19, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        defect_layout.assign_cell_type({2, 19, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({36, 19, 0}) == sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);
        CHECK(defect_layout.get_cell_type({2, 19, 0}) == sidb_100_cell_clk_lyt_siqad::cell_type::EMPTY);

        defect_layout.assign_sidb_defect({36, 19, 0},
                                         sidb_defect{sidb_defect_type::DB, -1,
                                                     params.operational_params.simulation_parameters.epsilon_r,
                                                     params.operational_params.simulation_parameters.lambda_tf});
        defect_layout.assign_sidb_defect(
            {2, 19, 0}, sidb_defect{sidb_defect_type::DB, -1, params.operational_params.simulation_parameters.epsilon_r,
                                    params.operational_params.simulation_parameters.lambda_tf});

        const auto found_gate_layouts_exhaustive =
            design_sidb_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);

        REQUIRE(found_gate_layouts_exhaustive.size() == 1);
        CHECK(found_gate_layouts_exhaustive[0].num_cells() == 19);
        CHECK(found_gate_layouts_exhaustive[0].get_cell_type({17, 11, 0}) ==
              sidb_100_cell_clk_lyt_siqad::cell_type::LOGIC);

        params.design_mode =
            design_sidb_gates_params<cell<sidb_100_cell_clk_lyt_siqad>>::design_sidb_gates_mode::QUICKCELL;

        const auto found_gate_layouts_quickcell =
            design_sidb_gates(defect_layout, std::vector<tt>{create_fan_out_tt()}, params);
        REQUIRE(found_gate_layouts_quickcell.size() == 1);
        CHECK(found_gate_layouts_quickcell[0].num_cells() == 19);
        CHECK(found_gate_layouts_quickcell[0].get_cell_type({17, 11, 0}) ==
              sidb_100_cell_clk_lyt_siqad::cell_type::LOGIC);
    }
}
