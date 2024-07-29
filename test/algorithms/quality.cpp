//
// Created by Jan Drewniok on 29.07.24.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>
#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/determine_physically_valid_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <set>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("QuickExact simulation of a Y-shaped SiDB OR gate with input 01", "[quality]",
                   sidb_100_cell_clk_lyt_siqad, cds_sidb_100_cell_clk_lyt_siqad)
{
    SECTION("Check if QuickExact is deterministic")
    {
        TestType lyt{};

        lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

        lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

        quickexact_params<cell<TestType>> params{
            sidb_simulation_parameters{2, -0.28},
            quickexact_params<cell<TestType>>::automatic_base_number_detection::OFF};

        std::set<double>   ground_state{};
        std::set<uint64_t> charge_index{};
        for (auto i = 0; i < 100000; i++)
        {
            const auto simulation_results = quickexact<TestType>(lyt, params);
            auto&      charge_lyt_first   = simulation_results.charge_distributions.front();
            ground_state.insert(charge_lyt_first.get_system_energy());
            charge_lyt_first.charge_distribution_to_index_general();
            charge_index.insert(charge_lyt_first.get_charge_index_and_base().first);
        }
        CHECK(ground_state.size() == 1);
        CHECK(charge_index.size() == 1);
    }

    SECTION("Test case 2")
    {
        TestType lyt{};

        lyt.assign_cell_type({22, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({24, 2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 3, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 4, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 5, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({24, 6, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 7, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({24, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({22, 9, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 11, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({17, 11, 1}, TestType::cell_type::NORMAL);

        const quickexact_params<cell<TestType>> params{
            sidb_simulation_parameters{2, -0.32},
            quickexact_params<cell<TestType>>::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact<TestType>(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 21);
    }
}

TEMPLATE_TEST_CASE("Critical temperature of Bestagon CX, QuickSim", "[quality]", sidb_100_cell_clk_lyt_siqad,
                   cds_sidb_100_cell_clk_lyt_siqad)
{
    TestType lyt{};

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

    critical_temperature_params params{};
    sidb_simulation_parameters  sim_params{2, -0.32, 5.6, 5.0};

    critical_temperature_stats critical_stats{};

    params.simulation_parameters = sim_params;
    params.engine                = critical_temperature_params::simulation_engine::EXACT;
    params.confidence_level      = 0.99;
    params.max_temperature       = 350;
    params.iteration_steps       = 80;
    params.alpha                 = 0.7;

    critical_temperature_gate_based(lyt, std::vector<tt>{create_crossing_wire_tt()}, params, &critical_stats);

    CHECK_THAT(std::fabs(critical_stats.energy_between_ground_state_and_first_erroneous - 0.32),
               Catch::Matchers::WithinAbs(0.00, 0.01));
    CHECK_THAT(std::abs(critical_stats.critical_temperature - 0.85), Catch::Matchers::WithinAbs(0.00, 0.01));
}

TEST_CASE(
    "Determine physical parameters for CDS (default physical parameters) of Bestagon AND gate, 10 input combination",
    "[quality]")
{
    auto lyt = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();

    lyt.assign_cell_type({36, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{lyt, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension           = sweep_parameter::EPSILON_R;
    op_domain_params.x_min                 = 5.0;
    op_domain_params.x_max                 = 5.9;
    op_domain_params.x_step                = 0.1;
    op_domain_params.y_dimension           = sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                 = 5.0;
    op_domain_params.y_max                 = 5.9;
    op_domain_params.y_step                = 0.1;

    SECTION("Using the ground state of default physical parameters as given CDS")
    {
        cds.assign_charge_state({38, 0, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 0}, sidb_charge_state::NEGATIVE);

        cds.assign_charge_state({6, 2, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({32, 2, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({8, 3, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({30, 3, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({12, 4, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({26, 4, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({14, 5, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({24, 5, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 8, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({18, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({23, 9, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({18, 11, 1}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({19, 13, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({20, 14, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({24, 15, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({26, 16, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({30, 17, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({32, 18, 0}, sidb_charge_state::NEUTRAL);

        cds.assign_charge_state({36, 19, 0}, sidb_charge_state::NEGATIVE);

        cds.update_after_charge_change();

        const auto valid_parameters = determine_physically_valid_parameters(cds, op_domain_params);
        CHECK(valid_parameters.operational_values.size() == 100);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.6, 5.0})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.0, 5.9})
                  ->second == 2);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.4, 5.3})
                  ->second == 1);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.operational_values, parameter_point{5.8, 5.3})
                  ->second == 0);
    }
}

TEST_CASE("Bestagon CX gate input 11, using cube coordinates", "[quality]")
{
    sidb_cell_clk_lyt_cube lyt{};

    const auto params = assess_physical_population_stability_params{};
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{36, 1, 0}),
                         sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 1, 0}),
                         sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{20, 12, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 5, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 11, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 4, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 15, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{26, 4, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 9, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{24, 15, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 16, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{18, 9, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{26, 16, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{24, 13, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{24, 5, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{30, 3, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 13, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{32, 2, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{20, 8, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{30, 17, 0}),
                         sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 18, 0}),
                         sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{32, 18, 0}),
                         sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 17, 0}),
                         sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 19, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{36, 19, 0}),
                         sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_cells() == 27);

    const sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_cube> lat{lyt};

    const auto result = assess_physical_population_stability(lat, params);
    REQUIRE(result.size() == 20);
    const auto& population_stability_detail = result[0];
    CHECK(population_stability_detail.critical_cell == cube::coord_t{14, 18, 0});
    CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
    CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.01);
    REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                 Catch::Matchers::WithinAbs(6.88, 1e-5));
}

TEST_CASE("Bestagon CX gate input 11, using offset coordinates", "[quality]")
{
    sidb_100_cell_clk_lyt lyt{};

    const auto params = assess_physical_population_stability_params{};
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{36, 1, 0}),
                         sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{2, 1, 0}),
                         sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{20, 12, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 5, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 11, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 4, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 15, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{26, 4, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 9, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{24, 15, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 16, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{18, 9, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{26, 16, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{24, 13, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{24, 5, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{30, 3, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 13, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{32, 2, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{20, 8, 0}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{30, 17, 0}),
                         sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 18, 0}),
                         sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{32, 18, 0}),
                         sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 17, 0}),
                         sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{2, 19, 0}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{36, 19, 0}),
                         sidb_technology::cell_type::NORMAL);

    CHECK(lyt.num_cells() == 27);

    const sidb_lattice<sidb_100_lattice, sidb_100_cell_clk_lyt> lat{lyt};

    const auto result = assess_physical_population_stability(lat, params);
    REQUIRE(result.size() == 20);
    const auto& population_stability_detail = result[0];
    CHECK(population_stability_detail.critical_cell == offset::ucoord_t{14, 18, 0});
    CHECK(population_stability_detail.transition_from_to == transition_type::NEUTRAL_TO_NEGATIVE);
    CHECK(population_stability_detail.minimum_potential_difference_to_transition < 0.01);
    REQUIRE_THAT(population_stability_detail.distance_corresponding_to_potential,
                 Catch::Matchers::WithinAbs(6.88, 1e-5));
}

TEST_CASE("is operational check for Bestagon CX gate", "[quality]")
{
    using layout = sidb_cell_clk_lyt_siqad;

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
