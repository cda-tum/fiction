//
// Created by Willem Lambooy on 29.02.2024.
//

#if (FICTION_ALGLIB_ENABLED)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/groundstate_from_simulation_result.hpp>
#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sidb_charge_state.hpp>
#include <fiction/technology/sidb_defect_surface.hpp>
#include <fiction/technology/sidb_defects.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/traits.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>
#include <fiction/utils/math_utils.hpp>

#include <algorithm>
#include <any>
#include <cstdint>
#include <set>
#include <vector>

using namespace fiction;

TEMPLATE_TEST_CASE("Empty layout ClusterComplete simulation", "[clustercomplete]", sidb_cell_clk_lyt_siqad,
                   charge_distribution_surface<sidb_cell_clk_lyt_siqad>)
{
    TestType lyt{};

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32}};
    params.validity_witness_partitioning_max_cluster_size_gss = 3;
    params.num_overlapping_witnesses_limit_gss                = 8;

    const auto simulation_results = clustercomplete(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.algorithm_name == "ClusterComplete");
    CHECK(std::any_cast<uint64_t>(
              simulation_results.additional_simulation_parameters.at("validity_witness_partitioning_limit")) == 3);
    CHECK(std::any_cast<uint64_t>(
              simulation_results.additional_simulation_parameters.at("num_overlapping_witnesses_limit")) == 8);
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a single SiDB", "[clustercomplete]", sidb_cell_clk_lyt_siqad,
                   charge_distribution_surface<sidb_cell_clk_lyt_siqad>)
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = clustercomplete(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    REQUIRE(simulation_results.charge_distributions.front().num_cells() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

template <typename Lyt>
static bool verify_clustercomplete_result(const charge_distribution_surface<Lyt>&              qe_cds,
                                          const std::vector<charge_distribution_surface<Lyt>>& cc_cdss) noexcept
{
    return std::any_of(cc_cdss.cbegin(), cc_cdss.cend(),
                       [&](const charge_distribution_surface<Lyt>& cc_cds)
                       {
                           for (const auto& c : qe_cds.get_sidb_order())
                           {
                               if (qe_cds.get_charge_state(c) != cc_cds.get_charge_state(c))
                               {
                                   return false;
                               }
                           }

                           return true;
                       });
}

template <typename Lyt>
static bool
verify_clustercomplete_result_by_charge_indices(const charge_distribution_surface<Lyt>&              qe_cds,
                                                const std::vector<charge_distribution_surface<Lyt>>& cc_cdss) noexcept
{
    return std::any_of(cc_cdss.cbegin(), cc_cdss.cend(),
                       [&](const auto& cc_cds) {
                           return cc_cds.get_charge_index_and_base().first == qe_cds.get_charge_index_and_base().first;
                       });
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a 4 DB layout with a positive charge", "[clustercomplete]",
                   sidb_cell_clk_lyt_siqad, charge_distribution_surface<sidb_cell_clk_lyt_siqad>)

{
    TestType lyt{};
    lyt.assign_cell_type({2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 1, 1}, TestType::cell_type::NORMAL);

    SECTION("Base 2")
    {
        const sidb_simulation_result<TestType>& qe_res = quickexact(
            lyt,
            quickexact_params<cell<TestType>>{sidb_simulation_parameters{2},
                                              quickexact_params<cell<TestType>>::automatic_base_number_detection::OFF});
        const sidb_simulation_result<TestType>& cc_res =
            clustercomplete(lyt, clustercomplete_params<cell<TestType>>{sidb_simulation_parameters{2}});

        REQUIRE(qe_res.charge_distributions.size() == 1);
        CHECK(cc_res.charge_distributions.size() == 1);

        for (const charge_distribution_surface<TestType>& cds : qe_res.charge_distributions)
        {
            CHECK(verify_clustercomplete_result<TestType>(cds, cc_res.charge_distributions));
            CHECK(verify_clustercomplete_result_by_charge_indices<TestType>(cds, cc_res.charge_distributions));
        }
    }

    SECTION("Base 3")
    {
        const sidb_simulation_result<TestType>& qe_res = quickexact(
            lyt,
            quickexact_params<cell<TestType>>{sidb_simulation_parameters{3},
                                              quickexact_params<cell<TestType>>::automatic_base_number_detection::OFF});
        const sidb_simulation_result<TestType>& cc_res =
            clustercomplete(lyt, clustercomplete_params<cell<TestType>>{sidb_simulation_parameters{3}});

        REQUIRE(qe_res.charge_distributions.size() == 2);
        CHECK(cc_res.charge_distributions.size() == 2);

        for (const charge_distribution_surface<TestType>& cds : qe_res.charge_distributions)
        {
            CHECK(verify_clustercomplete_result(cds, cc_res.charge_distributions));
            CHECK(verify_clustercomplete_result_by_charge_indices<TestType>(cds, cc_res.charge_distributions));
        }
    }
}

TEST_CASE("Exact Cluster Simulation of 2 Bestagon NAND gates", "[clustercomplete]")
{
    gate_level_layout<hex_even_row_gate_clk_lyt> gate_lyt{{2, 2}};
    gate_lyt.create_nand({}, {}, {0, 0});
    gate_lyt.create_nand({}, {}, {2, 2});

    const sidb_cell_clk_lyt_siqad& cell_lyt{
        convert_layout_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(gate_lyt))};

    SECTION("Base 2")
    {
        const sidb_simulation_result<sidb_cell_clk_lyt_siqad>& res =
            clustercomplete(cell_lyt, clustercomplete_params<>{sidb_simulation_parameters{2}});

        CHECK(res.charge_distributions.size() == 81);
        CHECK_THAT(minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()),
                   Catch::Matchers::WithinAbs(1.3192717848, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Base 3")
    {
        const sidb_simulation_result<sidb_cell_clk_lyt_siqad>& res =
            clustercomplete(cell_lyt, clustercomplete_params<>{sidb_simulation_parameters{3}});

        CHECK(res.charge_distributions.size() == 81);
        CHECK_THAT(minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()),
                   Catch::Matchers::WithinAbs(1.3192717848, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation of a Y-shape SiDB OR gate with input 01 under varying physical parameters",
    "[clustercomplete]", sidb_cell_clk_lyt_siqad, charge_distribution_surface<sidb_cell_clk_lyt_siqad>)
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

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.28}};

    SECTION("Check if ClusterComplete is deterministic")
    {
        std::set<double>   ground_state{};
        std::set<uint64_t> charge_index{};
#ifdef NDEBUG
        for (auto i = 0; i < 10000; i++)
#else   // NDEBUG
        for (auto i = 0; i < 100; i++)
#endif  // NDEBUG
        {
            const auto simulation_results = clustercomplete<TestType>(lyt, params);
            auto&      charge_lyt_first   = simulation_results.charge_distributions.front();
            ground_state.insert(round_to_n_decimal_places(charge_lyt_first.get_system_energy(), 6));
            charge_lyt_first.charge_distribution_to_index_general();
            charge_index.insert(charge_lyt_first.get_charge_index_and_base().first);
        }
        CHECK(ground_state.size() == 1);
        CHECK(charge_index.size() == 1);
    }

    SECTION("Add SiDBs which are positively charged in the ground state, layout does not fulfill the logic anymore.")
    {
        params.simulation_parameters.base = 3;
        lyt.assign_cell_type({15, 2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({15, 2, 0}, TestType::cell_type::NORMAL);

        const auto simulation_results = clustercomplete<TestType>(lyt, params);
        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        CHECK(ground_state->num_positive_sidbs() > 0);
    }

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.mu_minus = -0.1;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.mu_minus = -0.7;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.lambda_tf = 1;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.lambda_tf = 10;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.epsilon_r = 10;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
    "Single SiDB ClusterComplete simulation with one negatively charge defect (default initialization) in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.25}};
    lyt.assign_sidb_defect({1, 2, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                                  params.simulation_parameters.lambda_tf});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ClusterComplete simulation with one negatively charge defect (changed lambda_tf) in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.25}};

    lyt.assign_sidb_defect({1, 2, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r, 2});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ClusterComplete simulation with one negatively charge defect (changed epsilon_r) in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    lyt.assign_sidb_defect({1, 6, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, 0.3, params.simulation_parameters.lambda_tf});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "four SiDBs ClusterComplete simulation with one negatively charge defect (changed epsilon_r) in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({-2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.15}};

    lyt.assign_sidb_defect({0, 0, 1}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                                  params.simulation_parameters.lambda_tf});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(1) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(2) == sidb_charge_state::NEUTRAL);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(3) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with one highly negatively charge defect in proximity",
                   "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
                   (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.1}};

    lyt.assign_sidb_defect({1, 2, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -10, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ClusterComplete simulation with one highly negatively charge defect in proximity but with high "
    "screening",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.1}};

    lyt.assign_sidb_defect({1, 2, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -10, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf * 10E-5});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "Single SiDB ClusterComplete simulation with two highly negatively and oppositely charged defects in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.1}};

    lyt.assign_sidb_defect({2, 0, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -10, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});
    lyt.assign_sidb_defect({-2, 0, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, 10, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with local external potential", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.25}};

    params.local_external_potential.insert({{0, 0, 0}, -0.5});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with local external potential (high)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    params.local_external_potential.insert({{{0, 0, 0}, -1}});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with global external potential", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.25}};
    params.global_potential = -0.26;

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(std::any_cast<double>(simulation_results.additional_simulation_parameters.at("global_potential")) == -0.26);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with global external potential (high)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = -1;

    const auto simulation_results = clustercomplete<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::POSITIVE);
}

TEMPLATE_TEST_CASE("Single SiDB ClusterComplete simulation with global external potential (high, positive)",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};
    params.global_potential = 1;

    const auto simulation_results = clustercomplete<TestType>(lyt, params);
    REQUIRE(simulation_results.charge_distributions.size() == 1);
    CHECK(simulation_results.charge_distributions.front().get_charge_state_by_index(0) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a BDL pair", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

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

TEMPLATE_TEST_CASE("ClusterComplete simulation of a two-pair BDL wire with one perturber", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({13, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({17, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({19, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);
    auto       size_before        = simulation_results.charge_distributions.size();

    const auto simulation_results_new = clustercomplete<TestType>(lyt, params);
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

TEST_CASE("ClusterComplete simulation of a one-pair BDL wire with two perturbers", "[clustercomplete]")
{
    sidb_100_cell_clk_lyt_siqad lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);
    lyt.assign_cell_type({15, 0, 0}, sidb_100_cell_clk_lyt_siqad::cell_type::NORMAL);

    const sidb_simulation_parameters params{2, -0.32};

    charge_distribution_surface charge_layout_kon{lyt, params};

    charge_layout_kon.assign_charge_state({0, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({5, 0, 0}, sidb_charge_state::NEUTRAL);
    charge_layout_kon.assign_charge_state({7, 0, 0}, sidb_charge_state::NEGATIVE);
    charge_layout_kon.assign_charge_state({15, 0, 0}, sidb_charge_state::NEGATIVE);

    charge_layout_kon.update_after_charge_change();

    const clustercomplete_params<> sim_params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<sidb_100_cell_clk_lyt_siqad>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({5, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({7, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({15, 0, 0}) == sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.1152677452, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shape SiDB arrangement", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-11, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-10, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-4, -1, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-3, -2, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-7, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-7, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> sim_params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<TestType>(lyt, sim_params);

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

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shape SiDB OR gate with input 01, check energy and charge "
                   "distribution, using siqad coordinates",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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

    const clustercomplete_params<cell<TestType>> sim_params{sidb_simulation_parameters{2, -0.28}};

    const auto simulation_results = clustercomplete<TestType>(lyt, sim_params);

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

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shape SiDB OR gate with input 01, check energy and charge "
                   "distribution, using offset coordinates",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt), (cds_sidb_100_cell_clk_lyt))
{
    TestType lyt{};

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 1, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 2, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 3, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 5, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 6, 1}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 8, 1}),
                         TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> sim_params{sidb_simulation_parameters{2, -0.28}};

    const auto simulation_results = clustercomplete<TestType>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 3, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 8, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 6, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 1, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 5, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shape SiDB OR gate with input 01, check energy and charge "
                   "distribution, using cube coordinates",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt_cube), (cds_sidb_100_cell_clk_lyt_cube))
{
    TestType lyt{};

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 1, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 2, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 3, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 5, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 1}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 8, 1}), TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> sim_params{sidb_simulation_parameters{2, -0.28}};

    const auto simulation_results = clustercomplete<TestType>(lyt, sim_params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 3, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 8, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 1, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 5, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEGATIVE);

    CHECK_THAT(charge_lyt_first.get_system_energy(),
               Catch::Matchers::WithinAbs(0.4662582096, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shape SiDB OR gate with input 01 and local external potential at "
                   "perturber, using siqad coordinates",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    params.local_external_potential.insert({{{6, 2, 0}, -0.5}});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

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

TEMPLATE_TEST_CASE("ClusterComplete simulation of a Y-shaped SiDB OR gate with input 01 and local external potential "
                   "at perturber, using offset coordinates",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt), (cds_sidb_100_cell_clk_lyt))
{
    TestType lyt{};
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 3, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 2, 0}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 5, 0}),
                         TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 6, 1}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 8, 1}),
                         TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 1, 0}),
                         TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    params.local_external_potential.insert(
        {{siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0}), -0.5}});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{12, 3, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 8, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 6, 1})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{16, 1, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{10, 5, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{14, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<offset::ucoord_t>(siqad::coord_t{8, 3, 0})) ==
          sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation of a Y-shape SiDB OR gate with input 01 and local external potential at perturber, "
    "using cube coordinates",
    "[clustercomplete]", (sidb_100_cell_clk_lyt_cube), (cds_sidb_100_cell_clk_lyt_cube))
{
    TestType lyt{};
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 3, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 2, 0}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 5, 0}), TestType::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 1}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 8, 1}), TestType::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 1, 0}), TestType::cell_type::NORMAL);

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    params.local_external_potential.insert({{siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0}), -0.5}});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{12, 3, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 8, 1})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 1})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 1, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 5, 0})) ==
          sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 2, 0})) ==
          sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{8, 3, 0})) ==
          sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ClusterComplete simulation  of a Y-shape SiDB OR gate with input 01 and global external potential",
                   "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    params.global_potential = -0.5;

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
    "ClusterComplete simulation of a Y-shape SiDB OR gate with input 01 and global external potential (high)",
    "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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

    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    params.global_potential = -2;

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

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

TEMPLATE_TEST_CASE("ClusterComplete simulation of four SiDBs (far away)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ClusterComplete with one SiDB and one negatively charged defect in proximity", "[clustercomplete]",
                   (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
                   (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};
    lyt.assign_sidb_defect({-1, -1, 1},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation  of four SiDBs (far away) with one negatively charged defects in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};
    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                                  params.simulation_parameters.lambda_tf});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation of four SiDBs (far away) with two negatively charged defects in proximity",
    "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};

    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                                  params.simulation_parameters.lambda_tf});
    lyt.assign_sidb_defect({31, 0, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of four SiDBs (far away) with one negatively and positively charged "
                   "defect in proximity",
                   "[clustercomplete]", (sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>),
                   (charge_distribution_surface<sidb_defect_surface<sidb_100_cell_clk_lyt_siqad>>))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({30, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};

    lyt.assign_sidb_defect({1, 0, 0}, sidb_defect{sidb_defect_type::UNKNOWN, 1, params.simulation_parameters.epsilon_r,
                                                  params.simulation_parameters.lambda_tf});
    lyt.assign_sidb_defect({31, 0, 0},
                           sidb_defect{sidb_defect_type::UNKNOWN, -1, params.simulation_parameters.epsilon_r,
                                       params.simulation_parameters.lambda_tf});
    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());
    CHECK(simulation_results.charge_distributions.front().num_defects() == 2);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    CHECK(charge_lyt_first.get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({10, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({20, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({30, 0, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("three DBs next to each other", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 4);

    const auto ground_state = std::min_element(
        simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
        [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

    CHECK(ground_state->get_charge_state({-1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state->get_charge_state({1, 3, 0}) == sidb_charge_state::POSITIVE);
    CHECK(ground_state->get_charge_state({2, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state->get_charge_state({3, 3, 0}) == sidb_charge_state::NEUTRAL);
}

TEMPLATE_TEST_CASE("three DBs next to each other, small mu-", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.8}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() > 0);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({1, 3, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(charge_lyt_first.get_charge_state({2, 3, 0}) == sidb_charge_state::POSITIVE);
    CHECK(charge_lyt_first.get_charge_state({3, 3, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("four DBs next to each other, small mu-", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 4);

    const auto excited_state = *std::max_element(
        simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
        [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });
    CHECK_THAT(excited_state.get_system_energy(), Catch::Matchers::WithinAbs(0, physical_constants::POP_STABILITY_ERR));
}

TEMPLATE_TEST_CASE("seven DBs next to each other, small mu-", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 3, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(simulation_results.charge_distributions.size() == 10);
    const auto& charge_lyt_first = simulation_results.charge_distributions.front();
    CHECK(charge_lyt_first.get_system_energy() < 0.08);
    CHECK(charge_lyt_first.get_system_energy() > -2.74);
}

TEMPLATE_TEST_CASE("7 DBs next to each other (positively charged DBs occur)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
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

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 17);
}

TEMPLATE_TEST_CASE(
    "7 DBs next to each other | only one physically valid charge distribution with only one neutrally charged DB",
    "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-6, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 4, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 6, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({8, 3, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({-8, -3, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({-1, -1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 2, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 1);
}

TEMPLATE_TEST_CASE("4 DBs next to each other (positively charged DBs occur)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.1}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 2);
}

TEMPLATE_TEST_CASE("6 DBs next to each other (positively charged DBs occur)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({-1, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({10, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(lyt.num_cells() == 6);

    CHECK(simulation_results.charge_distributions.size() == 3);
}

TEMPLATE_TEST_CASE("4 DBs close to each other", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};

    lyt.assign_cell_type({0, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 0, 1}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.25}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() > 0);
}

TEMPLATE_TEST_CASE("3 DBs next to each other (positively charged DBs occur)", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{

    TestType lyt{};

    lyt.assign_cell_type({5, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({7, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    for (const auto& layout : simulation_results.charge_distributions)
    {
        CHECK(round_to_n_decimal_places(layout.get_system_energy(), 1) <= 0);
    }
}

TEMPLATE_TEST_CASE("13 DBs which are all negatively charged", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
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

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    REQUIRE(!simulation_results.charge_distributions.empty());

    const auto& charge_lyt_first = simulation_results.charge_distributions.front();

    charge_lyt_first.foreach_cell([&](const auto& cell)
                                  { CHECK(charge_lyt_first.get_charge_state(cell) == sidb_charge_state::NEGATIVE); });

    CHECK(lyt.num_cells() == 13);
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of a 3 DB Wire", "[clustercomplete]",
                   (sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>),
                   (charge_distribution_surface<sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>>))
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

    // clustercomplete parameters are initialized
    clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.28}};

    SECTION("Standard Physical Parameters")
    {
        const auto simulation_results = clustercomplete<TestType>(lyt, params);

        // check that physically valid charge distributions were found
        REQUIRE(!simulation_results.charge_distributions.empty());

        // find the ground state, which is the charge distribution with the lowest energy
        const auto ground_state = std::min_element(
            simulation_results.charge_distributions.cbegin(), simulation_results.charge_distributions.cend(),
            [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        CHECK(ground_state->num_negative_sidbs() == 5);
        CHECK(ground_state->num_neutral_sidbs() == 4);
        CHECK(ground_state->num_positive_sidbs() == 0);

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
        params.simulation_parameters.mu_minus = -0.1;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.mu_minus = -0.7;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.lambda_tf = 1;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.lambda_tf = 10;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.epsilon_r = 10;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
        params.simulation_parameters.epsilon_r = 1;

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

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
    "ClusterComplete simulation of two SiDBs placed directly next to each other with non-realistic relative "
    "permittivity",
    "[clustercomplete]", (sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>),
    (charge_distribution_surface<sidb_lattice<sidb_100_lattice, sidb_cell_clk_lyt_siqad>>))
{
    TestType lyt{};
    lyt.assign_cell_type({1, 3, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);

    SECTION("Base 2")
    {
        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32, 1.0e-3}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

        CHECK(simulation_results.charge_distributions.empty());
    }

    SECTION("Base 3")
    {
        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32, 1.0e-3}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 2);
    }
}

TEMPLATE_TEST_CASE("ClusterComplete simulation of positively charged SiDBs", "[clustercomplete]",
                   (sidb_100_cell_clk_lyt_siqad), (cds_sidb_100_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 0, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({11, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({11, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({18, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({20, 0, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    CHECK(simulation_results.charge_distributions.size() == 4);
}

TEMPLATE_TEST_CASE("Special test cases", "[clustercomplete]", (sidb_100_cell_clk_lyt_siqad),
                   (cds_sidb_100_cell_clk_lyt_siqad))
{
    SECTION("Test case 1")
    {
        TestType lyt{};

        lyt.assign_cell_type({2, 1, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 2, 0}, TestType::cell_type::NORMAL);

        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 2);
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

        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 21);
    }

    SECTION("Test case 3")
    {
        TestType lyt{};

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 0}, TestType::cell_type::NORMAL);

        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 3);
    }

    SECTION("Test case 4")
    {
        TestType lyt{};

        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({0, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 4, 0}, TestType::cell_type::NORMAL);

        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{3, -0.32}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

        CHECK(simulation_results.charge_distributions.size() == 2);
    }

    SECTION("Test case 5")
    {
        TestType lyt{};

        lyt.assign_cell_type({2, 2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 3, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 4, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 4, 1}, TestType::cell_type::NORMAL);

        const sidb_simulation_result<TestType>& cc_res = clustercomplete(lyt);

        REQUIRE(cc_res.charge_distributions.size() == 2);
    }

    SECTION("Test case 6")
    {
        TestType lyt{};

        lyt.assign_cell_type({3, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 0, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 0, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({22, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({11, 5, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({4, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({23, 7, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({16, 8, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 8, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({15, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({1, 10, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 10, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({14, 10, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({24, 11, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 11, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 12, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({24, 12, 0}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{3, -0.32, 5.6, 5.0};

        sidb_simulation_result<TestType> cc_res = clustercomplete(lyt, clustercomplete_params<cell<TestType>>{params});

        std::sort(cc_res.charge_distributions.begin(), cc_res.charge_distributions.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        REQUIRE(cc_res.charge_distributions.size() == 2);

        const auto groundstate  = cc_res.charge_distributions[0];
        const auto excitedstate = cc_res.charge_distributions[1];

        CHECK(groundstate.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({4, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(groundstate.get_charge_state({13, 0, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({5, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({22, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({11, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({2, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({4, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({23, 7, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({16, 8, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({8, 8, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({15, 9, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({1, 10, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({12, 10, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({14, 10, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({9, 11, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({24, 11, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(groundstate.get_charge_state({10, 11, 1}) == sidb_charge_state::POSITIVE);
        CHECK(groundstate.get_charge_state({13, 12, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(groundstate.get_charge_state({24, 12, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(excitedstate.get_charge_state({3, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({4, 0, 0}) == sidb_charge_state::POSITIVE);
        CHECK(excitedstate.get_charge_state({13, 0, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({5, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({22, 3, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({11, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({2, 6, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({4, 6, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({23, 7, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({16, 8, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({8, 8, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({15, 9, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({1, 10, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({12, 10, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({14, 10, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({9, 11, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({24, 11, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({10, 11, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(excitedstate.get_charge_state({13, 12, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(excitedstate.get_charge_state({24, 12, 0}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("Test case 7")
    {
        TestType lyt{};

        lyt.assign_cell_type({11, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({15, 1, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({17, 2, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({20, 3, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 3, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 4, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 5, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({17, 5, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({2, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({9, 6, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({10, 7, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({8, 7, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({17, 9, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({11, 9, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({12, 9, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 10, 0}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({5, 10, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({7, 11, 1}, TestType::cell_type::NORMAL);
        lyt.assign_cell_type({13, 11, 1}, TestType::cell_type::NORMAL);

        const sidb_simulation_parameters params{2, -0.32};

        sidb_simulation_result<TestType> cc_res = clustercomplete(lyt, clustercomplete_params<cell<TestType>>{params});

        std::sort(cc_res.charge_distributions.begin(), cc_res.charge_distributions.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.get_system_energy() < rhs.get_system_energy(); });

        CHECK(cc_res.charge_distributions.size() == 2);
    }
}

TEMPLATE_TEST_CASE("ClusterComplete gate simulation of Si-111 surface", "[clustercomplete]",
                   (sidb_111_cell_clk_lyt_siqad), (cds_sidb_111_cell_clk_lyt_siqad))
{
    TestType lyt{};
    lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({1, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({8, 0, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({6, 1, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({5, 2, 1}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 8, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 10, 0}, TestType::cell_type::NORMAL);

    lyt.assign_cell_type({4, 14, 0}, TestType::cell_type::NORMAL);

    const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32, 5.6, 5}};

    const auto simulation_results = clustercomplete<TestType>(lyt, params);

    const auto ground_state = groundstate_from_simulation_result(simulation_results);
    REQUIRE(ground_state.size() == 1);

    CHECK(ground_state.front().get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({2, 2, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({8, 0, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({6, 1, 1}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({5, 2, 1}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 8, 0}) == sidb_charge_state::NEUTRAL);
    CHECK(ground_state.front().get_charge_state({4, 10, 0}) == sidb_charge_state::NEGATIVE);
    CHECK(ground_state.front().get_charge_state({4, 14, 0}) == sidb_charge_state::NEGATIVE);
}

TEMPLATE_TEST_CASE("ClusterComplete AND gate simulation of Si-111 surface", "[clustercomplete]",
                   sidb_111_cell_clk_lyt_siqad, cds_sidb_111_cell_clk_lyt_siqad)
{
    SECTION("no input applied")
    {
        const auto                                   lyt = blueprints::and_gate_111<TestType>();
        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32, 5.6, 5}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);

        const auto ground_state = groundstate_from_simulation_result(simulation_results);
        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({0, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({23, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == sidb_charge_state::NEGATIVE);
    }

    SECTION("10 input applied")
    {
        auto lyt = blueprints::and_gate_111<TestType>();
        lyt.assign_cell_type({0, 0, 0}, TestType::cell_type::EMPTY);
        lyt.assign_cell_type({23, 1, 1}, TestType::cell_type::EMPTY);

        const clustercomplete_params<cell<TestType>> params{sidb_simulation_parameters{2, -0.32, 5.6, 5}};

        const auto simulation_results = clustercomplete<TestType>(lyt, params);
        CHECK(simulation_results.charge_distributions.size() == 7);

        const auto ground_state = groundstate_from_simulation_result(simulation_results);

        REQUIRE(ground_state.size() == 1);

        CHECK(ground_state.front().get_charge_state({1, 1, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({25, 0, 0}) == sidb_charge_state::NEGATIVE);

        CHECK(ground_state.front().get_charge_state({4, 4, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({21, 4, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({5, 5, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({19, 5, 1}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({8, 8, 0}) == sidb_charge_state::NEUTRAL);
        CHECK(ground_state.front().get_charge_state({17, 8, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({9, 9, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({15, 9, 1}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({10, 18, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({13, 17, 0}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({16, 18, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({15, 21, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({17, 23, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({19, 25, 1}) == sidb_charge_state::NEGATIVE);
        CHECK(ground_state.front().get_charge_state({21, 27, 0}) == sidb_charge_state::NEUTRAL);

        CHECK(ground_state.front().get_charge_state({23, 29, 1}) == sidb_charge_state::NEGATIVE);
    }
}

#else  // FICTION_ALGLIB_ENABLED

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClusterComplete", "[clustercomplete]")
{
    CHECK(true);  // workaround for empty test case
}

#endif  // FICTION_ALGLIB_ENABLED
