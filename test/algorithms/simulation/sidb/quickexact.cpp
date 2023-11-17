//
// Created by Jan Drewniok on 18.12.22.

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_surface.hpp>
#include <fiction/utils/math_utils.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Empty layout QuickExact simulation", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "QuickExact");
    CHECK(simulation_results.additional_simulation_parameters.empty());
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with one negatively charge defect (default initialization) in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};
    lyt.assign_sidb_defect({1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with one negatively charge defect (changed lambda_tf) in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    lyt.assign_sidb_defect({1, 2, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r, 2});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with one negatively charge defect (changed epsilon_r) in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    lyt.assign_sidb_defect({1, 6, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, 0.3, params.physical_parameters.lambda_tf});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "four SiDBs QuickExact simulation with one negatively charge defect (changed epsilon_r) in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({-2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.15}};

    lyt.assign_sidb_defect({0, 0, 1}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(1) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(2) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(3) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with one highly negatively charge defect in proximity", "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.1}};

    lyt.assign_sidb_defect({1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -10, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with one highly negatively charge defect in proximity but with high screening",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.1}};

    lyt.assign_sidb_defect({1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -10, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf * 10E-5});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with two highly negatively and oppositely charged defects in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.1}};

    lyt.assign_sidb_defect({2, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -10, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});
    lyt.assign_sidb_defect({-2, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 10, params.physical_parameters.epsilon_r,
                                                   params.physical_parameters.lambda_tf});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with local external potential", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    params.local_external_potential.insert({{0, 0, 0}, -0.5});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with local external potential (high)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    params.local_external_potential.insert({{{0, 0, 0}, -1}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with global external potential", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};
    params.global_potential = -0.26;

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with global external potential (high)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = -1;

    const auto simulation_results = quickexact<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB QuickExact simulation with global external potential (high, positive)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = 1;

    const auto simulation_results = quickexact<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a BDL pair", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 2);
    for (const auto& layouts : simulation_results.charge_distributions)
    {
        uint64_t counter_negative = 0;
        uint64_t counter_neutral  = 0;
        for (uint64_t i = 0; i < 2; i++)
        {
            if (layouts.get_charge_state_by_index(i) == sidb_charge_state::NEGATIVE)
            {
                counter_negative += 1;
            }
            else
            {
                counter_neutral += 1;
            }
        }
        CHECK(counter_neutral == 1);
        CHECK(counter_negative == 1);
    }
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a two-pair BDL wire with one perturber", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);
    auto       size_before        = simulation_results.charge_distributions.size();

    const auto simulation_results_new = quickexact<TestType>(lyt, params);
    auto       size_after             = simulation_results_new.charge_distributions.size();

    CHECK(size_before == 1);
    CHECK(size_after == 1);

    REQUIRE(!simulation_results_new.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results_new.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({11, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({13, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({17, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.2460493219, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("QuickExact simulation of a one-pair BDL wire with two perturbers", "[quickexact]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, TestType::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    charge_distribution_surface charge_layout_kon{lyt, params};

    charge_layout_kon.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({5, 0, 0}, sidb_charge_state::NEUTRAL);
    charge_layout_kon.assign_charge_state({7, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({15, 0, 0}, sidb_charge_state::NEGATIVE);

    charge_layout_kon.update_after_charge_change();

    const quickexact_params<TestType> sim_params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.1152677452, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a Y-shape SiDB arrangement", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({-11, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> sim_params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({-11, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-10, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-3, -2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-4, -1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 0, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({-7, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({-7, 3, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.3191788254, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a Y-shape SiDB OR gate with input 01, check energy and charge distribution",
    "[quickexact]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> sim_params{sidb_simulation_parameters{2, -0.28}};

    const auto simulation_results = quickexact<TestType>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a Y-shape SiDB OR gate with input 01 and local external potential at perturber",
    "[quickexact]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
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

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};
    params.local_external_potential.insert({{{6, 2, 0}, -0.5}});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation  of a Y-shape SiDB OR gate with input 01 and global external potential", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
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

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};
    params.global_potential = -0.5;

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a Y-shape SiDB OR gate with input 01 and global external potential (high)",
    "[quickexact]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
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

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};
    params.global_potential = -2;

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of four SiDBs (far away)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation  of four SiDBs (far away) with one negatively charged defects in proximity", "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};
    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of four SiDBs (far away) with two negatively charged defects in proximity", "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});
    lyt.assign_sidb_defect({31, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                   params.physical_parameters.lambda_tf});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of four SiDBs (far away) with one negatively and positively charged defect in proximity",
    "[quickexact]",
    (sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>),
    (charge_distribution_surface<
        sidb_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1, params.physical_parameters.epsilon_r,
                                                  params.physical_parameters.lambda_tf});
    lyt.assign_sidb_defect({31, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.physical_parameters.epsilon_r,
                                                   params.physical_parameters.lambda_tf});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Seven randomly distributed DBs, test if dependent cell calculation works correctly", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({6, 10, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 10, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({3, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({4, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({6, 3, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({6, 10, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 10, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "three DBs next to each other", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 3);

    auto energy_min = std::numeric_limits<double>::max();
    for (const auto& layout : simulation_results.charge_distributions)
    {
        if (layout.get_system_energy() < energy_min)
        {
            energy_min = layout.get_system_energy();
        }
    }

    for (const auto& layout : simulation_results.charge_distributions)
    {
        if (std::abs(layout.get_system_energy() - energy_min) < physical_constants::POP_STABILITY_ERR)
        {
            CHECK(layout.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
            CHECK(layout.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
            CHECK(layout.get_charge_state({2, 3, 0}) == sidb_charge_state::POSITIVE);
            CHECK(layout.get_charge_state({3, 3, 0}) == sidb_charge_state::NEGATIVE);
        }
    }
}

TEMPLATE_TEST_CASE(
    "three DBs next to each other, small mu-", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.8}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() > 0);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({2, 3, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({3, 3, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "four DBs next to each other, small mu-", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE(
    "seven DBs next to each other, small mu-", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 3);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_system_energy() < 0.08);
    CHECK(charge_lyt_first.get_system_energy() > -2.74);
}

TEMPLATE_TEST_CASE(
    "7 DBs next to each other (positively charged DBs occur)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 5);
}

TEMPLATE_TEST_CASE(
    "7 DBs next to each other | only one physically valid charge distribution with only one neutrally charged DB",
    "[quickexact]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({-6, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 4, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-8, -3, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEMPLATE_TEST_CASE(
    "4 DBs next to each other (positively charged DBs occur)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.1}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 2);
}

TEMPLATE_TEST_CASE(
    "5 DBs next to each other (positively charged DBs occur)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(lyt.num_cells() == 6);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEMPLATE_TEST_CASE(
    "3 DBs next to each other (positively charged DBs occur)", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{

    TestType lyt{};

    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    for (const auto& layout : simulation_results.charge_distributions)
    {
        CHECK(round_to_n_decimal_places(layout.get_system_energy(), 1) <= 0);
    }
}

TEMPLATE_TEST_CASE(
    "3 DBs next to each other with automatic base number detection", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.additional_simulation_parameters[0].first == "base_number");
    CHECK(std::any_cast<uint64_t>(simulation_results.additional_simulation_parameters[0].second) == 3);

    const quickexact_params<TestType> params_new{sidb_simulation_parameters{2, -0.32},
                                                 quickexact_params<TestType>::automatic_base_number_detection::OFF};

    const auto simulation_results_new = quickexact<TestType>(lyt, params_new);

    REQUIRE(!simulation_results_new.additional_simulation_parameters.empty());
    CHECK(simulation_results_new.additional_simulation_parameters[0].first == "base_number");
    CHECK(std::any_cast<uint64_t>(simulation_results_new.additional_simulation_parameters[0].second) == 2);
}

TEMPLATE_TEST_CASE(
    "13 DBs which are all negatively charged", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    lyt.assign_cell_type({26, 10, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({23, 19, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({0, 5, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({38, 10, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 5, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({40, 19, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 9, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({19, 16, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 8, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 15, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({39, 9, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({30, 15, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    charge_lyt_first.foreach_cell([&](const auto& cell)
                                  { CHECK(charge_lyt_first.get_charge_state(cell) == sidb_charge_state::NEGATIVE); });

    CHECK(lyt.num_cells() == 13);
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a Y-shape SiDB OR gate with input 01", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
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

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.28},
                                       quickexact_params<TestType>::automatic_base_number_detection::OFF};

    SECTION("Check if QuickExact is deterministic")
    {
        SECTION("Epsilon_r = 8")
        {
            params.physical_parameters.epsilon_r = 8;
            std::set<double>   ground_state{};
            std::set<uint64_t> charge_index{};
            for (auto i = 0; i < 10000; i++)
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
        SECTION("Epsilon_r = 2")
        {
            params.physical_parameters.epsilon_r = 2;
            std::set<double>   ground_state{};
            std::set<uint64_t> charge_index{};
            for (auto i = 0; i < 10000; i++)
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
    }

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased mu_minus")
    {
        params.physical_parameters.mu_minus = -0.1;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.061037632, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased mu_minus")
    {
        params.physical_parameters.mu_minus = -0.7;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(2.069954113, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased lambda_tf")
    {
        params.physical_parameters.lambda_tf = 1;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.5432404075, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased lambda_tf")
    {
        params.physical_parameters.lambda_tf = 10;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEUTRAL);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.2930574885, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased epsilon_r")
    {
        params.physical_parameters.epsilon_r = 10;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());
        const auto& charge_lyt_first = simulation_results.charge_distributions.front();

        CHECK(charge_lyt_first.get_charge_state({6, 2, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({12, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({16, 1, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(charge_lyt_first.get_charge_state({10, 5, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({14, 2, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(charge_lyt_first.get_charge_state({8, 3, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(charge_lyt_first.get_system_energy(),
                   Catch::Matchers::WithinAbs(0.505173434, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of a 3 DB Wire", "[quickexact]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    // three BDL pairs with one perturber
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({22, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({26, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({29, 0, 0}, TestType::cell_type::NORMAL);

    // quickexact parameters are initialized
    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28},
                                       quickexact_params<TestType>::automatic_base_number_detection::OFF};

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = quickexact<TestType>(lyt, params);

        // check that physically valid charge distributions were found
        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // check that charge distribution is correct; binary 1 is propagated through the BDL wire
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(0.274134844, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased mu_minus")
    {
        // set small absolute value for µ
        params.physical_parameters.mu_minus = -0.1;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // check charge distribution of the ground state; BDL wire no longer works as intended
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(0.0329179963, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased mu_minus")
    {
        // set large absolute value for µ
        params.physical_parameters.mu_minus = -0.7;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // Due to the set µ-value, all SiDBs are negatively charged (electrostatic interaction is not strong enough to
        // change the charge state of individual SiDBs).
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(1.8649862557, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decreased lambda_tf")
    {
        // set small lambda value, i.e., electrostatic screening is significant.
        params.physical_parameters.lambda_tf = 1;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // Due to the small lambda value, the electrostatic interaction is small. Hence, all SiDBs are negatively
        // charged.
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(0.4606785472, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased lambda_tf")
    {
        // set large lambda value, i.e., electrostatic screening is small.
        params.physical_parameters.lambda_tf = 10;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // check charge distribution of the ground state; BDL wire works as intended
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(0.3967750406, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Increased epsilon_r")
    {
        // set large relative permittivity
        params.physical_parameters.epsilon_r = 10;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // The electrostatic interaction is small, due to the large relative permittivity.
        // Therefore, all SiDBs are negatively charged.
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(1.0443923032, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Decrease epsilon_r, positively charged SiDBs can occur")
    {
        // set small relative permittivity
        params.physical_parameters.epsilon_r = 1;

        const auto simulation_results = quickexact<TestType>(lyt, params);

        REQUIRE(!simulation_results.charge_distributions.empty());

        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        // The electrostatic interaction is strong, due to the small relative permittivity.
        // Therefore, SiDBs can even be positively charged.
        CHECK(ground_state->get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({5, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(ground_state->get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({12, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(ground_state->get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({19, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(ground_state->get_charge_state({22, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state->get_charge_state({26, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(ground_state->get_charge_state({29, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK_THAT(ground_state->get_system_energy(),
                   Catch::Matchers::WithinAbs(-5.0592576221, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE(
    "QuickExact simulation of two SiDBs placed directly next to each other with non-realistic relative permittivity",
    "[quickexact]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);

    SECTION("automatic base number detection is off")
    {
        const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32, 1.0e-3},
                                                 quickexact_params<TestType>::automatic_base_number_detection::OFF};

        const auto simulation_results = quickexact<TestType>(lyt, params);

        CHECK(simulation_results.charge_distributions.empty());
    }

    SECTION("automatic base number detection is on")
    {
        const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32, 1.0e-3},
                                                 quickexact_params<TestType>::automatic_base_number_detection::ON};

        const auto simulation_results = quickexact<TestType>(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 2);
    }
}
