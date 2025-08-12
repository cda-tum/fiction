//
// Created by Jan Drewniok on 13.09.24.
//

#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain_ratio.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/constants.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <vector>

using namespace fiction;

TEST_CASE("BDL wire operational domain computation", "[compute-operational-ratio]")
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

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 5.5;
    op_domain_params.sweep_dimensions[0].max  = 5.5;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 5.0;
    op_domain_params.sweep_dimensions[1].max  = 5.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    SECTION("Operational domain with one parameter point")
    {
        const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.32, -0.32, 0.01};

        op_domain_params.sweep_dimensions.push_back(z_dimension);

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lat, std::vector<tt>{create_id_tt()},
                                                              parameter_point({5.5, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size (1.0)
        CHECK_THAT(op_domain_ratio - 1.0, Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("semi-operational domain")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 0.5;
        op_domain_params.sweep_dimensions[0].max  = 4.25;
        op_domain_params.sweep_dimensions[0].step = 0.25;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 0.5;
        op_domain_params.sweep_dimensions[1].max  = 4.25;
        op_domain_params.sweep_dimensions[1].step = 0.25;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lat, std::vector<tt>{create_id_tt()},
                                                              parameter_point({4.25, 4.25}), op_ratio_params);

        CHECK_THAT(op_domain_ratio - (80.0 / 256.0), Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }
}

TEST_CASE("SiQAD NAND gate", "[compute-operational-ratio]")
{
    const auto lyt = blueprints::siqad_nand_gate<sidb_100_cell_clk_lyt_siqad>();

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config =
        bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;
    op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
    op_domain_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::FILTER_THEN_SIMULATION;
    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
    op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 2.0;
    op_domain_params.sweep_dimensions[0].max  = 10.0;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 2.0;
    op_domain_params.sweep_dimensions[1].max  = 10.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    operational_domain_ratio_params op_ratio_params{op_domain_params};

    // pruning and simulation to determine the operational status of the layout
    const auto op_domain_ratio_pruning_and_simulation = operational_domain_ratio(
        lyt, std::vector<tt>{create_nand_tt()}, parameter_point({5.6, 5.0, -0.28}), op_ratio_params);

    // only pruning to determine the operational status of the layout
    op_ratio_params.op_domain_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::FILTER_ONLY;

    const auto op_domain_ratio_only_pruning = operational_domain_ratio(
        lyt, std::vector<tt>{create_nand_tt()}, parameter_point({5.6, 5.0, -0.28}), op_ratio_params);

    CHECK_THAT(op_domain_ratio_pruning_and_simulation,
               Catch::Matchers::WithinAbs(0.11918914799573235, constants::ERROR_MARGIN));

    CHECK_THAT(op_domain_ratio_only_pruning, Catch::Matchers::WithinAbs(0.11918914799573235, constants::ERROR_MARGIN));
}

// to save runtime in the CI, this test is only run in RELEASE mode
#ifdef NDEBUG
TEST_CASE("Bestagon AND gate", "[compute-operational-ratio]")
{
    const auto lyt = blueprints::bestagon_and_gate<sidb_100_cell_clk_lyt_siqad>();

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.simulation_parameters = sim_params;
    op_domain_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    // set x-dimension
    op_domain_params.sweep_dimensions[0].min  = 5.0;
    op_domain_params.sweep_dimensions[0].max  = 6.0;
    op_domain_params.sweep_dimensions[0].step = 0.1;

    // set y-dimension
    op_domain_params.sweep_dimensions[1].min  = 5.0;
    op_domain_params.sweep_dimensions[1].max  = 6.0;
    op_domain_params.sweep_dimensions[1].step = 0.1;

    const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.32, -0.32, 0.01};

    SECTION("semi-operational domain")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK_THAT(op_domain_ratio - (23.0 / 121.0), Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION("semi-operational domain, reject kinks")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);
        op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK_THAT(op_domain_ratio - (23.0 / 121.0), Catch::Matchers::WithinAbs(0.0, constants::ERROR_MARGIN));
    }

    SECTION(
        "semi-operational domain, reject kinks, only pruning is used to determine the operational status of the layout")
    {
        op_domain_params.sweep_dimensions.push_back(z_dimension);
        op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const operational_domain_ratio_params op_ratio_params{op_domain_params};

        const auto op_domain_ratio = operational_domain_ratio(lyt, std::vector<tt>{create_and_tt()},
                                                              parameter_point({5.6, 5.0, -0.32}), op_ratio_params);

        // check if the operational domain has the correct size
        CHECK(op_domain_ratio >= (23.0 / 121.0));
    }
}
#endif
