//
// Created by Jan Drewniok on 25.10.23.
//

#include "fiction/technology/is_sidb_gate_design_impossible.hpp"

#include "catch2/catch_template_test_macros.hpp"

#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("SiQAD's AND gate with input BDL pairs of different size", "[is-gate-design-impossible]")
{
    using layout = sidb_defect_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({19, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    SECTION("without defect")
    {
        CHECK(!is_sidb_gate_design_impossible(
            lyt, std::vector<tt>{create_and_tt()},
            is_sidb_gate_design_impossible_params{sidb_simulation_parameters{2, -0.28}}));
    }

    SECTION("with defect")
    {
        lyt.assign_sidb_defect({12, 6, 0}, sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10, 5});
        lyt.assign_sidb_defect({11, 6, 0}, sidb_defect{sidb_defect_type::SI_VACANCY, -1, 10, 5});
        CHECK(is_sidb_gate_design_impossible(
            lyt, std::vector<tt>{create_and_tt()},
            is_sidb_gate_design_impossible_params{sidb_simulation_parameters{2, -0.28}}));
    }
}

TEST_CASE("Bestagon CROSSING gate", "[is-gate-design-impossible]")
{
    using layout = sidb_defect_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({36, 1, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({38, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 12, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 11, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 4, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 9, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({24, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({30, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 13, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({32, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({20, 8, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({8, 17, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    SECTION("without defect")
    {
        CHECK(!is_sidb_gate_design_impossible(
            lyt, create_crossing_wire_tt(),
            is_sidb_gate_design_impossible_params{sidb_simulation_parameters{2, -0.32}}));
    }

    SECTION("with defect")
    {
        lyt.assign_sidb_defect({34, 18, 0}, sidb_defect{sidb_defect_type::SI_VACANCY, -1, 5, 5});
        lyt.assign_sidb_defect({34, 18, 1}, sidb_defect{sidb_defect_type::SI_VACANCY, -1, 5, 5});
        CHECK(is_sidb_gate_design_impossible(
            lyt, create_crossing_wire_tt(),
            is_sidb_gate_design_impossible_params{sidb_simulation_parameters{2, -0.32}}));
    }
}
