//
// Created by Jan Drewniok on 21.03.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/displacement_robustness.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <vector>

using namespace fiction;

template <typename Lyt>
void check_identical_information_of_stats_and_domain(const displacement_robustness_domain<Lyt>&  domain,
                                                     const displacement_robustness_domain_stats& stats)
{
    const auto num_operational_layouts = static_cast<std::size_t>(
        std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                      [](const auto& robust) { return robust.second == operational_status::OPERATIONAL; }));
    const auto num_non_operational_layouts = static_cast<std::size_t>(
        std::count_if(domain.operational_values.begin(), domain.operational_values.end(),
                      [](const auto& robust) { return robust.second == operational_status::NON_OPERATIONAL; }));

    CHECK(num_operational_layouts == stats.num_operational_sidb_displacements);
    CHECK(num_non_operational_layouts == stats.num_non_operational_sidb_displacements);
}

TEST_CASE("Determine the SiDB gate displacement robustness of the Y-shaped SiDB AND gate", "[displacement-robustness]")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
    params.displacement_variations                        = {1, 1};
    params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.28};
    params.operational_params.bdl_params.maximum_distance = 2.0;
    params.operational_params.bdl_params.minimum_distance = 0.2;
    params.fixed_cells = {{0, 0, 1}, {2, 1, 1}, {20, 0, 1}, {18, 1, 1}, {14, 3, 1}, {16, 2, 1}, {10, 7, 0}, {10, 9, 1}};
    params.percentage_of_analyzed_displaced_layouts = 0.1;
    params.allow_dimer_change_in_y_direction        = true;

    SECTION("One displacement variation in the x- and y-directions, allow dimer change")
    {
        displacement_robustness_domain_stats stats{};

        const auto robustness_domain =
            determine_sidb_gate_displace_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) ==
              static_cast<std::size_t>(std::pow(6, lyt.num_cells() - params.fixed_cells.size()) - 1));
        CHECK(static_cast<double>(robustness_domain.operational_values.size()) ==
              std::pow(6, lyt.num_cells() - params.fixed_cells.size()) - 1);
        check_identical_information_of_stats_and_domain(robustness_domain, stats);
    }

    SECTION("Two displacement variations in the y-direction, disallow dimer change")
    {
        displacement_robustness_domain_stats stats{};
        params.displacement_variations                        = {0, 2};
        params.allow_dimer_change_in_y_direction              = false;
        params.operational_params.bdl_params.maximum_distance = 3.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto robustness_domain =
            determine_sidb_gate_displace_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) <
              static_cast<std::size_t>(std::pow(9, lyt.num_cells() - params.fixed_cells.size())));
        CHECK(static_cast<std::size_t>(robustness_domain.operational_values.size()) <
              static_cast<std::size_t>(std::pow(9, lyt.num_cells() - params.fixed_cells.size())));
        check_identical_information_of_stats_and_domain(robustness_domain, stats);
    }
}

TEST_CASE("Determine the probability of fabricating an operational SiQAD Y-shaped AND gate with a fabrication error "
          "rate (p) of 0.3",
          "[displacement-robustness]")
{
    const auto lyt = blueprints::siqad_and_gate<sidb_cell_clk_lyt_siqad>();

    SECTION("only one displacement variation, SiQAD coordinate")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.28};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_and_tt()}, params, 0.3);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.83, 0.1));
    }
}

TEST_CASE("Determine the probability of fabricating an operational Bestagon AND gate with a fabrication error rate (p) "
          "of 0.1",
          "[displacement-robustness]")
{
    const auto lyt = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();
    SECTION("one displacement variation in x-direction")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.analysis_mode                                  = sidb_gate_displacement_robustness_params<
            sidb_cell_clk_lyt_siqad>::displacement_analysis_mode::RANDOM_SAMPLING;
        params.percentage_of_analyzed_displaced_layouts  = 0.1;
        params.percentage_of_displaced_sidb_combinations = 0.1;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_and_tt()}, params, 0.1);
        CHECK(result >= -std::numeric_limits<double>::epsilon());
    }
}

TEST_CASE("Determine the probability of fabricating an operational BDL wire with a fabrication error rate (p) of 0.1",
          "[displacement-robustness]")
{
    const auto lyt = blueprints::bdl_wire<sidb_cell_clk_lyt_siqad>();
    SECTION("one displacement variation in y-direction")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.allow_dimer_change_in_y_direction              = false;
        params.analysis_mode                                  = sidb_gate_displacement_robustness_params<
            sidb_cell_clk_lyt_siqad>::displacement_analysis_mode::ALL_POSSIBLE_DISPLACEMENTS;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_id_tt()}, params, 0.2);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.7222222, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("one displacement variation in x-direction")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.analysis_mode                                  = sidb_gate_displacement_robustness_params<
            sidb_cell_clk_lyt_siqad>::displacement_analysis_mode::RANDOM_SAMPLING;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_id_tt()}, params, 0.2);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.7222222, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("one displacement variation in x-direction, random sampling")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.percentage_of_displaced_sidb_combinations      = 0.1;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.00, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("fabrication error rate p = 0.0")
    {
        sidb_gate_displacement_robustness_params<sidb_cell_clk_lyt_siqad> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameter        = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.percentage_of_displaced_sidb_combinations      = 0.1;

        const auto result = determine_probability_of_fabricating_operational_gate_for_given_error_rate(
            lyt, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.0, physical_constants::POP_STABILITY_ERR));
    }
}
