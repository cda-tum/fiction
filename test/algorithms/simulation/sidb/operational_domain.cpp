//
// Created by marcel on 27.07.23.
//

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/algorithms/simulation/sidb/is_operational.hpp>
#include <fiction/algorithms/simulation/sidb/operational_domain.hpp>
#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/truth_table_utils.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <optional>
#include <stdexcept>
#include <vector>

using namespace fiction;

void check_op_domain_params_and_operational_status(
    const operational_domain<parameter_point, operational_status>& op_domain, const operational_domain_params& params,
    const std::optional<operational_status>& status) noexcept
{
    REQUIRE(params.sweep_dimensions.size() == op_domain.dimensions.size());

    for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
    {
        CHECK(op_domain.dimensions[d] == params.sweep_dimensions[d].dimension);
    }

    for (const auto& [coord, op_value] : op_domain.operational_values)
    {
        for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
        {
            const auto& sweep_param = params.sweep_dimensions[d];
            const auto& coord_value = coord.parameters[d];

            CHECK(sweep_param.min <= coord_value);
            CHECK(sweep_param.max >= coord_value);
            CHECK(sweep_param.step > 0.0);
        }

        if (status.has_value())
        {
            if (status.value() == operational_status::OPERATIONAL)
            {
                CHECK(op_value == *status);
            }
        }
    }
}

TEST_CASE("Test parameter point", "[operational-domain]")
{
    // Test default constructor
    const parameter_point p_default;
    REQUIRE(p_default.parameters.empty());

    // Test parameterized constructor
    const std::vector<double> values = {1.0, 2.0, 3.0};
    const parameter_point     p_param(values);
    REQUIRE(p_param.parameters == values);

    // Test equality operator
    const parameter_point p1({1.0, 2.0, 3.0});
    const parameter_point p2({1.0, 2.0, 3.0});
    const parameter_point p3({1.0, 2.0, 3.0000001});

    SECTION("Equality operator - exact equality")
    {
        REQUIRE(p1 == p2);
    }

    SECTION("Equality operator - within tolerance")
    {
        REQUIRE(p1 == p3);
    }

    // Test inequality operator
    const parameter_point p4({1.0, 2.0, 3.1});
    REQUIRE(p1 != p4);

    // Test structured bindings (get<I>() method)
    SECTION("Structured bindings - valid index")
    {
        REQUIRE(p1.get<0>() == 1.0);
        REQUIRE(p1.get<1>() == 2.0);
        REQUIRE(p1.get<2>() == 3.0);
    }

    SECTION("Structured bindings - invalid index")
    {
        REQUIRE_THROWS_AS(p1.get<3>(), std::out_of_range);
    }
}

TEST_CASE("Error handling of operational domain algorithms", "[operational-domain]")
{
    const sidb_100_cell_clk_lyt_siqad lat{sidb_cell_clk_lyt_siqad{}};  // empty layout

    SECTION("invalid number of dimensions")
    {
        operational_domain_params zero_dimensional_params{};
        operational_domain_params one_dimensional_params{};
        operational_domain_params three_dimensional_params{};
        operational_domain_params four_dimensional_params{};

        // 0-dimensional
        zero_dimensional_params.sweep_dimensions = {};

        // 1-dimensional
        one_dimensional_params.sweep_dimensions = {{sweep_parameter::EPSILON_R}};

        // 3-dimensional
        three_dimensional_params.sweep_dimensions = {{sweep_parameter::EPSILON_R},
                                                     {sweep_parameter::LAMBDA_TF},
                                                     {sweep_parameter::MU_MINUS}};

        // 4-dimensional
        four_dimensional_params.sweep_dimensions = {{sweep_parameter::EPSILON_R},
                                                    {sweep_parameter::LAMBDA_TF},
                                                    {sweep_parameter::MU_MINUS},
                                                    {sweep_parameter::EPSILON_R}};

        SECTION("flood_fill")
        {
            // flood fill operates on 2-dimensional and 3-dimensional parameter spaces
            for (const auto& params : {zero_dimensional_params, one_dimensional_params, four_dimensional_params})
            {
                CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                std::invalid_argument);
            }
        }
        SECTION("contour_tracing")
        {
            // contour tracing operates only on 2-dimensional parameter spaces
            for (const auto& params :
                 {zero_dimensional_params, one_dimensional_params, three_dimensional_params, four_dimensional_params})
            {
                CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                std::invalid_argument);
            }
        }
    }

    SECTION("invalid sweep dimensions")
    {
        operational_domain_params invalid_params_1{};
        operational_domain_params invalid_params_2{};
        operational_domain_params invalid_params_3{};

        SECTION("min/max mismatch")
        {
            // 1-dimensional with invalid min/max on 1st dimension
            invalid_params_1.sweep_dimensions         = {{sweep_parameter::EPSILON_R}};
            invalid_params_1.sweep_dimensions[0].min  = 10.0;
            invalid_params_1.sweep_dimensions[0].max  = 1.0;
            invalid_params_1.sweep_dimensions[0].step = 0.1;

            // 2-dimensional with invalid min/max on 2nd dimension
            invalid_params_2.sweep_dimensions         = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
            invalid_params_2.sweep_dimensions[1].min  = 5.5;
            invalid_params_2.sweep_dimensions[1].max  = 5.4;
            invalid_params_2.sweep_dimensions[1].step = 0.1;

            // 3-dimensional with invalid min/max on 3rd dimension
            invalid_params_3.sweep_dimensions         = {{sweep_parameter::EPSILON_R},
                                                         {sweep_parameter::LAMBDA_TF},
                                                         {sweep_parameter::MU_MINUS}};
            invalid_params_3.sweep_dimensions[2].min  = -0.4;
            invalid_params_3.sweep_dimensions[2].max  = -0.5;
            invalid_params_3.sweep_dimensions[2].step = 0.01;

            for (const auto& params : {invalid_params_1, invalid_params_2, invalid_params_3})
            {
                SECTION("grid_search")
                {
                    CHECK_THROWS_AS(operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()}, params),
                                    std::invalid_argument);
                }
                SECTION("random_sampling")
                {
                    CHECK_THROWS_AS(
                        operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100, params),
                        std::invalid_argument);
                }
                SECTION("flood_fill")
                {
                    CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
                SECTION("contour_tracing")
                {
                    CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
            }
        }

        SECTION("negative step size")
        {
            // 1-dimensional with negative step size on 1st dimension
            invalid_params_1.sweep_dimensions         = {{sweep_parameter::EPSILON_R}};
            invalid_params_1.sweep_dimensions[0].min  = 1.0;
            invalid_params_1.sweep_dimensions[0].max  = 10.0;
            invalid_params_1.sweep_dimensions[0].step = -0.5;

            // 2-dimensional with negative step size on 2nd dimension
            invalid_params_2.sweep_dimensions         = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};
            invalid_params_2.sweep_dimensions[1].min  = 5.5;
            invalid_params_2.sweep_dimensions[1].max  = 5.6;
            invalid_params_2.sweep_dimensions[1].step = -0.1;

            // 3-dimensional with negative step size on 3rd dimension
            invalid_params_3.sweep_dimensions         = {{sweep_parameter::EPSILON_R},
                                                         {sweep_parameter::LAMBDA_TF},
                                                         {sweep_parameter::MU_MINUS}};
            invalid_params_3.sweep_dimensions[2].min  = -0.4;
            invalid_params_3.sweep_dimensions[2].max  = -0.5;
            invalid_params_3.sweep_dimensions[2].step = -0.01;

            for (const auto& params : {invalid_params_1, invalid_params_2, invalid_params_3})
            {
                SECTION("grid_search")
                {
                    CHECK_THROWS_AS(operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()}, params),
                                    std::invalid_argument);
                }
                SECTION("random_sampling")
                {
                    CHECK_THROWS_AS(
                        operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100, params),
                        std::invalid_argument);
                }
                SECTION("flood_fill")
                {
                    CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
                SECTION("contour_tracing")
                {
                    CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
            }
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

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.sweep_dimensions      = {{sweep_parameter::EPSILON_R}, {sweep_parameter::LAMBDA_TF}};

    operational_domain_stats op_domain_stats{};

    SECTION("operational area, only one parameter point")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 5.5;
        op_domain_params.sweep_dimensions[0].max  = 5.5;
        op_domain_params.sweep_dimensions[0].step = 0.1;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 5.0;
        op_domain_params.sweep_dimensions[1].max  = 5.0;
        op_domain_params.sweep_dimensions[1].step = 0.1;

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

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.32, -0.32, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.operational_values.size() == 1);

                // for the selected range, all samples should be within the parameters and operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations == 2);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            }
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

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.32, -0.32, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.operational_values.size() == 1);

                // for the selected range, all samples should be within the parameters and operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 200);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations > 0);
                CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            }
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.32, -0.32, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                SECTION("one random sample")
                {
                    const auto op_domain_3d = operational_domain_flood_fill(
                        lat, std::vector<tt>{create_id_tt()}, 1, op_domain_params, std::nullopt, &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.operational_values.size() == 1);

                    // for the selected range, all samples should be within the parameters and operational
                    check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                                  operational_status::OPERATIONAL);

                    CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                    CHECK(op_domain_stats.num_simulator_invocations == 2);
                    CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
                    CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
                    CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
                }
                SECTION("no random sample but given parameter point")
                {
                    const auto op_domain_3d = operational_domain_flood_fill(
                        lat, std::vector<tt>{create_id_tt()}, 0, op_domain_params,
                        std::vector<parameter_point>{parameter_point({5.5, 5.0, -0.32})}, &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.operational_values.size() == 1);

                    // for the selected range, all samples should be within the parameters and operational
                    check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                                  operational_status::OPERATIONAL);

                    CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                    CHECK(op_domain_stats.num_simulator_invocations == 2);
                    CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
                    CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
                    CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
                }
            }
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
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 5.1;
        op_domain_params.sweep_dimensions[0].max  = 6.0;
        op_domain_params.sweep_dimensions[0].step = 0.1;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 4.5;
        op_domain_params.sweep_dimensions[1].max  = 5.4;
        op_domain_params.sweep_dimensions[1].step = 0.1;

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
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

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
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 5.1;
        op_domain_params.sweep_dimensions[0].max  = 6.0;
        op_domain_params.sweep_dimensions[0].step = 0.1;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 4.5;
        op_domain_params.sweep_dimensions[1].max  = 4.9;
        op_domain_params.sweep_dimensions[1].step = 0.1;

        SECTION("grid_search")
        {
            const auto op_domain = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
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

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.35, -0.29, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.operational_values.size() == 350);

                // for the selected range, all samples should be within the parameters and operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations == 700);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 350);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 350);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            }
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

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.35, -0.29, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 100,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.operational_values.size() <= 350);

                // for the selected range, all samples should be within the parameters and operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 700);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 350);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations > 0);
                CHECK(op_domain_stats.num_operational_parameter_combinations <= 350);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            }
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 1,
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.35, -0.29, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                SECTION("one random sample")
                {
                    const auto op_domain_3d = operational_domain_flood_fill(
                        lat, std::vector<tt>{create_id_tt()}, 100, op_domain_params, std::nullopt, &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.operational_values.size() == 350);

                    // for the selected range, all samples should be within the parameters and operational
                    check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                                  operational_status::OPERATIONAL);

                    CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                    CHECK(op_domain_stats.num_simulator_invocations == 700);
                    CHECK(op_domain_stats.num_evaluated_parameter_combinations == 350);
                    CHECK(op_domain_stats.num_operational_parameter_combinations == 350);
                    CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
                }
                SECTION("no random samples, but given parameter points")
                {
                    const auto op_domain_3d =
                        operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 0, op_domain_params,
                                                      std::vector<parameter_point>{parameter_point({5.1, 4.9, -0.32}),
                                                                                   parameter_point({5.1, 4.8, -0.32})},
                                                      &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.operational_values.size() == 350);

                    // for the selected range, all samples should be within the parameters and operational
                    check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                                  operational_status::OPERATIONAL);

                    CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                    CHECK(op_domain_stats.num_simulator_invocations == 700);
                    CHECK(op_domain_stats.num_evaluated_parameter_combinations == 350);
                    CHECK(op_domain_stats.num_operational_parameter_combinations == 350);
                    CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
                    CHECK(op_domain_stats.operational_to_total_ratio == 1.0);
                }
            }
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
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 2.5;
        op_domain_params.sweep_dimensions[0].max  = 3.4;
        op_domain_params.sweep_dimensions[0].step = 0.1;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 4.5;
        op_domain_params.sweep_dimensions[1].max  = 5.4;
        op_domain_params.sweep_dimensions[1].step = 0.1;

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
            CHECK(op_domain_stats.operational_to_total_ratio == 0.0);

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.14, -0.10, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector<tt>{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.operational_values.size() == 500);

                // for the selected range, all samples should be within the parameters and non-operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::NON_OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 1000);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 500);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 500);
                CHECK(op_domain_stats.operational_to_total_ratio == 0.0);
            }
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
            CHECK(op_domain_stats.num_simulator_invocations < 10000);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 5000);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 5000);

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.14, -0.10, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector<tt>{create_id_tt()}, 5000,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct maximum size
                CHECK(op_domain_3d.operational_values.size() <= 5000);

                // for the selected range, all samples should be within the parameters and non-operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::NON_OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations < 10000);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 5000);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 5000);
            }
        }
        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 25,
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

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

            SECTION("3-dimensional")
            {
                const auto z_dimension = operational_domain_value_range{sweep_parameter::MU_MINUS, -0.14, -0.10, 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_flood_fill(
                    lat, std::vector<tt>{create_id_tt()}, 25, op_domain_params, std::nullopt, &op_domain_stats);

                // check if the operational domain has the correct maximum size
                CHECK(op_domain_3d.operational_values.size() <= 500);

                // for the selected range, all samples should be within the parameters and non-operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::NON_OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 1000);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 500);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 500);
            }
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
            CHECK(op_domain_stats.operational_to_total_ratio == 0.0);
        }
    }
    SECTION("floating-point error")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 2.5;
        op_domain_params.sweep_dimensions[0].max  = 4.4;
        op_domain_params.sweep_dimensions[0].step = 0.9;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 2.5;
        op_domain_params.sweep_dimensions[1].max  = 2.5;
        op_domain_params.sweep_dimensions[1].step = 0.1;

        SECTION("flood_fill")
        {
            const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 10000,
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.operational_values.size() == 3);

            CHECK(op_domain_stats.num_operational_parameter_combinations == 2);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 1);
        }
    }
    SECTION("semi-operational area")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 0.5;
        op_domain_params.sweep_dimensions[0].max  = 4.25;
        op_domain_params.sweep_dimensions[0].step = 0.25;

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 0.5;
        op_domain_params.sweep_dimensions[1].max  = 4.25;
        op_domain_params.sweep_dimensions[1].step = 0.25;

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
            SECTION("random sample to find operational parameter points")
            {
                const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_id_tt()}, 50,
                                                                     op_domain_params, std::nullopt, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain.operational_values.size() <= 256);

                // for the selected range, all samples should be within the parameters
                check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 512);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 256);
                CHECK(op_domain_stats.num_operational_parameter_combinations <= 80);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 176);
            }
            SECTION("no random sample but given parameter point")
            {
                const auto op_domain = operational_domain_flood_fill(
                    lat, std::vector<tt>{create_id_tt()}, 0, op_domain_params,
                    std::vector<parameter_point>{parameter_point({4.25, 4.25})}, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain.operational_values.size() == 125);

                // for the selected range, all samples should be within the parameters
                check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 125);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 80);
                CHECK_THAT(op_domain_stats.operational_to_total_ratio,
                           Catch::Matchers::WithinAbs(0.3125 - 80 / 512, 0.0));
            }
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector<tt>{create_id_tt()}, 50,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (max 10 steps in each dimension)
            CHECK(op_domain.operational_values.size() <= 256);

            // for the selected range, all samples should be within the parameters
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 512);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 256);
            CHECK(op_domain_stats.num_operational_parameter_combinations <= 80);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 176);
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

    const sidb_100_cell_clk_lyt_siqad lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.sweep_dimensions      = {{sweep_parameter::EPSILON_R, 5.1, 6.0, 0.1},
                                              {sweep_parameter::LAMBDA_TF, 4.5, 5.4, 0.1}};

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
        const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_and_tt()}, 1, op_domain_params,
                                                             std::nullopt, &op_domain_stats);

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

    const sidb_100_cell_clk_lyt_cube lat{lyt};

    sidb_simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.simulation_parameters = sim_params;
    op_domain_params.sweep_dimensions      = {{sweep_parameter::EPSILON_R, 5.1, 6.0, 0.1},
                                              {sweep_parameter::LAMBDA_TF, 4.5, 5.4, 0.1}};

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
        const auto op_domain = operational_domain_flood_fill(lat, std::vector<tt>{create_and_tt()}, 1, op_domain_params,
                                                             std::nullopt, &op_domain_stats);

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
    op_domain_params.sweep_dimensions      = {{sweep_parameter::EPSILON_R, 5.60, 5.61, 0.01},
                                              {sweep_parameter::LAMBDA_TF, 5.0, 5.01, 0.01}};

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
        SECTION("one random sample")
        {
            const auto op_domain = operational_domain_flood_fill(layout, std::vector<tt>{create_and_tt()}, 1,
                                                                 op_domain_params, std::nullopt, &op_domain_stats);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.operational_values.size() == 4);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 16);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 4);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 4);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.operational_to_total_ratio == 1.00);
        }
        SECTION("no random sample, but given operational parameter point")
        {
            const auto op_domain = operational_domain_flood_fill(
                layout, std::vector<tt>{create_and_tt()}, 0, op_domain_params,
                std::vector<parameter_point>{parameter_point({5.6, 5.0})}, &op_domain_stats);

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
