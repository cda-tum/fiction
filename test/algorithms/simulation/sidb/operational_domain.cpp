//
// Created by marcel on 27.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

using namespace fiction;

TEST_CASE("BDL wire operational domain computation", "[operational-domain]")
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

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 5.1;
    op_domain_params.x_max       = 6.1;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 4.5;
    op_domain_params.y_max       = 5.5;
    op_domain_params.y_step      = 0.1;

    operational_domain_stats op_domain_stats{};

    SECTION("operational area")
    {
        SECTION("grid_search")
        {
            const auto op_domain =
                operational_domain_grid_search(lyt, create_id_tt(), op_domain_params, &op_domain_stats);

            CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
            CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 100);

            // for the selected range, all samples should be within the parameters and operational
            for (const auto& [coord, op_value] : op_domain.operational_values)
            {
                CHECK(coord.first >= 5.1);
                CHECK(coord.first <= 6.1);
                CHECK(coord.second >= 4.5);
                CHECK(coord.second <= 5.5);

                CHECK(op_value == operational_domain::operational_status::OPERATIONAL);
            }

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("random_sampling")
        {
            const auto op_domain =
                operational_domain_random_sampling(lyt, create_id_tt(), 100, op_domain_params, &op_domain_stats);

            CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
            CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

            // check if the operational domain has the correct size (max 10 steps in each dimension)
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and operational
            for (const auto& [coord, op_value] : op_domain.operational_values)
            {
                CHECK(coord.first >= 5.1);
                CHECK(coord.first <= 6.1);
                CHECK(coord.second >= 4.5);
                CHECK(coord.second <= 5.5);

                CHECK(op_value == operational_domain::operational_status::OPERATIONAL);
            }

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
    }
    SECTION("non-operational area")
    {
        op_domain_params.sim_params.mu_minus = -0.15;

        SECTION("grid_search")
        {
            const auto op_domain =
                operational_domain_grid_search(lyt, create_id_tt(), op_domain_params, &op_domain_stats);

            CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
            CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 100);

            // for the selected range, all samples should be within the parameters and non-operational
            for (const auto& [coord, op_value] : op_domain.operational_values)
            {
                CHECK(op_value == operational_domain::operational_status::NON_OPERATIONAL);
            }

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 100);
        }
        SECTION("random_sampling")
        {
            const auto op_domain =
                operational_domain_random_sampling(lyt, create_id_tt(), 100, op_domain_params, &op_domain_stats);

            CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
            CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

            // check if the operational domain has the correct size (max 10 steps in each dimension)
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and non-operational
            for (const auto& [coord, op_value] : op_domain.operational_values)
            {
                CHECK(coord.first >= 5.1);
                CHECK(coord.first <= 6.1);
                CHECK(coord.second >= 4.5);
                CHECK(coord.second <= 5.5);

                CHECK(op_value == operational_domain::operational_status::NON_OPERATIONAL);
            }

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 100);
        }
    }
}

TEST_CASE("SiQAD's AND gate operational domain computation", "[operational-domain]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

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

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 5.1;
    op_domain_params.x_max       = 6.1;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 4.5;
    op_domain_params.y_max       = 5.5;
    op_domain_params.y_step      = 0.1;

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain = operational_domain_grid_search(lyt, create_and_tt(), op_domain_params, &op_domain_stats);

        CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
        CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 100);

        // for the selected range, all samples should be within the parameters and operational
        for (const auto& [coord, op_value] : op_domain.operational_values)
        {
            CHECK(coord.first >= 5.1);
            CHECK(coord.first <= 6.1);
            CHECK(coord.second >= 4.5);
            CHECK(coord.second <= 5.5);

            CHECK(op_value == operational_domain::operational_status::OPERATIONAL);
        }

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("random_sampling")
    {
        const auto op_domain =
            operational_domain_random_sampling(lyt, create_and_tt(), 100, op_domain_params, &op_domain_stats);

        CHECK(op_domain.x_dimension == operational_domain::sweep_parameter::EPSILON_R);
        CHECK(op_domain.y_dimension == operational_domain::sweep_parameter::LAMBDA_TF);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        for (const auto& [coord, op_value] : op_domain.operational_values)
        {
            CHECK(coord.first >= 5.1);
            CHECK(coord.first <= 6.1);
            CHECK(coord.second >= 4.5);
            CHECK(coord.second <= 5.5);

            CHECK(op_value == operational_domain::operational_status::OPERATIONAL);
        }

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}
