//
// Created by Willem Lambooy on 29/02/2024.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>
#include <fiction/algorithms/simulation/sidb/quickexact.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/charge_distribution_surface.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <algorithm>
#include <cstdint>

using namespace fiction;

TEMPLATE_TEST_CASE(
    "Empty layout ClusterComplete simulation", "[clustercomplete]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    TestType lyt{};

    const clustercomplete_params params{sidb_simulation_parameters{2, -0.32}};

    const auto simulation_results = clustercomplete(lyt, params);

    CHECK(simulation_results.charge_distributions.empty());
    CHECK(simulation_results.algorithm_name == "ClusterComplete");
}

template <typename Lyt>
static bool verify_clustercomplete_result(const charge_distribution_surface<Lyt>&              qe_cl,
                                          const std::vector<charge_distribution_surface<Lyt>>& cc_cls) noexcept
{
    for (const charge_distribution_surface<Lyt>& cc_cl : cc_cls)
    {
        bool is_same = true;

        for (const cell<Lyt>& c : qe_cl.get_sidb_order())
        {
            if (qe_cl.get_charge_state(c) != cc_cl.get_charge_state(c))
            {
                is_same = false;
                break;
            }
        }

        if (is_same)
        {
            return true;
        }
    }

    return false;
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation of a 4 DB layout with a positive charge", "[clustercomplete]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))

{
    TestType lyt{};
    lyt.assign_cell_type({2, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({4, 0, 1}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 0}, TestType::cell_type::NORMAL);
    lyt.assign_cell_type({3, 1, 1}, TestType::cell_type::NORMAL);

    SECTION("Base 2")
    {
        const sidb_simulation_result<TestType>& qe_res = quickexact(
            lyt, quickexact_params<TestType>{sidb_simulation_parameters{2},
                                             quickexact_params<TestType>::automatic_base_number_detection::OFF});
        const sidb_simulation_result<TestType>& cc_res =
            clustercomplete(lyt, clustercomplete_params{sidb_simulation_parameters{2}});

        REQUIRE(qe_res.charge_distributions.size() == 1);
        CHECK(cc_res.charge_distributions.size() == 1);

        for (const charge_distribution_surface<TestType>& cl : qe_res.charge_distributions)
        {
            CHECK(verify_clustercomplete_result<TestType>(cl, cc_res.charge_distributions));
        }
    }

    SECTION("Base 3")
    {
        const sidb_simulation_result<TestType>& qe_res = quickexact(
            lyt, quickexact_params<TestType>{sidb_simulation_parameters{3},
                                             quickexact_params<TestType>::automatic_base_number_detection::OFF});
        const sidb_simulation_result<TestType>& cc_res =
            clustercomplete(lyt, clustercomplete_params{sidb_simulation_parameters{3}});

        REQUIRE(qe_res.charge_distributions.size() == 2);
        CHECK(cc_res.charge_distributions.size() == 2);

        for (const charge_distribution_surface<TestType>& cl : qe_res.charge_distributions)
        {
            CHECK(verify_clustercomplete_result(cl, cc_res.charge_distributions));
        }
    }
}

TEMPLATE_TEST_CASE(
    "Exact Cluster Simulation of 2 Bestagon NAND gates", "[clustercomplete]",
    (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
    (charge_distribution_surface<cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>>))
{
    gate_level_layout<hex_even_row_gate_clk_lyt> gate_lyt{{2, 2}};
    gate_lyt.create_nand({}, {}, {0, 0});
    gate_lyt.create_nand({}, {}, {2, 2});

    const TestType& cell_lyt{
        convert_to_siqad_coordinates(apply_gate_library<sidb_cell_clk_lyt, sidb_bestagon_library>(gate_lyt))};

    SECTION("Base 2")
    {
        const sidb_simulation_result<TestType>& res =
            clustercomplete(cell_lyt, clustercomplete_params{sidb_simulation_parameters{2}});

        CHECK(res.charge_distributions.size() == 81);
        CHECK_THAT(minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()),
                   Catch::Matchers::WithinAbs(1.3192717848, physical_constants::POP_STABILITY_ERR));
    }

    SECTION("Base 3")
    {
        const sidb_simulation_result<TestType>& res =
            clustercomplete(cell_lyt, clustercomplete_params{sidb_simulation_parameters{3}});

        CHECK(res.charge_distributions.size() == 81);
        CHECK_THAT(minimum_energy(res.charge_distributions.cbegin(), res.charge_distributions.cend()),
                   Catch::Matchers::WithinAbs(1.3192717848, physical_constants::POP_STABILITY_ERR));
    }
}

TEMPLATE_TEST_CASE(
    "ClusterComplete simulation of a Y-shape SiDB OR gate with input 01 under varying physical parameters",
    "[clustercomplete]", (cell_level_layout<sidb_technology, clocked_layout<cartesian_layout<siqad::coord_t>>>),
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

    clustercomplete_params params{sidb_simulation_parameters{2, -0.28}};

    SECTION("Check if ClusterComplete is deterministic")
    {
        std::set<double>   ground_state{};
        std::set<uint64_t> charge_index{};
        for (auto i = 0; i < 10000; i++)
        {
            const auto simulation_results = clustercomplete<TestType>(lyt, params);
            auto&      charge_lyt_first   = simulation_results.charge_distributions.front();
            ground_state.insert(charge_lyt_first.get_system_energy());
            charge_lyt_first.charge_distribution_to_index_general();
            charge_index.insert(charge_lyt_first.get_charge_index_and_base().first);
        }
        CHECK(ground_state.size() == 1);
        CHECK(charge_index.size() == 1);
    }

    SECTION("Add SiDBs which are positively charged in the ground state, layout does not fulfill the logic anymore.")
    {
        params.physical_parameters.base = 3;
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
        params.physical_parameters.mu_minus = -0.1;

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
        params.physical_parameters.mu_minus = -0.7;

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
        params.physical_parameters.lambda_tf = 1;

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
        params.physical_parameters.lambda_tf = 10;

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
        params.physical_parameters.epsilon_r = 10;

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
