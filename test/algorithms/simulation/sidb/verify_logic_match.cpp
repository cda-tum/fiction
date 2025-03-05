//
// Created by Jan Drewniok on 12.11.24.
//

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/verify_logic_match.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("Bestagon FO2 gate", "[does-charge-distribution-match-logic-for-given-input-pattern]")
{
    const auto lyt = blueprints::bestagon_fo2<sidb_cell_clk_lyt_siqad>();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    auto bii = bdl_input_iterator<sidb_cell_clk_lyt_siqad>{lyt};

    const quickexact_params<cell<sidb_cell_clk_lyt_siqad>> params{sidb_simulation_parameters{2, -0.32}};

    SECTION("The index is 2, which means that the left input is set to one and the right input is set to zero.")
    {
        bii = 2;

        const auto simulation_results = quickexact<sidb_cell_clk_lyt_siqad>(*bii, params);

        const auto gs = simulation_results.get_groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match<sidb_cell_clk_lyt_siqad>(gs.front(), is_operational_params{},
                                                          std::vector<tt>{create_fan_out_tt()}, 2, input_wires,
                                                          output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Index is 1, which means that the left input is set to zero and the right input is set to one.")
    {
        bii = 1;

        const auto simulation_results = quickexact<sidb_cell_clk_lyt_siqad>(*bii, params);

        const auto gs = simulation_results.get_groundstates();

        REQUIRE(!gs.empty());

        SECTION("Correct index")
        {
            CHECK(verify_logic_match<sidb_cell_clk_lyt_siqad>(gs.front(), is_operational_params{},
                                                              std::vector<tt>{create_fan_out_tt()}, 1, input_wires,
                                                              output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match<sidb_cell_clk_lyt_siqad>(gs.front(), is_operational_params{},
                                                              std::vector<tt>{create_fan_out_tt()}, 2, input_wires,
                                                              output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}

TEST_CASE("AND gate mirrored on the x-axis on the H-Si 111 surface",
          "[does-charge-distribution-match-logic-for-given-input-pattern]")
{
    const auto lyt = blueprints::and_gate_111_mirrored_on_the_x_axis<sidb_111_cell_clk_lyt_siqad>();

    const auto input_wires  = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
    const auto output_wires = detect_bdl_wires(lyt, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

    REQUIRE(input_wires.size() == 2);
    REQUIRE(output_wires.size() == 1);

    auto bii = bdl_input_iterator<sidb_111_cell_clk_lyt_siqad>{lyt};

    const quickexact_params<cell<sidb_111_cell_clk_lyt_siqad>> params{sidb_simulation_parameters{2, -0.32}};

    SECTION("The index is 2, which means that the left input is set to one and the right input is set to zero.")
    {
        bii = 2;

        const auto simulation_results = quickexact<sidb_111_cell_clk_lyt_siqad>(*bii, params);

        const auto gs = simulation_results.get_groundstates();

        REQUIRE(!gs.empty());

        CHECK(verify_logic_match<sidb_111_cell_clk_lyt_siqad>(gs.front(), is_operational_params{},
                                                              std::vector<tt>{create_and_tt()}, 2, input_wires,
                                                              output_wires) == operational_status::OPERATIONAL);
    }

    SECTION("Index is 1, which means that the left input is set to zero and the right input is set to one.")
    {
        bii = 1;

        const auto simulation_results = quickexact<sidb_111_cell_clk_lyt_siqad>(*bii, params);

        const auto gs = simulation_results.get_groundstates();

        REQUIRE(!gs.empty());

        is_operational_params op_params{};
        op_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        SECTION("Correct index")
        {
            CHECK(verify_logic_match<sidb_111_cell_clk_lyt_siqad>(gs.front(), op_params,
                                                                  std::vector<tt>{create_and_tt()}, 1, input_wires,
                                                                  output_wires) == operational_status::OPERATIONAL);
        }
        SECTION("Wrong input index")
        {
            CHECK(verify_logic_match<sidb_111_cell_clk_lyt_siqad>(gs.front(), op_params,
                                                                  std::vector<tt>{create_and_tt()}, 2, input_wires,
                                                                  output_wires) == operational_status::NON_OPERATIONAL);
        }
    }
}
