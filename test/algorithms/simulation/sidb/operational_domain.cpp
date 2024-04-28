//
// Created by marcel on 27.07.23.
//

#include <catch2/catch_test_macros.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/technology/physical_constants.hpp>
#include <fiction/technology/sidb_lattice.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/phmap_utils.hpp>
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

        if (*status == operational_status::OPERATIONAL)
        {
            // std::cout << fmt::format("({}, {})\n", coord.x, coord.y);
            // CHECK(op_value == *status);
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
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;

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
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 5.1;
    op_domain_params.x_max       = 6.0;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 4.5;
    op_domain_params.y_max       = 5.4;
    op_domain_params.y_step      = 0.1;

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
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 5.1;
    op_domain_params.x_max       = 6.0;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 4.5;
    op_domain_params.y_max       = 5.4;
    op_domain_params.y_step      = 0.1;

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

TEST_CASE("Determine physical parameters for cds of SiQAD Y-shaped AND gate, 10 input combination",
          "[operational-domain]")
{
    using layout = sidb_cell_clk_lyt_siqad;

    layout lyt{{20, 10}, "AND gate"};

    lyt.assign_cell_type({-2, -1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({0, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({2, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({10, 1, 1}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 4, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 5, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({6, 7, 1}, sidb_technology::cell_type::NORMAL);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{lyt, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 4.1;
    op_domain_params.x_max       = 6.0;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 4.1;
    op_domain_params.y_max       = 6.0;
    op_domain_params.y_step      = 0.1;

    SECTION("Using the typical groundstate as given cds")
    {
        cds.assign_charge_state({-2, -1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 4, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 5, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 7, 1}, sidb_charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = find_valid_physical_parameters(cds, op_domain_params);
        CHECK(valid_parameters.suitable_physical_parameters.size() == 356);
    }

    SECTION("Using the 2nd excited charge distribution for default physical parameters as given cds")
    {
        cds.assign_charge_state({-2, -1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({0, 0, 1}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({12, 0, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({2, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({10, 1, 1}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 4, 0}, sidb_charge_state::NEUTRAL);
        cds.assign_charge_state({6, 5, 0}, sidb_charge_state::NEGATIVE);
        cds.assign_charge_state({6, 7, 1}, sidb_charge_state::NEGATIVE);
        cds.update_after_charge_change();

        const auto valid_parameters = find_valid_physical_parameters(cds, op_domain_params);
        CHECK(valid_parameters.suitable_physical_parameters.size() == 98);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.9, 5.5})
                  ->second == 1);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.8, 4.4})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.8, 4.4})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{6.0, 6.0})
                  ->second == 1);
    }
}

TEST_CASE("Determine physical parameters for cds of Bestagon AND gate, 10 input combination", "[operational-domain]")
{
    auto lyt = blueprints::bestagon_and_gate<sidb_cell_clk_lyt_siqad>();

    lyt.assign_cell_type({36, 1, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);
    lyt.assign_cell_type({0, 0, 0}, sidb_cell_clk_lyt_siqad::cell_type::EMPTY);

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    charge_distribution_surface cds{lyt, sim_params};

    operational_domain_params op_domain_params{};
    op_domain_params.sim_params  = sim_params;
    op_domain_params.x_dimension = operational_domain::sweep_parameter::EPSILON_R;
    op_domain_params.x_min       = 5.0;
    op_domain_params.x_max       = 5.9;
    op_domain_params.x_step      = 0.1;
    op_domain_params.y_dimension = operational_domain::sweep_parameter::LAMBDA_TF;
    op_domain_params.y_min       = 5.0;
    op_domain_params.y_max       = 5.9;
    op_domain_params.y_step      = 0.1;

    SECTION("Using the ground state of default physical parameters as given cds")
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

        const auto valid_parameters = find_valid_physical_parameters(cds, op_domain_params);
        CHECK(valid_parameters.suitable_physical_parameters.size() == 100);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.6, 5.0})
                  ->second == 0);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.0, 5.9})
                  ->second == 2);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.4, 5.3})
                  ->second == 1);
        CHECK(find_parameter_point_with_tolerance(valid_parameters.suitable_physical_parameters,
                                                  operational_domain::parameter_point{5.8, 5.3})
                  ->second == 0);
    }
}
