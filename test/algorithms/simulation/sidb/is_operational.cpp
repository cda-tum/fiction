//
// Created by Jan Drewniok on 11.09.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <cstdint>
#include <set>
#include <vector>

using namespace fiction;

TEST_CASE("SiQAD's AND gate with input BDL pairs of different size", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

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

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(is_operational(lat, std::vector<tt>{create_and_tt()},
                         is_operational_params{sidb_simulation_parameters{2, -0.28}})
              .first == operational_status::OPERATIONAL);
    CHECK(is_operational(lat, std::vector<tt>{create_and_tt()},
                         is_operational_params{sidb_simulation_parameters{2, -0.1}})
              .first == operational_status::NON_OPERATIONAL);
}

TEST_CASE("Bestagon FO2 gate", "[is-operational]")
{
    const auto lyt = blueprints::bestagon_fo2<sidb_cell_clk_lyt_siqad>();

    SECTION("using QuickExact")
    {
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_fan_out_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::OPERATIONAL);
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_fan_out_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::NON_OPERATIONAL);
    }

    SECTION("using QuickSim")
    {
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_fan_out_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKSIM})
                  .first == operational_status::OPERATIONAL);
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_fan_out_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKSIM})
                  .first == operational_status::NON_OPERATIONAL);
    }
}

TEST_CASE("Bestagon CROSSING gate", "[is-operational]")
{
    const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 29);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(
        is_operational(lat, create_crossing_wire_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::OPERATIONAL);
    CHECK(
        is_operational(lat, create_crossing_wire_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::NON_OPERATIONAL);
}

TEST_CASE("Bestagon AND gate", "[is-operational]")
{
    const auto lyt = blueprints::bestagon_and<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 23);

    SECTION("Check operation for different values of mu")
    {
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_and_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::OPERATIONAL);
        CHECK(is_operational(
                  lyt, std::vector<tt>{create_and_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::NON_OPERATIONAL);
    }
    SECTION("Count the number of non-operational input combinations")
    {
        const auto op_inputs = operational_input_patterns(
            lyt, std::vector<tt>{create_and_tt()},
            is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT});
        CHECK(op_inputs.size() == 1);
        CHECK(op_inputs == std::set<uint64_t>{3});
    }
}

TEST_CASE("Not working diagonal Wire", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 2, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 5, 0}, sidb_technology::cell_type::NORMAL);

    // canvas SiDB
    lyt.assign_cell_type({14, 6, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({24, 15, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({26, 16, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({30, 17, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({32, 18, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({36, 19, 0}, sidb_technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(
        is_operational(lat, std::vector<tt>{create_id_tt()},
                       is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::NON_OPERATIONAL);
}

TEMPLATE_TEST_CASE("AND gate on the H-Si(111)-1x1 surface", "[is-operational]", sidb_111_cell_clk_lyt_siqad,
                   cds_sidb_111_cell_clk_lyt_siqad)
{
    const auto lyt = blueprints::and_gate_111<TestType>();

    SECTION("Check operation for different values of mu")
    {
        const auto op_inputs = operational_input_patterns(
            lyt, std::vector<tt>{create_and_tt()},
            is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT});
        CHECK(op_inputs.size() == 4);
        CHECK(op_inputs == std::set<uint64_t>{0, 1, 2, 3});
    }
    SECTION("Count the number of non-operational input combinations")
    {
        const auto op_inputs = operational_input_patterns(
            lyt, std::vector<tt>{create_and_tt()},
            is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT});
        CHECK(op_inputs.size() == 2);
        CHECK(op_inputs == std::set<uint64_t>{0, 3});
    }

    SECTION("Verify the operational status of the AND gate, which is mirrored on the x-axis. Note that the input BDL "
            "pairs are located at the bottom, while the output BDL pairs are at the top.")
    {
        const auto lyt_mirrored_x = blueprints::and_gate_111_mirrored_on_the_x_axis<TestType>();
        const auto op_inputs      = operational_input_patterns(
            lyt_mirrored_x, std::vector<tt>{create_and_tt()},
            is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT});
        CHECK(op_inputs.size() == 4);
        CHECK(op_inputs == std::set<uint64_t>{0, 1, 2, 3});
    }
}

TEST_CASE("AND gate with bestagon structure and kink state at right input wire for input 01", "[is-operational]")
{
    const auto lyt = blueprints::and_gate_with_kink_states<sidb_cell_clk_lyt_siqad>();

    SECTION("allow kink states")
    {
        CHECK(is_operational(lyt, std::vector<tt>{create_and_tt()},
                             is_operational_params{sidb_simulation_parameters{2, -0.32}})
                  .first == operational_status::OPERATIONAL);
    }
    SECTION("Forbid kink states")
    {
        CHECK(is_operational(lyt, std::vector<tt>{create_and_tt()},
                             is_operational_params{sidb_simulation_parameters{2, -0.32},
                                                   sidb_simulation_engine::QUICKEXACT, detect_bdl_wires_params{},
                                                   operational_condition::FORBIDDING_KINKS})
                  .first == operational_status::NON_OPERATIONAL);
    }
}

TEST_CASE("flipped CX bestagon gate", "[is-operational]")
{
    const auto lyt = blueprints::crossing_bestagon_shape_input_down_output_up<sidb_cell_clk_lyt_siqad>();

    CHECK(is_operational(lyt, create_crossing_wire_tt(),
                         is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                                               detect_bdl_wires_params{}, operational_condition::FORBIDDING_KINKS})
              .first == operational_status::OPERATIONAL);
}
