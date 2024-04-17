//
// Created by Jan Drewniok on 21.03.24.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/assess_displacement_robustness.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <cmath>
#include <cstdio>
#include <vector>

using namespace fiction;

TEST_CASE("assess_sidb_gate_displacement_robustness for Y-shape SiDB AND gate", "[assess-displacement-robustness]")
{
    sidb_cell_clk_lyt_siqad lyt{};
    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    SECTION("only one displacement variation, SiQAD coordinate")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.28};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        displacement_robustness_stats stats{};

        const auto result =
            determine_sidb_gate_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) ==
              static_cast<std::size_t>(std::pow(3, lyt.num_cells()) - 1));
        CHECK(static_cast<double>(result.operational_values.size()) == std::pow(3, lyt.num_cells()) - 1);
        CHECK(num_non_operational_layouts(result) == stats.num_non_operational_sidb_displacements);
        CHECK(num_operational_layouts(result) == stats.num_operational_sidb_displacements);
    }

    SECTION("only one displacement variations, SiQAD coordinate")
    {
        // TODO after merge.
    }

    SECTION("only one displacement variation")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.28};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.fixed_cells = {{0, 0, 1}, {2, 1, 1}, {20, 0, 1}, {18, 1, 1}, {14, 3, 1}, {16, 2, 1}, {10, 7, 0}};
        displacement_robustness_stats stats{};

        const auto result =
            determine_sidb_gate_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) <
              static_cast<std::size_t>(std::pow(9, lyt.num_cells() - params.fixed_cells.size())));
        CHECK(static_cast<double>(result.operational_values.size()) <
              std::pow(9, lyt.num_cells() - params.fixed_cells.size()));
        CHECK(num_non_operational_layouts(result) == stats.num_non_operational_sidb_displacements);
        CHECK(num_operational_layouts(result) == stats.num_operational_sidb_displacements);
    }
}

TEST_CASE("assess_sidb_gate_displacement_robustness for Y-shape SiDB AND gate with p = 0.1",
          "[assess-displacement-robustness]")
{
    sidb_cell_clk_lyt_siqad lyt{};
    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({20, 0, 1}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({18, 1, 1}, sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type({4, 2, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({14, 3, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({16, 2, 1}, sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({10, 7, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({10, 9, 1}, sidb_technology::cell_type::NORMAL);

    SECTION("only one displacement variation, SiQAD coordinate")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.28};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_sidb_gate_robustness_domain_p_error_rate(lyt, std::vector<tt>{create_and_tt()}, params, 0.3);
        std::cout << result << std::endl;
    }
}

TEST_CASE("assess_sidb_gate_displacement_robustness for Y-shape SiDB AND gate with p = 0.1, 111",
          "[assess-displacement-robustness]")
{
    const auto lyt = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();
    SECTION("only one displacement variation, SiQAD coordinate")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.mode                                           = sidb_gate_displacement_robustness_params<
            sidb_cell_clk_lyt_siqad>::displacement_analysis_mode::RANDOM_SAMPLING;
        params.random_sampling_percentage = 1.0;

        const auto result =
            determine_sidb_gate_robustness_domain_p_error_rate(lyt, std::vector<tt>{create_and_tt()}, params, 0.1);
        std::cout << result << std::endl;
    }
}

TEST_CASE("assess_sidb_gate_displacement_robustness forBDL wire with p = 0.1, 111", "[assess-displacement-robustness]")
{
    const auto lyt = blueprints::bdl_wire<sidb_cell_clk_lyt_siqad>();
    SECTION("only one displacement variation, SiQAD coordinate")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_sidb_gate_robustness_domain_p_error_rate(lyt, std::vector<tt>{create_id_tt()}, params, 0.2);
        std::cout << fmt::format("result:{}", result) << std::endl;
    }

    SECTION("using p = 0.0")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_sidb_gate_robustness_domain_p_error_rate(lyt, std::vector<tt>{create_id_tt()}, params, 0.0);
        std::cout << fmt::format("result:{}", result) << std::endl;
    }
}