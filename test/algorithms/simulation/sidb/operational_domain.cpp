//
// Created by marcel on 27.07.23.
//

#include <catch2/catch_template_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <optional>
#include <vector>

using namespace fiction;

TEST_CASE("Structured binding support for parameter_points", "[operational-domain]")
{
    auto param_point = operational_domain::parameter_point{1.0, 2.0};

    CHECK(param_point.x == 1.0);
    CHECK(param_point.y == 2.0);

    const auto& [x, y] = param_point;

    CHECK(x == 1.0);
    CHECK(y == 2.0);
}

void check_op_domain_params_and_operational_status(const operational_domain&                op_domain,
                                                   const operational_domain_params&         params,
                                                   const std::optional<operational_status>& status) noexcept
{
    CHECK(op_domain.x_dimension == params.x_dimension);
    CHECK(op_domain.y_dimension == params.y_dimension);

    for (const auto& [coord, op_value] : op_domain.operational_values)
    {
        CHECK(coord.x - params.x_min > -physical_constants::POP_STABILITY_ERR);
        CHECK(params.x_max - coord.x > -physical_constants::POP_STABILITY_ERR);
        CHECK(coord.y - params.y_min > -physical_constants::POP_STABILITY_ERR);
        CHECK(params.y_max - coord.y > -physical_constants::POP_STABILITY_ERR);

        if (status)
        {
            CHECK(op_value == *status);
        }
    }
}

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

    const sidb_lattice<sidb_100_lattice, layout> lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension           = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.y_dimension           = operational_domain::sweep_parameter::LAMBDA_TF;

    operational_domain_stats op_domain_stats{};

    SECTION("operational area, only one parameter point")
    {
        op_domain_params.x_min  = 5.5;
        op_domain_params.x_max  = 5.5;
        op_domain_params.x_step = 0.1;

        op_domain_params.y_min  = 5.0;
        op_domain_params.y_max  = 5.0;
        op_domain_params.y_step = 0.1;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                  op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations > 0);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
    }

    SECTION("operational area, same number of steps in x- and y-direction")
    {
        op_domain_params.x_min  = 5.1;
        op_domain_params.x_max  = 6.0;
        op_domain_params.x_step = 0.1;

        op_domain_params.y_min  = 4.5;
        op_domain_params.y_max  = 5.4;
        op_domain_params.y_step = 0.1;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                  op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 100);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations > 0);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 100);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
    }

    SECTION("operational area, different number of steps in x- and y-direction")
    {
        op_domain_params.x_min  = 5.1;
        op_domain_params.x_max  = 6.0;
        op_domain_params.x_step = 0.1;

        op_domain_params.y_min  = 4.5;
        op_domain_params.y_max  = 4.9;
        op_domain_params.y_step = 0.1;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                  op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations > 0);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }

        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() <= 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        }
    }

    SECTION("non-operational area")
    {
        op_domain_params.x_min  = 2.5;
        op_domain_params.x_max  = 3.4;
        op_domain_params.x_step = 0.1;

        op_domain_params.y_min  = 4.5;
        op_domain_params.y_max  = 5.4;
        op_domain_params.y_step = 0.1;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                  op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 100);

            // for the selected range, all samples should be within the parameters and non-operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params,
                                                          operational_status::NON_OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 100);
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 5000,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.operational_values.size() <= 5000);

            // for the selected range, all samples should be within the parameters and non-operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params,
                                                          operational_status::NON_OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 10000);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 5000);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 5000);
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 25,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters and non-operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params,
                                                          operational_status::NON_OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 100);
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 25,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.operational_values.size() <= 25);

            // for the selected range, all samples should be within the parameters and non-operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params,
                                                          operational_status::NON_OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 50);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 25);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 25);
        }
    }
    SECTION("floating-point error")
    {
        op_domain_params.x_min  = 2.5;
        op_domain_params.x_max  = 4.4;
        op_domain_params.x_step = 0.9;

        op_domain_params.y_min  = 2.5;
        op_domain_params.y_max  = 2.5;
        op_domain_params.y_step = 0.1;

        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 10000,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 3);

            CHECK(op_domain_stats.num_operational_parameter_combinations == 2);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 1);
        }
    }
    SECTION("semi-operational area")
    {
        op_domain_params.x_min  = 0.5;
        op_domain_params.x_max  = 4.25;
        op_domain_params.x_step = 0.25;

        op_domain_params.y_min  = 0.5;
        op_domain_params.y_max  = 4.25;
        op_domain_params.y_step = 0.25;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                  op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (16 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 256);

            // for the selected range, all samples should be within the parameters
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 512);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 256);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 80);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 176);
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 100);
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 50,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() >= 80);

            // for the selected range, all samples should be within the parameters
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 512);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 512);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 80);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 100);
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 50,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (max 10 steps in each dimension)
            CHECK(op_domain.operational_values.size() <= 100);

            // for the selected range, all samples should be within the parameters
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 512);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 80);
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

    const sidb_lattice<sidb_100_lattice, layout> lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension           = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min                 = 5.1;
    op_domain_params.x_max                 = 6.0;
    op_domain_params.x_step                = 0.1;
    op_domain_params.y_dimension           = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                 = 4.5;
    op_domain_params.y_max                 = 5.4;
    op_domain_params.y_step                = 0.1;

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain =
            operational_domain_grid_search(lat, std::vector<tt>{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("random_sampling")
    {
        const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_and_tt()}, 100,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("flood_fill")
    {
        const auto op_domain =
            operational_domain_flood_fill(lat, std::vector<tt>{create_and_tt()}, 1, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("contour_tracing")
    {
        const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_and_tt()}, 1,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}

TEST_CASE("SiQAD's AND gate operational domain computation, using cube coordinates", "[operational-domain]")
{
    using layout = sidb_cell_clk_lyt_cube;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{0, 0, 1}),
                         sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{2, 1, 1}),
                         sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{20, 0, 1}),
                         sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{18, 1, 1}),
                         sidb_technology::cell_type::INPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{4, 2, 1}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{6, 3, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{14, 3, 1}),
                         sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{16, 2, 1}),
                         sidb_technology::cell_type::NORMAL);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 6, 0}),
                         sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 7, 0}),
                         sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type(siqad::to_fiction_coord<cube::coord_t>(siqad::coord_t{10, 9, 1}),
                         sidb_technology::cell_type::NORMAL);

    const sidb_lattice<sidb_100_lattice, layout> lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension           = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min                 = 5.1;
    op_domain_params.x_max                 = 6.0;
    op_domain_params.x_step                = 0.1;
    op_domain_params.y_dimension           = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                 = 4.5;
    op_domain_params.y_max                 = 5.4;
    op_domain_params.y_step                = 0.1;

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain =
            operational_domain_grid_search(lat, std::vector<tt>{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("random_sampling")
    {
        const auto op_domain = operational_domain_random_sampling(lat, std::vector<tt>{create_and_tt()}, 100,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("flood_fill")
    {
        const auto op_domain =
            operational_domain_flood_fill(lat, std::vector<tt>{create_and_tt()}, 1, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("contour_tracing")
    {
        const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_and_tt()}, 1,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}

TEMPLATE_TEST_CASE("AND gate on the H-Si(111)-1x1 surface", "[operational-domain]", sidb_111_cell_clk_lyt_siqad,
                   cds_sidb_111_cell_clk_lyt_siqad)
{
    const auto layout = blueprints::and_gate_111<TestType>();

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.x_dimension           = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min                 = 5.60;
    op_domain_params.x_max                 = 5.61;
    op_domain_params.x_step                = 0.01;
    op_domain_params.y_dimension           = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min                 = 5.0;
    op_domain_params.y_max                 = 5.01;
    op_domain_params.y_step                = 0.01;

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain = operational_domain_grid_search(layout, std::vector<tt>{create_and_tt()},
                                                              op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 4);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 16);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 4);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 4);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("random_sampling")
    {
        const auto op_domain = operational_domain_random_sampling(layout, std::vector<tt>{create_and_tt()}, 100,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 16);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 4);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 4);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("flood_fill")
    {
        const auto op_domain = operational_domain_flood_fill(layout, std::vector<tt>{create_and_tt()}, 1,
                                                             op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.operational_values.size() == 4);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 16);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 4);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 4);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
    SECTION("contour_tracing")
    {
        const auto op_domain = operational_domain_contour_tracing(layout, std::vector<tt>{create_and_tt()}, 1,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.operational_values.size() <= 4);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 16);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 4);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 4);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}
