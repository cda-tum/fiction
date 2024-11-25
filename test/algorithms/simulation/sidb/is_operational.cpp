//
// Created by Jan Drewniok on 11.09.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/iter/bdl_input_iterator.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <cstdint>
// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
#include <optional>
#endif
#include <set>
#include <vector>

using namespace fiction;

TEST_CASE("SiQAD OR gate", "[is-operational]")
{
    const auto layout_or_gate = blueprints::siqad_or_gate<sidb_cell_clk_lyt_siqad>();

    const sidb_100_cell_clk_lyt_siqad lat{layout_or_gate};

    CHECK(is_operational(
              lat, std::vector<tt>{create_or_tt()},
              is_operational_params{sidb_simulation_parameters{2, -0.28}, sidb_simulation_engine::QUICKEXACT,
                                    bdl_input_iterator_params{
                                        detect_bdl_wires_params{1.5},
                                        bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED},
                                    operational_condition::REJECT_KINKS})
              .first == operational_status::NON_OPERATIONAL);

    // determine if kinks induce layout to become non-operational.
    const auto kink_induced_non_operational = is_kink_induced_non_operational(
        lat, std::vector<tt>{create_or_tt()},
        is_operational_params{
            sidb_simulation_parameters{2, -0.28}, sidb_simulation_engine::QUICKEXACT,
            bdl_input_iterator_params{detect_bdl_wires_params{1.5},
                                      bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED},
            operational_condition::REJECT_KINKS});
    CHECK(kink_induced_non_operational);

    // determine input patterns for which kinks induce layout to become non-operational.
    const auto kink_induced_non_operational_input_pattern = kink_induced_non_operational_input_patterns(
        lat, std::vector<tt>{create_or_tt()},
        is_operational_params{
            sidb_simulation_parameters{2, -0.28}, sidb_simulation_engine::QUICKEXACT,
            bdl_input_iterator_params{detect_bdl_wires_params{1.5},
                                      bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED},
            operational_condition::REJECT_KINKS});

    CHECK(kink_induced_non_operational_input_pattern.size() == 1);

    CHECK(is_operational(
              lat, std::vector<tt>{create_or_tt()},
              is_operational_params{sidb_simulation_parameters{2, -0.28}, sidb_simulation_engine::QUICKEXACT,
                                    bdl_input_iterator_params{
                                        detect_bdl_wires_params{1.5},
                                        bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED},
                                    operational_condition::TOLERATE_KINKS})
              .first == operational_status::OPERATIONAL);
}

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
    SECTION("Count the number of non-operational input combinations, accepting kinks")
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

TEMPLATE_TEST_CASE("AND gate on the H-Si(111)-1x1 surface", "[is-operational]", sidb_111_cell_clk_lyt_siqad)
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

TEST_CASE(
    "AND gate with bestagon structure and kink state on right input wire for input 01 and left input wire for input 10",
    "[is-operational]")
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
                                                   sidb_simulation_engine::QUICKEXACT, bdl_input_iterator_params{},
                                                   operational_condition::REJECT_KINKS})
                  .first == operational_status::NON_OPERATIONAL);
    }
    SECTION("check if is_kink_induced_non_operational return true")
    {
        // check if the function works correctly even if the parameter is wrong (kinks are accepted).
        CHECK(is_kink_induced_non_operational(
            lyt, std::vector<tt>{create_and_tt()},
            is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                                  bdl_input_iterator_params{}, operational_condition::TOLERATE_KINKS}));
    }

    SECTION("check input patterns for which kinks induce the layout to become non-operational")
    {
        CHECK(kink_induced_non_operational_input_patterns(
                  lyt, std::vector<tt>{create_and_tt()},
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                                        bdl_input_iterator_params{}, operational_condition::TOLERATE_KINKS}) ==
              std::set<uint64_t>{1, 2});
    }
}

TEST_CASE("BDL wire", "[is-operational]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{24, 0}, "BDL wire"};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({3, 0, 0}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({20, 0, 0}, sidb_technology::cell_type::OUTPUT);

    // output perturber
    lyt.assign_cell_type({24, 0, 0}, sidb_technology::cell_type::NORMAL);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb_simulation_parameters sim_params{};

    sim_params.base = 2;

    const is_operational_params params{sim_params};

    CHECK(is_operational(lyt, std::vector<tt>{create_id_tt()}, params).first == operational_status::OPERATIONAL);
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("flipped CX bestagon gate", "[is-operational]")
{
    const auto lyt = blueprints::crossing_bestagon_shape_input_down_output_up<sidb_cell_clk_lyt_siqad>();

    CHECK(is_operational(lyt, create_crossing_wire_tt(),
                         is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                                               bdl_input_iterator_params{}, operational_condition::REJECT_KINKS})
              .first == operational_status::OPERATIONAL);

    const auto kink_induced_non_operational_input_pattern = kink_induced_non_operational_input_patterns(
        lyt, create_crossing_wire_tt(),
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, operational_condition::REJECT_KINKS});

    CHECK(kink_induced_non_operational_input_pattern.empty());

    const auto kink_induced_non_operational = is_kink_induced_non_operational(
        lyt, create_crossing_wire_tt(),
        is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT,
                              bdl_input_iterator_params{}, operational_condition::REJECT_KINKS});

    CHECK(!kink_induced_non_operational);
}

TEST_CASE("is operational check for Bestagon CX gate", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_crossing<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 29);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    SECTION("without predetermined wires")
    {
        CHECK(is_operational(
                  lat, create_crossing_wire_tt(),
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::OPERATIONAL);
        CHECK(is_operational(
                  lat, create_crossing_wire_tt(),
                  is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT})
                  .first == operational_status::NON_OPERATIONAL);
    }

    SECTION("using predetermined wires")
    {
        const auto input_bdl_wires  = detect_bdl_wires(lat, detect_bdl_wires_params{}, bdl_wire_selection::INPUT);
        const auto output_bdl_wires = detect_bdl_wires(lat, detect_bdl_wires_params{}, bdl_wire_selection::OUTPUT);

        CHECK(is_operational(
                  lat, create_crossing_wire_tt(),
                  is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT},
                  std::optional{input_bdl_wires}, std::optional{output_bdl_wires})
                  .first == operational_status::OPERATIONAL);
        CHECK(is_operational(
                  lat, create_crossing_wire_tt(),
                  is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT},
                  std::optional{input_bdl_wires}, std::optional{output_bdl_wires})
                  .first == operational_status::NON_OPERATIONAL);
        CHECK(!is_kink_induced_non_operational(
            lat, create_crossing_wire_tt(),
            is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT},
            std::optional{input_bdl_wires}, std::optional{output_bdl_wires}));
    }
}

TEST_CASE("is operational check for Bestagon double wire", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_double_wire<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 30);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(
        is_operational(lat, create_double_wire_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::OPERATIONAL);
    CHECK(
        is_operational(lat, create_double_wire_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.30}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::NON_OPERATIONAL);
}

TEST_CASE("is operational check for Bestagon half adder", "[is-operational], [quality]")
{
    const auto lyt = blueprints::bestagon_ha<sidb_cell_clk_lyt_siqad>();

    CHECK(lyt.num_cells() == 26);

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    CHECK(
        is_operational(lat, create_half_adder_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.32}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::OPERATIONAL);
    CHECK(
        is_operational(lat, create_half_adder_tt(),
                       is_operational_params{sidb_simulation_parameters{2, -0.25}, sidb_simulation_engine::QUICKEXACT})
            .first == operational_status::NON_OPERATIONAL);
}
#endif
