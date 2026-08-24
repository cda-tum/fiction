//
// Created by Jan Drewniok on 11.09.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/networks/utils/truth_table_utils.hpp>
#include <fiction/technology/fcn/cell_technologies.hpp>
#include <fiction/technology/sidb/model/defects.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/types.hpp>

#include <cstdint>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

using namespace fiction;

TEST_CASE("SiQAD OR gate", "[is-operational]")
{
    const auto or_gate = blueprints::siqad_or_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{or_gate};

    auto op_params = sidb::simulation::logic::is_operational_params{
        .sim_params = sidb::model::simulation_parameters{2, -0.32},
        .sim_engine = sidb::simulation::engine::QUICKEXACT,
        .input_bdl_iterator_params =
            sidb::simulation::logic::bdl_input_iterator_params{
                .bdl_wire_params = sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 1.5},
                .input_bdl_config = sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::
                    PERTURBER_ABSENCE_ENCODED},
        .op_condition = sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS};

    SECTION("determine if layout is operational, tolerate kinks")
    {
        CHECK(sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params)
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }

    // from now on, we will reject kinks
    op_params.op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;

    SECTION("determine if layout is operational, accept kinks")
    {
        CHECK(sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params)
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

    SECTION("determine if kinks induce layout to become non-operational")
    {
        const auto kink_induced_non_operational = sidb::simulation::logic::is_kink_induced_non_operational(
            lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params);
        CHECK(kink_induced_non_operational);
    }

    const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 1.5},
        sidb::simulation::logic::bdl_wire_selection::INPUT);
    const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 1.5},
        sidb::simulation::logic::bdl_wire_selection::OUTPUT);

    REQUIRE(input_wires.size() == 2);

    CHECK(input_wires[0].pairs.size() == 2);
    CHECK(input_wires[1].pairs.size() == 2);

    CHECK(output_wires.size() == 1);

    SECTION("use pre-determined I/O pins")
    {
        CHECK(sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_and_tt()}, op_params,
                                                      input_wires, output_wires)
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

    SECTION("determine if kinks induce layout to become non-operational")
    {
        CHECK(sidb::simulation::logic::is_kink_induced_non_operational(
            lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params, input_wires, output_wires));
    }

    SECTION("determine input patterns for which kinks induce layout to become non-operational")
    {
        const auto kink_induced_non_operational_input_pattern =
            sidb::simulation::logic::kink_induced_non_operational_input_patterns(
                lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params);

        CHECK(kink_induced_non_operational_input_pattern.size() == 1);

        op_params.op_condition = sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS;
        CHECK(sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_or_tt()}, op_params)
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }
}

TEST_CASE("Test is_physical_validity_feasible for empty canvas", "[is-operational]")
{
    const auto lyt = blueprints::two_input_two_output_bestagon_skeleton<sidb_cell_clk_lyt_siqad>();

    const auto op_params = sidb::simulation::logic::is_operational_params{
        .sim_params                = sidb::model::simulation_parameters{2, -0.32},
        .sim_engine                = sidb::simulation::engine::QUICKEXACT,
        .input_bdl_iterator_params = sidb::simulation::logic::bdl_input_iterator_params{},
        .op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS,
        .strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION};

    CHECK(sidb::simulation::logic::is_operational(lyt, networks::utils::create_crossing_wire_tt(), op_params).first ==
          sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}

TEST_CASE("SiQAD NAND gate", "[is-operational]")
{
    const auto nand_gate = blueprints::siqad_nand_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{nand_gate};

    auto op_params = sidb::simulation::logic::is_operational_params{
        .sim_params = sidb::model::simulation_parameters{2, -0.28},
        .sim_engine = sidb::simulation::engine::QUICKEXACT,
        .input_bdl_iterator_params =
            sidb::simulation::logic::bdl_input_iterator_params{
                .bdl_wire_params = sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 1.5},
                .input_bdl_config = sidb::simulation::logic::bdl_input_iterator_params::input_bdl_configuration::
                    PERTURBER_ABSENCE_ENCODED},
        .op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS,
        .strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION};

    SECTION("Pruning and simulation")
    {
        CHECK(
            sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_nand_tt()}, op_params)
                .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }
    SECTION("only pruning")
    {
        op_params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY;
        CHECK(
            sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_nand_tt()}, op_params)
                .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }

    const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 2.0},
        sidb::simulation::logic::bdl_wire_selection::INPUT);
    const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, sidb::simulation::logic::detect_bdl_wires_params{.threshold_bdl_interdistance = 2.0},
        sidb::simulation::logic::bdl_wire_selection::OUTPUT);

    sidb_100_cell_clk_lyt_siqad canvas_lyt{};
    canvas_lyt.assign_cell_type({10, 4, 1}, sidb::technology::cell_type::NORMAL);
    canvas_lyt.assign_cell_type({10, 5, 1}, sidb::technology::cell_type::NORMAL);

    SECTION("use pre-determined I/O pins")
    {
        CHECK(sidb::simulation::logic::is_operational(lat, std::vector<tt>{networks::utils::create_nand_tt()},
                                                      op_params, input_wires, output_wires, std::optional{canvas_lyt})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }
}

TEST_CASE("SiQAD's AND gate with input BDL pairs of different size", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 1}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({19, 1, 1}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb::technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb::technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb::technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(sidb::simulation::logic::is_operational(
              lat, std::vector<tt>{networks::utils::create_and_tt()},
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.28}})
              .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    CHECK(sidb::simulation::logic::is_operational(
              lat, std::vector<tt>{networks::utils::create_and_tt()},
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.1}})
              .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}

TEST_CASE("Bestagon FO2 gate", "[is-operational]")
{
    const auto lyt = blueprints::bestagon_fo2<sidb_cell_clk_lyt_siqad>();

    SECTION("using QuickExact")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

    SECTION("using QuickSim")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKSIM})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKSIM})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

#if (FICTION_ALGLIB_ENABLED)

    SECTION("using ClusterComplete")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{3, -0.32},
                                                                 sidb::simulation::engine::CLUSTERCOMPLETE})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_fan_out_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{3, -0.30},
                                                                 sidb::simulation::engine::CLUSTERCOMPLETE})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

#endif  // FICTION_ALGLIB_ENABLED
}

TEST_CASE("Bestagon CROSSING gate", "[is-operational]")
{
    const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 29);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_crossing_wire_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_crossing_wire_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}

TEST_CASE("Bestagon AND gate", "[is-operational]")
{
    auto lyt = blueprints::bestagon_and<sidb_defect_cell_clk_lyt_siqad>();

    const sidb::model::simulation_parameters params{2, -0.32};

    SECTION("Without defects")
    {
        CHECK(lyt.num_cells() == 23);

        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }
    SECTION("With defects")
    {
        lyt.assign_defect(
            {3, 16, 1}, sidb::model::defect{sidb::model::defect_type::UNKNOWN, -1, params.epsilon_r, params.lambda_tf});
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{params, sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);

        // move defect one to the right
        lyt.move_sidb_defect({3, 16, 1}, {4, 16, 1});
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{params, sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);

        // move defect one to the right
        lyt.move_sidb_defect({4, 16, 1}, {5, 16, 1});
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{params, sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }
    SECTION("Check operation for different values of mu")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }
    SECTION("Count the number of non-operational input combinations, accepting kinks")
    {
        const auto op_inputs = sidb::simulation::logic::operational_input_patterns(
            lyt, std::vector<tt>{networks::utils::create_and_tt()},
            sidb::simulation::logic::is_operational_params{.sim_params = sidb::model::simulation_parameters{2, -0.30},
                                                           .sim_engine = sidb::simulation::engine::QUICKEXACT});
        CHECK(op_inputs.size() == 1);
        CHECK(op_inputs == std::set<uint64_t>{3});
    }
}

TEST_CASE("SiQAD AND gate", "[is-operational]")
{
    auto lyt = blueprints::siqad_and_gate<sidb_defect_cell_clk_lyt_siqad>();

    sidb::model::simulation_parameters params{2, -0.28};

    SECTION("check in regime with positive charges")
    {
        SECTION("base 2")
        {
            params.base      = 2;
            params.epsilon_r = 1.7;
            params.lambda_tf = 6.9;
            CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_and_tt()},
                                                          sidb::simulation::logic::is_operational_params{params})
                      .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
            CHECK(sidb::simulation::logic::operational_input_patterns(
                      lyt, std::vector<tt>{networks::utils::create_and_tt()},
                      sidb::simulation::logic::is_operational_params{params})
                      .empty());

            params.epsilon_r = 3.5;
            params.lambda_tf = 5.9;
            CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_and_tt()},
                                                          sidb::simulation::logic::is_operational_params{params})
                      .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
            CHECK(sidb::simulation::logic::operational_input_patterns(
                      lyt, std::vector<tt>{networks::utils::create_and_tt()},
                      sidb::simulation::logic::is_operational_params{params})
                      .empty());
        }
        SECTION("base 3")
        {
            params.base      = 3;
            params.epsilon_r = 1.7;
            params.lambda_tf = 6.9;
            CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_and_tt()},
                                                          sidb::simulation::logic::is_operational_params{params})
                      .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
            CHECK(sidb::simulation::logic::operational_input_patterns(
                      lyt, std::vector<tt>{networks::utils::create_and_tt()},
                      sidb::simulation::logic::is_operational_params{params})
                      .empty());

            params.epsilon_r = 3.5;
            params.lambda_tf = 5.9;
            CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_and_tt()},
                                                          sidb::simulation::logic::is_operational_params{params})
                      .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
            CHECK(sidb::simulation::logic::operational_input_patterns(
                      lyt, std::vector<tt>{networks::utils::create_and_tt()},
                      sidb::simulation::logic::is_operational_params{params})
                      .size() == 2);
        }
    }
}

TEST_CASE("Not working diagonal Wire", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb::technology::cell_type::NORMAL);

    // canvas SiDB
    lyt.assign_cell_type({14, 6, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 15, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb::technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb::technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb::technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(sidb::simulation::logic::is_operational(
              lat, std::vector<tt>{networks::utils::create_id_tt()},
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}

TEMPLATE_TEST_CASE("AND gate on the H-Si(111)-1x1 surface", "[is-operational]", sidb_111_cell_clk_lyt_siqad,
                   cds_sidb_111_cell_clk_lyt_siqad)
{
    const auto lyt = blueprints::and_gate_111<TestType>();

    SECTION("check operation for different values of mu")
    {
        const auto op_inputs = sidb::simulation::logic::operational_input_patterns(
            lyt, std::vector<tt>{networks::utils::create_and_tt()},
            sidb::simulation::logic::is_operational_params{.sim_params = sidb::model::simulation_parameters{2, -0.32},
                                                           .sim_engine = sidb::simulation::engine::QUICKEXACT});
        CHECK(op_inputs.size() == 4);
        CHECK(op_inputs == std::set<uint64_t>{0, 1, 2, 3});
    }
    SECTION("count the number of non-operational input combinations")
    {
        const auto op_inputs = sidb::simulation::logic::operational_input_patterns(
            lyt, std::vector<tt>{networks::utils::create_and_tt()},
            sidb::simulation::logic::is_operational_params{.sim_params = sidb::model::simulation_parameters{2, -0.30},
                                                           .sim_engine = sidb::simulation::engine::QUICKEXACT});
        CHECK(op_inputs.size() == 2);
        CHECK(op_inputs == std::set<uint64_t>{0, 3});
    }

    SECTION("verify the operational status of the AND gate, which is mirrored on the x-axis. Note that the input BDL "
            "pairs are located at the bottom, while the output BDL pairs are at the top.")
    {
        const auto lyt_mirrored_x = blueprints::and_gate_111_mirrored_on_the_x_axis<TestType>();
        const auto op_inputs      = sidb::simulation::logic::operational_input_patterns(
            lyt_mirrored_x, std::vector<tt>{networks::utils::create_and_tt()},
            sidb::simulation::logic::is_operational_params{.sim_params = sidb::model::simulation_parameters{2, -0.32},
                                                           .sim_engine = sidb::simulation::engine::QUICKEXACT});
        CHECK(op_inputs.size() == 4);
        CHECK(op_inputs == std::set<uint64_t>{0, 1, 2, 3});
    }
}

TEST_CASE(
    "AND gate with Bestagon structure and kink state on right input wire for input 01 and left input wire for input 10",
    "[is-operational]")
{
    const auto lyt = blueprints::and_gate_with_kink_states<sidb_cell_clk_lyt_siqad>();

    SECTION("allow kink states")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32}})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }
    SECTION("reject kink states")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{
                      sidb::model::simulation_parameters{2, -0.32}, sidb::simulation::engine::QUICKEXACT,
                      sidb::simulation::logic::bdl_input_iterator_params{},
                      sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }
    SECTION("check if is_kink_induced_non_operational returns true")
    {
        // check if the function works correctly even if the parameter is wrong (kinks are accepted).
        CHECK(sidb::simulation::logic::is_kink_induced_non_operational(
            lyt, std::vector<tt>{networks::utils::create_and_tt()},
            sidb::simulation::logic::is_operational_params{
                sidb::model::simulation_parameters{2, -0.32}, sidb::simulation::engine::QUICKEXACT,
                sidb::simulation::logic::bdl_input_iterator_params{},
                sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS}));
    }

    SECTION("check input patterns for which kinks induce the layout to become non-operational")
    {
        CHECK(sidb::simulation::logic::kink_induced_non_operational_input_patterns(
                  lyt, std::vector<tt>{networks::utils::create_and_tt()},
                  sidb::simulation::logic::is_operational_params{
                      sidb::model::simulation_parameters{2, -0.32}, sidb::simulation::engine::QUICKEXACT,
                      sidb::simulation::logic::bdl_input_iterator_params{},
                      sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS}) ==
              std::set<uint64_t>{1, 2});
    }
}

TEST_CASE("BDL wire", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{24, 0}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb::technology::cell_type::INPUT);
    lyt.assign_cell_type({3, 0, 0}, sidb::technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb::technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb::technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb::technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb::technology::cell_type::OUTPUT);

    // output perturber
    lyt.assign_cell_type({24, 0, 0}, sidb::technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb::model::simulation_parameters sim_params{};

    sim_params.base = 2;

    const sidb::simulation::logic::is_operational_params params{.sim_params = sim_params};

    CHECK(
        sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_id_tt()}, params).first ==
        sidb::simulation::logic::operational_status::OPERATIONAL);
}

TEST_CASE("Special wire that cannot be pruned, but is non-operational when kinks are rejected", "[is-operational]")
{
    sidb_cell_clk_lyt_siqad lyt{};

    // input wires
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    lyt.assign_cell_type({14, 5, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    // canvas SiDBs
    lyt.assign_cell_type({11, 7, 0}, sidb_cell_clk_lyt_siqad::cell_type::LOGIC);
    lyt.assign_cell_type({13, 13, 0}, sidb_cell_clk_lyt_siqad::cell_type::LOGIC);

    // output wires
    lyt.assign_cell_type({14, 15, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_cell_clk_lyt_siqad::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_cell_clk_lyt_siqad::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    sidb::model::simulation_parameters sim_params{};

    sim_params.base = 2;

    sidb::simulation::logic::is_operational_params params{.sim_params = sim_params};

    SECTION("Rejecting Kinks")
    {
        params.op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS;
        params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION;

        CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_id_tt()}, params)
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

    SECTION("Only conducting pruning and tolerating kinks")
    {
        params.op_condition = sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS;
        params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        CHECK(sidb::simulation::logic::is_operational(lyt, std::vector<tt>{networks::utils::create_id_tt()}, params)
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("flipped CX bestagon gate", "[is-operational]")
{
    const auto lyt = blueprints::crossing_bestagon_shape_input_down_output_up<sidb_cell_clk_lyt_siqad>();

    CHECK(sidb::simulation::logic::is_operational(
              lyt, networks::utils::create_crossing_wire_tt(),
              sidb::simulation::logic::is_operational_params{
                  sidb::model::simulation_parameters{2, -0.32}, sidb::simulation::engine::QUICKEXACT,
                  sidb::simulation::logic::bdl_input_iterator_params{},
                  sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
              .first == sidb::simulation::logic::operational_status::OPERATIONAL);

    const auto kink_induced_non_operational_input_pattern =
        sidb::simulation::logic::kink_induced_non_operational_input_patterns(
            lyt, networks::utils::create_crossing_wire_tt(),
            sidb::simulation::logic::is_operational_params{
                .sim_params                = sidb::model::simulation_parameters{2, -0.32},
                .sim_engine                = sidb::simulation::engine::QUICKEXACT,
                .input_bdl_iterator_params = sidb::simulation::logic::bdl_input_iterator_params{},
                .op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS});

    CHECK(kink_induced_non_operational_input_pattern.empty());

    const auto kink_induced_non_operational = sidb::simulation::logic::is_kink_induced_non_operational(
        lyt, networks::utils::create_crossing_wire_tt(),
        sidb::simulation::logic::is_operational_params{
            .sim_params                = sidb::model::simulation_parameters{2, -0.32},
            .sim_engine                = sidb::simulation::engine::QUICKEXACT,
            .input_bdl_iterator_params = sidb::simulation::logic::bdl_input_iterator_params{},
            .op_condition = sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS});

    CHECK(!kink_induced_non_operational);
}

TEST_CASE("is operational check for Bestagon CX gate", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 29);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    SECTION("without predetermined wires")
    {
        CHECK(sidb::simulation::logic::is_operational(
                  lat, networks::utils::create_crossing_wire_tt(),
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lat, networks::utils::create_crossing_wire_tt(),
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKEXACT})
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
    }

    SECTION("using predetermined wires")
    {
        const auto input_bdl_wires =
            sidb::simulation::logic::detect_bdl_wires(lat, sidb::simulation::logic::detect_bdl_wires_params{},
                                                      sidb::simulation::logic::bdl_wire_selection::INPUT);
        const auto output_bdl_wires =
            sidb::simulation::logic::detect_bdl_wires(lat, sidb::simulation::logic::detect_bdl_wires_params{},
                                                      sidb::simulation::logic::bdl_wire_selection::OUTPUT);

        CHECK(sidb::simulation::logic::is_operational(
                  lat, networks::utils::create_crossing_wire_tt(),
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                                 sidb::simulation::engine::QUICKEXACT},
                  input_bdl_wires, output_bdl_wires)
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
        CHECK(sidb::simulation::logic::is_operational(
                  lat, networks::utils::create_crossing_wire_tt(),
                  sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                                 sidb::simulation::engine::QUICKEXACT},
                  input_bdl_wires, output_bdl_wires)
                  .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
        CHECK(!sidb::simulation::logic::is_kink_induced_non_operational(
            lat, networks::utils::create_crossing_wire_tt(),
            sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                           sidb::simulation::engine::QUICKEXACT},
            input_bdl_wires, output_bdl_wires));
    }

    SECTION("using predetermined wires and only applying pruning without simulation")
    {
        const auto input_bdl_wires =
            sidb::simulation::logic::detect_bdl_wires(lat, sidb::simulation::logic::detect_bdl_wires_params{},
                                                      sidb::simulation::logic::bdl_wire_selection::INPUT);
        const auto output_bdl_wires =
            sidb::simulation::logic::detect_bdl_wires(lat, sidb::simulation::logic::detect_bdl_wires_params{},
                                                      sidb::simulation::logic::bdl_wire_selection::OUTPUT);

        auto op_params =
            sidb::simulation::logic::is_operational_params{.sim_params = sidb::model::simulation_parameters{2, -0.32}};
        op_params.strategy_to_analyze_operational_status =
            sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        CHECK(sidb::simulation::logic::is_operational(lat, networks::utils::create_crossing_wire_tt(), op_params,
                                                      input_bdl_wires, output_bdl_wires)
                  .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    }
}

TEST_CASE("is operational check for Bestagon double wire", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_double_wire<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 30);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_double_wire_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_double_wire_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.30},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}

TEST_CASE("is operational check for Bestagon half adder", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_ha<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 26);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_half_adder_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.32},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::OPERATIONAL);
    CHECK(sidb::simulation::logic::is_operational(
              lat, networks::utils::create_half_adder_tt(),
              sidb::simulation::logic::is_operational_params{sidb::model::simulation_parameters{2, -0.25},
                                                             sidb::simulation::engine::QUICKEXACT})
              .first == sidb::simulation::logic::operational_status::NON_OPERATIONAL);
}
#endif

TEST_CASE("Pre-generated input pattern layouts match the layout-based overload", "[is-operational]")
{
    const auto check_agreement =
        [](const auto& lat, const auto& spec, const sidb::simulation::logic::is_operational_params& params)
    {
        const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
            lat, params.input_bdl_iterator_params.bdl_wire_params, sidb::simulation::logic::bdl_wire_selection::INPUT);
        const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
            lat, params.input_bdl_iterator_params.bdl_wire_params, sidb::simulation::logic::bdl_wire_selection::OUTPUT);

        const auto [expected_status, expected_calls] =
            sidb::simulation::logic::is_operational(lat, spec, params, input_wires, output_wires);

        const auto input_pattern_layouts = sidb::simulation::logic::generate_bdl_input_pattern_layouts(
            lat, params.input_bdl_iterator_params, input_wires);

        const auto [status, calls] =
            sidb::simulation::logic::is_operational(input_pattern_layouts, spec, params, input_wires, output_wires);

        CHECK(status == expected_status);
        CHECK(calls == expected_calls);
    };

    SECTION("SiQAD AND gate")
    {
        const sidb_100_cell_clk_lyt_siqad lat{blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>()};

        for (const auto condition :
             {sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS,
              sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
        {
            // an operational and a non-operational parameter setting, so both the early-return and the
            // all-patterns-simulated paths are covered
            for (const auto mu_minus : {-0.32, -0.15})
            {
                sidb::simulation::logic::is_operational_params params{
                    .sim_params = sidb::model::simulation_parameters{2, mu_minus},
                    .sim_engine = sidb::simulation::engine::QUICKEXACT};
                params.op_condition = condition;

                check_agreement(lat, std::vector<tt>{networks::utils::create_and_tt()}, params);
            }
        }
    }

    SECTION("SiQAD OR gate")
    {
        const sidb_100_cell_clk_lyt_siqad lat{blueprints::siqad_or_gate<sidb_cell_clk_lyt_siqad>()};

        sidb::simulation::logic::is_operational_params params{.sim_params =
                                                                  sidb::model::simulation_parameters{2, -0.28},
                                                              .sim_engine = sidb::simulation::engine::QUICKEXACT};
        params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

        check_agreement(lat, std::vector<tt>{networks::utils::create_or_tt()}, params);
    }

    SECTION("Bestagon AND gate")
    {
        const sidb_100_cell_clk_lyt_siqad lat{blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>()};

        const sidb::simulation::logic::is_operational_params params{.sim_params =
                                                                        sidb::model::simulation_parameters{2, -0.32},
                                                                    .sim_engine = sidb::simulation::engine::QUICKEXACT};

        check_agreement(lat, std::vector<tt>{networks::utils::create_and_tt()}, params);
    }
}

TEST_CASE("Pre-generated input pattern layouts are validated", "[is-operational]")
{
    const sidb_100_cell_clk_lyt_siqad lat{blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>()};

    const sidb::simulation::logic::is_operational_params params{.sim_params =
                                                                    sidb::model::simulation_parameters{2, -0.32},
                                                                .sim_engine = sidb::simulation::engine::QUICKEXACT};

    const auto input_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, params.input_bdl_iterator_params.bdl_wire_params, sidb::simulation::logic::bdl_wire_selection::INPUT);
    const auto output_wires = sidb::simulation::logic::detect_bdl_wires(
        lat, params.input_bdl_iterator_params.bdl_wire_params, sidb::simulation::logic::bdl_wire_selection::OUTPUT);

    const auto input_pattern_layouts =
        sidb::simulation::logic::generate_bdl_input_pattern_layouts(lat, params.input_bdl_iterator_params, input_wires);

    REQUIRE(input_pattern_layouts.size() == 4);

    SECTION("Too few layouts for the specification")
    {
        const std::vector<sidb_100_cell_clk_lyt_siqad> too_few{input_pattern_layouts.cbegin(),
                                                               input_pattern_layouts.cbegin() + 2};

        CHECK_THROWS_AS(sidb::simulation::logic::is_operational(too_few,
                                                                std::vector<tt>{networks::utils::create_and_tt()},
                                                                params, input_wires, output_wires),
                        std::invalid_argument);
    }

    SECTION("Empty specification")
    {
        CHECK_THROWS_AS(sidb::simulation::logic::is_operational(input_pattern_layouts, std::vector<tt>{}, params,
                                                                input_wires, output_wires),
                        std::invalid_argument);
    }
}

TEST_CASE("Both is_operational entry points apply the same canvas rule", "[is-operational]")
{
    // a layout with canvas SiDBs, so that both entry points can build a canvas layout from its `LOGIC` cells
    sidb_cell_clk_lyt_siqad lyt{};

    // input wires
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    lyt.assign_cell_type({14, 5, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    // canvas SiDBs
    lyt.assign_cell_type({11, 7, 0}, sidb_cell_clk_lyt_siqad::cell_type::LOGIC);
    lyt.assign_cell_type({13, 13, 0}, sidb_cell_clk_lyt_siqad::cell_type::LOGIC);

    // output wires
    lyt.assign_cell_type({14, 15, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({12, 16, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    lyt.assign_cell_type({8, 17, 0}, sidb_cell_clk_lyt_siqad::cell_type::OUTPUT);
    lyt.assign_cell_type({6, 18, 0}, sidb_cell_clk_lyt_siqad::cell_type::OUTPUT);

    lyt.assign_cell_type({2, 19, 0}, sidb_cell_clk_lyt_siqad::cell_type::NORMAL);

    const auto spec = std::vector<tt>{networks::utils::create_id_tt()};

    for (const auto condition : {sidb::simulation::logic::is_operational_params::operational_condition::TOLERATE_KINKS,
                                 sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS})
    {
        for (const auto strategy :
             {sidb::simulation::logic::is_operational_params::operational_analysis_strategy::SIMULATION_ONLY,
              sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY,
              sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION})
        {
            sidb::simulation::logic::is_operational_params params{.sim_params = sidb::model::simulation_parameters{2}};
            params.op_condition                           = condition;
            params.strategy_to_analyze_operational_status = strategy;

            const auto input_wires =
                sidb::simulation::logic::detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params,
                                                          sidb::simulation::logic::bdl_wire_selection::INPUT);
            const auto output_wires =
                sidb::simulation::logic::detect_bdl_wires(lyt, params.input_bdl_iterator_params.bdl_wire_params,
                                                          sidb::simulation::logic::bdl_wire_selection::OUTPUT);

            const auto [status_without_wires, calls_without_wires] =
                sidb::simulation::logic::is_operational(lyt, spec, params);
            const auto [status_with_wires, calls_with_wires] =
                sidb::simulation::logic::is_operational(lyt, spec, params, input_wires, output_wires);

            // both entry points must take the same path, so both the verdict and the number of simulator
            // invocations must agree; the invocation count is what distinguishes the filtering path from the
            // simulation path
            CHECK(status_with_wires == status_without_wires);
            CHECK(calls_with_wires == calls_without_wires);

            // this wire cannot be pruned, so `FILTER_ONLY` combined with `REJECT_KINKS` reports it operational by
            // approximation. Every other combination reaches the simulation and finds it non-operational. Before the
            // canvas rule was unified, `FILTER_ONLY` combined with `TOLERATE_KINKS` also reported it operational
            // through the overload taking pre-detected wires: the canvas was built, so the `FILTER_ONLY` early return
            // fired, but the filtering steps that would have populated it never ran, because they require
            // `REJECT_KINKS`
            const auto expected_status =
                (strategy ==
                     sidb::simulation::logic::is_operational_params::operational_analysis_strategy::FILTER_ONLY &&
                 condition == sidb::simulation::logic::is_operational_params::operational_condition::REJECT_KINKS) ?
                    sidb::simulation::logic::operational_status::OPERATIONAL :
                    sidb::simulation::logic::operational_status::NON_OPERATIONAL;

            CHECK(status_with_wires == expected_status);
        }
    }
}
