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
#include <fiction/utils/math_utils.hpp>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout ExGS simulation", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.algorithm_name == "quickexact");
    CHECK(simulation_results.additional_simulation_parameters.empty());
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ExGS simulation with one negatively charge defect (default initialization) in proximity", "[ExGS]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    params.defects.insert({{1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with one negatively charge defect (changed lambda_tf) in proximity",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    params.defects.insert({{1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, 0.0, 2 * 10E-9}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with one negatively charge defect (changed epsilon_r) in proximity",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    params.defects.insert({{1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, 0.3}});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with one highly negatively charge defect in proximity", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.1}};

    params.defects.insert({{1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -10}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ExGS simulation with one highly negatively charge defect in proximity but with high screening",
    "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.1}};

    params.defects.insert({{1, 2, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -10, params.physical_parameters.epsilon_r,
                                       params.physical_parameters.lambda_tf * 10E-5}});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with two highly negatively and oppositely charged defects in proximity",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.1}};
    params.defects.insert({{2, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -10}});
    params.defects.insert({{-2, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 10}});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with local external potential", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};

    params.local_external_potential.insert({{0, 0, 0}, -0.5});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with local external potential (high)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    params.local_external_potential.insert({{{0, 0, 0}, -1}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with global external potential", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{2, -0.25}};
    params.global_potential = -0.26;

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with global external potential (high)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = -1;

    const auto simulation_results = quickexact<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ExGS simulation with global external potential (high, positive)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = 1;

    const auto simulation_results = quickexact<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ExGS simulation of a BDL pair", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};
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

TEMPLATE_TEST_CASE("ExGS simulation of a two-pair BDL wire with one perturber", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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
               Catch::Matchers::WithinAbs(0.24602741408, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a one-pair BDL wire with two perturbers", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{50, 10}};

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
               Catch::Matchers::WithinAbs(0.1152574819, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB arrangement", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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
               Catch::Matchers::WithinAbs(0.31915040629512115, fiction::physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB OR gate with input 01", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({6, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 3, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({14, 2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 5, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({10, 6, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 8, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({16, 1, 0}, TestType::cell_type::NORMAL);

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
               Catch::Matchers::WithinAbs(0.46621669, fiction::physical_constants::POP_STABILITY_ERR));
}
//
TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB OR gate with input 01 and local external potential at perturber",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB OR gate with input 01 and global external potential", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("ExGS simulation of a Y-shape SiDB OR gate with input 01 and global external potential (high)",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("ExGS simulation of four SiDBs (far away)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("ExGS simulation of four SiDBs (far away) with one negatively charged defects in proximity",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};
    params.defects.insert({{1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ExGS simulation of four SiDBs (far away) with two negatively charged defects in proximity",
                   "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    params.defects.insert({{1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1}});
    params.defects.insert({{31, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1}});

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "ExGS simulation of four SiDBs (far away) with one negatively and positively charged defect in proximity", "[ExGS]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.28}};

    params.defects.insert({{1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1}});
    params.defects.insert({{31, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1}});
    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Seven randomly distributed DBs, test if dependent cell calculation works correctly", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("three DBs next to each other", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("three DBs next to each other, small mu-", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("four DBs next to each other, small mu-", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_system_energy() == 0);
}

TEMPLATE_TEST_CASE("seven DBs next to each other, small mu-", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("7 DBs next to each other (positively charged DBs occur)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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
    "[ExGS]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

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

TEMPLATE_TEST_CASE("4 DBs next to each other (positively charged DBs occur)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.1}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 2);
}

TEMPLATE_TEST_CASE("5 DBs next to each other (positively charged DBs occur)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{
    TestType lyt{{20, 10}};

    lyt.assign_cell_type({-1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEMPLATE_TEST_CASE("3 DBs next to each other (positively charged DBs occur)", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    for (const auto& layout : simulation_results.charge_distributions)
    {
        CHECK(round_to_n_decimal_places(layout.get_system_energy(), 1) <= 0);
    }
}

TEMPLATE_TEST_CASE("3 DBs next to each other with automatic base number detection", "[ExGS]",
                   (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>))
{

    using sidb_layout = cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>;

    sidb_layout lyt{{20, 10}};

    lyt.assign_cell_type({5, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, sidb_layout::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_layout::cell_type::NORMAL);

    const quickexact_params<TestType> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = quickexact<TestType>(lyt, params);

    REQUIRE(!simulation_results.additional_simulation_parameters.empty());
    CHECK(simulation_results.additional_simulation_parameters[0].first == "base_number");
    CHECK(std::any_cast<uint64_t>(simulation_results.additional_simulation_parameters[0].second) == 3);

    const quickexact_params<TestType> params_new{sidb_simulation_parameters{3, -0.32},
                                                 automatic_base_number_detection::OFF};

    const auto simulation_results_new = quickexact<TestType>(lyt, params_new);

    REQUIRE(!simulation_results_new.additional_simulation_parameters.empty());
    CHECK(simulation_results_new.additional_simulation_parameters[0].first == "base_number");
    CHECK(std::any_cast<uint64_t>(simulation_results_new.additional_simulation_parameters[0].second) == 2);

    CHECK(simulation_results_new.simulation_runtime < simulation_results.simulation_runtime);
}
