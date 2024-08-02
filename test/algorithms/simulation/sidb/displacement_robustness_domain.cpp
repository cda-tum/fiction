//
// Created by Jan Drewniok on 21.03.24.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/displacement_robustness_domain.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
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

    displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
    params.displacement_variations                        = {1, 1};
    params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.28};
    params.operational_params.bdl_params.maximum_distance = 2.0;
    params.operational_params.bdl_params.minimum_distance = 0.2;
    params.fixed_sidbs                                    = {{0, 0, 1},  {2, 1, 1},  {20, 0, 1}, {18, 1, 1}, {14, 3, 1},
                                                             {16, 2, 1}, {10, 7, 0}, {10, 9, 1}, {4, 2, 1}};
    params.percentage_of_analyzed_displaced_layouts       = 0.1;
    params.dimer_policy                                   = dimer_displacement_policy::ALLOW_OTHER_DIMER;

    SECTION("One displacement variation in the x- and y-directions, allow dimer change")
    {
        displacement_robustness_domain_stats stats{};

        const auto robustness_domain =
            determine_displacement_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) ==
              static_cast<std::size_t>(0.1 * std::pow(9, lyt.num_cells() - params.fixed_sidbs.size()) +
                                       1));  // +1 since the not displaced (aka original layout) is also stored
        CHECK(static_cast<std::size_t>(robustness_domain.operational_values.size()) ==
              static_cast<std::size_t>(0.1 * std::pow(9, lyt.num_cells() - params.fixed_sidbs.size()) +
                                       1));  // +1 since the not displaced (aka original layout) is also stored
        check_identical_information_of_stats_and_domain(robustness_domain, stats);
    }

    SECTION("Two displacement variations in the y-direction, disallow dimer change")
    {
        displacement_robustness_domain_stats stats{};
        params.displacement_variations                        = {0, 2};
        params.dimer_policy                                   = dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER;
        params.operational_params.bdl_params.maximum_distance = 3.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto robustness_domain =
            determine_displacement_robustness_domain(lyt, std::vector<tt>{create_and_tt()}, params, &stats);
        CHECK((stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements) <
              static_cast<std::size_t>(std::pow(9, lyt.num_cells() - params.fixed_sidbs.size())));
        CHECK(static_cast<std::size_t>(robustness_domain.operational_values.size()) <
              static_cast<std::size_t>(std::pow(9, lyt.num_cells() - params.fixed_sidbs.size())));
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
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.28};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_and_tt()}, params, 0.3);
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
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.analysis_mode =
            displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>>::displacement_analysis_mode::RANDOM;
        params.percentage_of_analyzed_displaced_layouts = 0.1;

        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_and_tt()}, params, 0.1);
        CHECK(result >= -std::numeric_limits<double>::epsilon());
    }
}

TEST_CASE("Determine the probability of fabricating an operational BDL wire with a fabrication error rate (p) of 0.1",
          "[displacement-robustness]")
{
    const auto lyt = blueprints::bdl_wire<sidb_cell_clk_lyt_siqad>();

    SECTION("one displacement variation in y-direction")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.dimer_policy                                   = dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER;
        params.analysis_mode                                  = displacement_robustness_domain_params<
            cell<sidb_cell_clk_lyt_siqad>>::displacement_analysis_mode::EXHAUSTIVE;

        // Each SiDB can show a displacement.
        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_id_tt()}, params, 1.0);

        displacement_robustness_domain_stats stats{};
        const auto                           result_displacement_domain =
            determine_displacement_robustness_domain(lyt, std::vector<tt>{create_id_tt()}, params, &stats);
        CHECK_THAT(static_cast<double>(stats.num_operational_sidb_displacements) /
                       static_cast<double>(stats.num_operational_sidb_displacements +
                                           stats.num_non_operational_sidb_displacements),
                   Catch::Matchers::WithinAbs(result, physical_constants::POP_STABILITY_ERR));

        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.625, physical_constants::POP_STABILITY_ERR));

        const auto result_20_percent_error =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_id_tt()}, params, 0.2);

        CHECK(result_20_percent_error > result);
    }

    SECTION("one displacement variation in x-direction")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.dimer_policy                                   = dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER;
        params.analysis_mode                                  = displacement_robustness_domain_params<
            cell<sidb_cell_clk_lyt_siqad>>::displacement_analysis_mode::EXHAUSTIVE;

        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_id_tt()}, params, 0.2);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.66666666666666, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("one displacement variation in x-direction, random sampling")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.00, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("fabrication error rate p = 0.0")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt_siqad>> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result =
            determine_propability_of_fabricating_operational_gate(lyt, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.0, physical_constants::POP_STABILITY_ERR));
    }
}

TEST_CASE("Determine the probability of fabricating an operational BDL, offset coordinates",
          "[displacement-robustness]")
{
    auto lyt = blueprints::bdl_wire<sidb_cell_clk_lyt_siqad>();

    const auto lyt_offset = convert_to_fiction_coordinates<sidb_cell_clk_lyt>(lyt);

    SECTION("one displacement variation in y-direction")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.dimer_policy                                   = dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER;
        params.analysis_mode =
            displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>>::displacement_analysis_mode::EXHAUSTIVE;

        // Each SiDB can show a displacement.
        const auto result = determine_propability_of_fabricating_operational_gate(
            lyt_offset, std::vector<tt>{create_id_tt()}, params, 1.0);

        displacement_robustness_domain_stats stats{};
        const auto                           result_displacement_domain =
            determine_displacement_robustness_domain(lyt_offset, std::vector<tt>{create_id_tt()}, params, &stats);
        CHECK_THAT(static_cast<double>(stats.num_operational_sidb_displacements) /
                       static_cast<double>(stats.num_operational_sidb_displacements +
                                           stats.num_non_operational_sidb_displacements),
                   Catch::Matchers::WithinAbs(result, physical_constants::POP_STABILITY_ERR));

        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.625, physical_constants::POP_STABILITY_ERR));

        const auto result_20_percent_error = determine_propability_of_fabricating_operational_gate(
            lyt_offset, std::vector<tt>{create_id_tt()}, params, 0.2);

        CHECK(result_20_percent_error > result);
    }

    SECTION("one displacement variation in x-direction")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;
        params.dimer_policy                                   = dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER;
        params.analysis_mode =
            displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>>::displacement_analysis_mode::EXHAUSTIVE;

        const auto result = determine_propability_of_fabricating_operational_gate(
            lyt_offset, std::vector<tt>{create_id_tt()}, params, 0.2);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(0.66666666666666, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("one displacement variation in x-direction, random sampling")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>> params{};
        params.displacement_variations                        = {1, 0};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result = determine_propability_of_fabricating_operational_gate(
            lyt_offset, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.00, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("fabrication error rate p = 0.0")
    {
        displacement_robustness_domain_params<cell<sidb_cell_clk_lyt>> params{};
        params.displacement_variations                        = {0, 1};
        params.operational_params.simulation_parameters       = sidb_simulation_parameters{2, -0.32};
        params.operational_params.bdl_params.maximum_distance = 2.0;
        params.operational_params.bdl_params.minimum_distance = 0.2;

        const auto result = determine_propability_of_fabricating_operational_gate(
            lyt_offset, std::vector<tt>{create_id_tt()}, params, 0.0);
        CHECK_THAT(result, Catch::Matchers::WithinAbs(1.0, physical_constants::POP_STABILITY_ERR));
    }
}
