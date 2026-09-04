/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Tests for `fiction/technology/sidb/simulation/logic/operational_domain.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils/blueprints/layout_blueprints.hpp"

#include <fiction/synthesis/truth_tables.hpp>
#include <fiction/technology/sidb/cell_level_layout_conversion.hpp>
#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engine.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/technology.hpp>
#include <fiction/types.hpp>
#include <fiction/utils/math/math_utils.hpp>

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace fiction;
using namespace fiction::sidb;
using namespace fiction::sidb::model;
using namespace fiction::sidb::simulation;
using namespace fiction::sidb::simulation::logic;
using namespace fiction::synthesis;
using namespace fiction::utils::math;

template <typename OpDomain>
static void check_op_domain_params_and_operational_status(const OpDomain&                          op_domain,
                                                          const operational_domain_params&         params,
                                                          const std::optional<operational_status>& status)
{
    REQUIRE(params.sweep_dimensions.size() == op_domain.get_number_of_dimensions());

    for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
    {
        CHECK(op_domain.get_dimension(d) == params.sweep_dimensions[d].dimension);
    }

    op_domain.for_each(
        [&op_domain,  // NOLINT(clang-diagnostic-unused-lambda-capture)
         &params, &status](const auto& coord,
                           const auto& op_value)  // NOLINT(misc-unused-parameters)
        {
            for (auto d = 0u; d < params.sweep_dimensions.size(); ++d)
            {
                const auto& sweep_param = params.sweep_dimensions[d];
                const auto& coord_value = coord.get_parameters()[d];

                CHECK(sweep_param.min <= (coord_value + ERROR_MARGIN));
                CHECK(sweep_param.max >= (coord_value - ERROR_MARGIN));
                CHECK(sweep_param.step > 0.0);
            }

            if (status.has_value())
            {
                if (status.value() == operational_status::OPERATIONAL)
                {
                    if constexpr (std::is_same_v<OpDomain, critical_temperature_domain>)
                    {
                        REQUIRE(op_domain.contains(coord).has_value());
                        CHECK(std::get<1>(op_domain.contains(coord).value()) > 0.0);
                    }
                    else
                    {
                        CHECK(std::get<0>(op_value) == *status);
                    }
                }
                else
                {
                    if constexpr (std::is_same_v<OpDomain, critical_temperature_domain>)
                    {
                        REQUIRE(op_domain.contains(coord).has_value());
                        CHECK_THAT(std::get<1>(op_domain.contains(coord).value()),
                                   Catch::Matchers::WithinAbs(0.0, 0.00001));
                    }
                    else
                    {
                        CHECK(std::get<0>(op_value) == *status);
                    }
                }
            }
        });
}

TEST_CASE("Test parameter point", "[operational-domain]")
{
    // Test default constructor
    const parameter_point p_default;
    REQUIRE(p_default.get_parameters().empty());

    // Test parameterized constructor
    const std::vector<double> values = {1.0, 2.0, 3.0};
    const parameter_point     p_param(values);
    REQUIRE(p_param.get_parameters() == values);

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

    SECTION("Equal parameter points hash equally")
    {
        // every hash-based container relies on this invariant. Comparing with a tolerance does not establish it on
        // its own, since two values within the tolerance can still fall either side of a cell boundary
        const std::hash<parameter_point> hash{};

        REQUIRE(p1 == p3);
        CHECK(hash(p1) == hash(p3));
        CHECK(hash(p1) == hash(p2));
    }

    SECTION("Negative parameter values are hashable")
    {
        // a `MU_MINUS` sweep produces negative values throughout. Casting them straight to an unsigned type is
        // undefined behavior
        const parameter_point negative({-0.32, -0.5, -1.0});

        const std::hash<parameter_point> hash{};

        CHECK(hash(negative) == hash(parameter_point{{-0.32, -0.5, -1.0}}));
        CHECK(hash(negative) != hash(parameter_point{{0.32, 0.5, 1.0}}));

        // and they must survive a round trip through a hash-based container
        const std::unordered_set<parameter_point> points{negative, parameter_point{{-0.28, -0.5, -1.0}}};

        CHECK(points.size() == 2);
        CHECK(points.count(negative) == 1);
    }
}

TEST_CASE("operational_domain class member functions", "[operational-domain]")
{
    operational_domain opdom{};

    CHECK(opdom.empty());
    CHECK(opdom.get_number_of_dimensions() == 0);
    REQUIRE_THROWS_AS(opdom.get_dimension(0), std::out_of_range);
    REQUIRE_THROWS_AS(opdom.get_dimension(1), std::out_of_range);

    opdom.add_dimension(sweep_parameter::EPSILON_R);
    CHECK(opdom.get_number_of_dimensions() == 1);
    CHECK(opdom.get_dimension(0) == sweep_parameter::EPSILON_R);
    REQUIRE_THROWS_AS(opdom.get_dimension(1), std::out_of_range);

    opdom = operational_domain({sweep_parameter::LAMBDA_TF, sweep_parameter::MU_MINUS});
    CHECK(opdom.get_number_of_dimensions() == 2);
    CHECK(opdom.get_dimension(0) == sweep_parameter::LAMBDA_TF);
    CHECK(opdom.get_dimension(1) == sweep_parameter::MU_MINUS);
}

TEST_CASE("Error handling of operational domain algorithms", "[operational-domain]")
{
    const layout lat{};  // empty layout

    SECTION("invalid number of dimensions")
    {
        operational_domain_params zero_dimensional_params{};
        operational_domain_params one_dimensional_params{};

        // 0-dimensional
        zero_dimensional_params.sweep_dimensions = {};

        // 1-dimensional
        one_dimensional_params.sweep_dimensions = {{.dimension = sweep_parameter::EPSILON_R}};

        SECTION("flood_fill")
        {
            // flood fill needs a neighborhood to expand over, so it requires at least two dimensions. Three and more
            // are supported; the three-dimensional cases are covered on an actual gate below
            for (const auto& params : {zero_dimensional_params, one_dimensional_params})
            {
                CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, params),
                                std::invalid_argument);
            }
        }
        SECTION("contour_tracing")
        {
            // contour tracing needs a boundary to trace, so it requires at least two dimensions
            for (const auto& params : {zero_dimensional_params, one_dimensional_params})
            {
                CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1, params),
                                std::invalid_argument);
            }
        }
    }

    SECTION("operational domain sketch preconditions")
    {
        // the sketch determines the operational status by filtering alone. Its filtering steps are only defined when
        // kinks are rejected, and they enumerate the charge configurations of the canvas that the layout's `LOGIC`
        // cells define. Without either, the sketch would silently fall back to a full simulation of the whole
        // parameter space, which is the exhaustive cost it exists to avoid
        const auto and_gate = blueprints::bestagon_and_gate();

        const auto& and_lat = and_gate;

        REQUIRE(and_lat.num_cells_of_type(sidb_technology::cell_type::LOGIC) > 0);

        operational_domain_params sketch_params{};
        sketch_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;
        sketch_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        sketch_params.sweep_dimensions                = {
            {.dimension = sweep_parameter::EPSILON_R, .min = 5.5, .max = 5.6, .step = 0.1},
            {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.0, .max = 5.1, .step = 0.1}};

        SECTION("kinks must be rejected")
        {
            auto tolerating_params = sketch_params;
            tolerating_params.operational_params.op_condition =
                is_operational_params::operational_condition::TOLERATE_KINKS;

            CHECK_THROWS_AS(operational_domain_grid_search(and_lat, std::vector{create_and_tt()}, tolerating_params),
                            std::invalid_argument);
            CHECK_THROWS_AS(
                operational_domain_random_sampling(and_lat, std::vector{create_and_tt()}, 1, tolerating_params),
                std::invalid_argument);
            CHECK_THROWS_AS(operational_domain_flood_fill(and_lat, std::vector{create_and_tt()}, 1, tolerating_params),
                            std::invalid_argument);
            CHECK_THROWS_AS(
                operational_domain_contour_tracing(and_lat, std::vector{create_and_tt()}, 1, tolerating_params),
                std::invalid_argument);
        }

        SECTION("the layout must have canvas cells")
        {
            // `lat` is the empty layout declared above and therefore has no `LOGIC` cells
            CHECK_THROWS_AS(operational_domain_grid_search(lat, std::vector{create_id_tt()}, sketch_params),
                            std::invalid_argument);
        }

        SECTION("both preconditions met")
        {
            CHECK_NOTHROW(operational_domain_grid_search(and_lat, std::vector{create_and_tt()}, sketch_params));
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
            invalid_params_1.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R}};
            invalid_params_1.sweep_dimensions[0].min  = 10.0;
            invalid_params_1.sweep_dimensions[0].max  = 1.0;
            invalid_params_1.sweep_dimensions[0].step = 0.1;

            // 2-dimensional with invalid min/max on 2nd dimension
            invalid_params_2.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R},
                                                         {.dimension = sweep_parameter::LAMBDA_TF}};
            invalid_params_2.sweep_dimensions[1].min  = 5.5;
            invalid_params_2.sweep_dimensions[1].max  = 5.4;
            invalid_params_2.sweep_dimensions[1].step = 0.1;

            // 3-dimensional with invalid min/max on 3rd dimension
            invalid_params_3.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R},
                                                         {.dimension = sweep_parameter::LAMBDA_TF},
                                                         {.dimension = sweep_parameter::MU_MINUS}};
            invalid_params_3.sweep_dimensions[2].min  = -0.4;
            invalid_params_3.sweep_dimensions[2].max  = -0.5;
            invalid_params_3.sweep_dimensions[2].step = 0.01;

            for (const auto& params : {invalid_params_1, invalid_params_2, invalid_params_3})
            {
                SECTION("grid_search")
                {
                    CHECK_THROWS_AS(operational_domain_grid_search(lat, std::vector{create_id_tt()}, params),
                                    std::invalid_argument);
                }
                SECTION("random_sampling")
                {
                    CHECK_THROWS_AS(operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100, params),
                                    std::invalid_argument);
                }
                SECTION("flood_fill")
                {
                    CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
                SECTION("contour_tracing")
                {
                    CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
            }
        }

        SECTION("negative step size")
        {
            // 1-dimensional with negative step size on 1st dimension
            invalid_params_1.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R}};
            invalid_params_1.sweep_dimensions[0].min  = 1.0;
            invalid_params_1.sweep_dimensions[0].max  = 10.0;
            invalid_params_1.sweep_dimensions[0].step = -0.5;

            // 2-dimensional with negative step size on 2nd dimension
            invalid_params_2.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R},
                                                         {.dimension = sweep_parameter::LAMBDA_TF}};
            invalid_params_2.sweep_dimensions[1].min  = 5.5;
            invalid_params_2.sweep_dimensions[1].max  = 5.6;
            invalid_params_2.sweep_dimensions[1].step = -0.1;

            // 3-dimensional with negative step size on 3rd dimension
            invalid_params_3.sweep_dimensions         = {{.dimension = sweep_parameter::EPSILON_R},
                                                         {.dimension = sweep_parameter::LAMBDA_TF},
                                                         {.dimension = sweep_parameter::MU_MINUS}};
            invalid_params_3.sweep_dimensions[2].min  = -0.4;
            invalid_params_3.sweep_dimensions[2].max  = -0.5;
            invalid_params_3.sweep_dimensions[2].step = -0.01;

            for (const auto& params : {invalid_params_1, invalid_params_2, invalid_params_3})
            {
                SECTION("grid_search")
                {
                    CHECK_THROWS_AS(operational_domain_grid_search(lat, std::vector{create_id_tt()}, params),
                                    std::invalid_argument);
                }
                SECTION("random_sampling")
                {
                    CHECK_THROWS_AS(operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100, params),
                                    std::invalid_argument);
                }
                SECTION("flood_fill")
                {
                    CHECK_THROWS_AS(operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
                SECTION("contour_tracing")
                {
                    CHECK_THROWS_AS(operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1, params),
                                    std::invalid_argument);
                }
            }
        }
    }
}

TEST_CASE("SiQAD OR gate", "[operational-domain]")
{
    const auto lyt = blueprints::siqad_or_gate();

    operational_domain_stats op_domain_stats{};

    operational_domain_params op_domain_params{};

    op_domain_params.sweep_dimensions = {{.dimension = sweep_parameter::EPSILON_R, .min = 7, .max = 8, .step = 0.01},
                                         {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.5, .max = 6, .step = 0.01}};

    op_domain_params.operational_params.sim_params.mu_minus                                                   = -0.28;
    op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5;

    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::TOLERATE_KINKS;

    const auto op_domain =
        operational_domain_grid_search(lyt, std::vector{create_or_tt()}, op_domain_params, &op_domain_stats);

    check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);
}

TEST_CASE("Three-dimensional operational domain sketch", "[operational-domain]")
{
    // the sketch determines the operational status by filtering alone, which is a property of a single parameter
    // point and therefore independent of how many dimensions are swept. These cases pin that contract in three
    // dimensions, where the third dimension is the only remaining sweep parameter, `MU_MINUS`
    const layout lat{blueprints::bestagon_and_gate()};

    operational_domain_params params{};
    params.operational_params.sim_params   = simulation_parameters{2, -0.32};
    params.operational_params.sim_engine   = engine::QUICKEXACT;
    params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
    params.sweep_dimensions = {{.dimension = sweep_parameter::EPSILON_R, .min = 5.5, .max = 5.7, .step = 0.1},
                               {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.0, .max = 5.2, .step = 0.1},
                               {.dimension = sweep_parameter::MU_MINUS, .min = -0.32, .max = -0.30, .step = 0.02}};

    // 3 x 3 x 2 parameter points
    constexpr std::size_t num_parameter_points = 18;

    auto simulation_params = params;
    simulation_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::SIMULATION_ONLY;

    auto sketch_params = params;
    sketch_params.operational_params.strategy_to_analyze_operational_status =
        is_operational_params::operational_analysis_strategy::FILTER_ONLY;

    const auto simulated = operational_domain_grid_search(lat, std::vector{create_and_tt()}, simulation_params);

    REQUIRE(simulated.size() == num_parameter_points);

    SECTION("grid search yields a superset of the simulated operational domain")
    {
        // the sketch never rejects an operational point, it only fails to reject some non-operational ones. So every
        // point that is operational under simulation must be operational in the sketch. This is the sketch's actual
        // guarantee; the number of points it marks operational depends on how well the filters bite and is not a
        // contract
        operational_domain_stats stats{};

        const auto sketch = operational_domain_grid_search(lat, std::vector{create_and_tt()}, sketch_params, &stats);

        CHECK(sketch.size() == num_parameter_points);
        CHECK(stats.num_evaluated_parameter_combinations == num_parameter_points);

        simulated.for_each(
            [&sketch](const auto& parameter_point, const auto& status)
            {
                if (std::get<0>(status) == operational_status::OPERATIONAL)
                {
                    const auto sketched = sketch.contains(parameter_point);

                    REQUIRE(sketched.has_value());
                    CHECK(std::get<0>(sketched.value()) == operational_status::OPERATIONAL);
                }
            });
    }

    SECTION("random sampling agrees with a direct operational check")
    {
        const auto sketch = operational_domain_random_sampling(lat, std::vector{create_and_tt()}, 8, sketch_params);

        CHECK(sketch.size() <= 8);

        const auto input_wires = detect_bdl_wires(
            lat, sketch_params.operational_params.input_bdl_iterator_params.bdl_wire_params, bdl_wire_selection::INPUT);
        const auto output_wires =
            detect_bdl_wires(lat, sketch_params.operational_params.input_bdl_iterator_params.bdl_wire_params,
                             bdl_wire_selection::OUTPUT);

        sketch.for_each(
            [&](const auto& parameter_point, const auto& status)
            {
                auto point_params = sketch_params.operational_params;

                point_params.sim_params.epsilon_r = parameter_point.get_parameters().at(0);
                point_params.sim_params.lambda_tf = parameter_point.get_parameters().at(1);
                point_params.sim_params.mu_minus  = parameter_point.get_parameters().at(2);

                const auto [expected, _] =
                    is_operational(lat, std::vector{create_and_tt()}, point_params, input_wires, output_wires);

                CHECK(std::get<0>(status) == expected);
            });
    }

    SECTION("flood fill yields a subset of the grid-searched sketch")
    {
        // flood fill traces the boundary of the sketch region rather than of the operational region, which is sound
        // because the sketch region is a superset of the operational one. It can only miss a component that no
        // random sample landed in, which is the failure mode it already has under simulation
        const auto grid_sketch  = operational_domain_grid_search(lat, std::vector{create_and_tt()}, sketch_params);
        const auto flood_sketch = operational_domain_flood_fill(lat, std::vector{create_and_tt()}, 4, sketch_params);

        CHECK(flood_sketch.size() <= num_parameter_points);

        flood_sketch.for_each(
            [&grid_sketch](const auto& parameter_point, const auto& status)
            {
                const auto grid_status = grid_sketch.contains(parameter_point);

                REQUIRE(grid_status.has_value());
                CHECK(std::get<0>(status) == std::get<0>(grid_status.value()));
            });
    }
}

TEST_CASE("Three-dimensional contour tracing", "[operational-domain]")
{
    // in three dimensions the boundary of the operational region is a surface rather than a curve, so it is collected
    // by a breadth-first search over the operational points that border a non-operational one instead of being walked
    // in clockwise order. What the algorithm promises is unchanged: every point it reports was either simulated or
    // enclosed by the traced boundary
    const layout lat{blueprints::bestagon_and_gate()};

    operational_domain_params params{};
    params.operational_params.sim_params = simulation_parameters{2, -0.32};
    params.operational_params.sim_engine = engine::QUICKEXACT;
    params.sweep_dimensions = {{.dimension = sweep_parameter::EPSILON_R, .min = 5.4, .max = 5.8, .step = 0.1},
                               {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.9, .max = 5.3, .step = 0.1},
                               {.dimension = sweep_parameter::MU_MINUS, .min = -0.32, .max = -0.30, .step = 0.02}};

    const auto exhaustive = operational_domain_grid_search(lat, std::vector{create_and_tt()}, params);

    REQUIRE(exhaustive.size() == 50);

    SECTION("agrees with the grid search on every point it reports")
    {
        operational_domain_stats stats{};

        const auto traced = operational_domain_contour_tracing(lat, std::vector{create_and_tt()}, 8, params, &stats);

        // tracing only samples the boundary and its surroundings, so it must not cover the whole space
        CHECK(traced.size() <= exhaustive.size());
        CHECK(stats.num_evaluated_parameter_combinations <= exhaustive.size());

        traced.for_each(
            [&exhaustive](const auto& parameter_point, const auto& status)
            {
                const auto expected = exhaustive.contains(parameter_point);

                REQUIRE(expected.has_value());
                CHECK(std::get<0>(status) == std::get<0>(expected.value()));
            });
    }

    SECTION("carries the operational domain sketch")
    {
        auto sketch_params                            = params;
        sketch_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;
        sketch_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const auto grid_sketch = operational_domain_grid_search(lat, std::vector{create_and_tt()}, sketch_params);
        const auto traced_sketch =
            operational_domain_contour_tracing(lat, std::vector{create_and_tt()}, 8, sketch_params);

        traced_sketch.for_each(
            [&grid_sketch](const auto& parameter_point, const auto& status)
            {
                const auto expected = grid_sketch.contains(parameter_point);

                REQUIRE(expected.has_value());
                CHECK(std::get<0>(status) == std::get<0>(expected.value()));
            });
    }
}

TEST_CASE("Sampling zero points does not divide by zero", "[operational-domain]")
{
    // the parallel helpers slice their work across `min(number_of_threads, work_size)` threads and derive the slice
    // size by dividing by that count, which is zero when there is no work at all. `samples = 0` reaches it through
    // public API
    const layout lat{blueprints::siqad_and_gate()};

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = simulation_parameters{2, -0.32};
    op_domain_params.operational_params.sim_engine = engine::QUICKEXACT;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 5.5, .max = 5.7, .step = 0.1},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.0, .max = 5.2, .step = 0.1}};

    operational_domain_stats op_domain_stats{};

    const auto op_domain =
        operational_domain_random_sampling(lat, std::vector{create_and_tt()}, 0, op_domain_params, &op_domain_stats);

    CHECK(op_domain.empty());
    CHECK(op_domain_stats.num_evaluated_parameter_combinations == 0);
}

TEST_CASE("Pinning the thread count does not change the operational domain", "[operational-domain]")
{
    const layout lat{blueprints::siqad_and_gate()};

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = simulation_parameters{2, -0.32};
    op_domain_params.operational_params.sim_engine = engine::QUICKEXACT;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 5.5, .max = 5.7, .step = 0.1},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.0, .max = 5.2, .step = 0.1}};

    operational_domain_stats default_stats{};

    const auto default_domain =
        operational_domain_grid_search(lat, std::vector{create_and_tt()}, op_domain_params, &default_stats);

    // one worker thread must produce exactly the same domain as the hardware-thread default
    op_domain_params.number_of_threads = 1;

    operational_domain_stats single_threaded_stats{};

    const auto single_threaded_domain =
        operational_domain_grid_search(lat, std::vector{create_and_tt()}, op_domain_params, &single_threaded_stats);

    CHECK(single_threaded_domain.size() == default_domain.size());
    CHECK(single_threaded_stats.num_simulator_invocations == default_stats.num_simulator_invocations);
    CHECK(single_threaded_stats.num_evaluated_parameter_combinations ==
          default_stats.num_evaluated_parameter_combinations);
    CHECK(single_threaded_stats.num_operational_parameter_combinations ==
          default_stats.num_operational_parameter_combinations);
    CHECK(single_threaded_stats.num_non_operational_parameter_combinations ==
          default_stats.num_non_operational_parameter_combinations);

    // a thread count of zero is treated as one rather than dividing by zero
    op_domain_params.number_of_threads = 0;

    operational_domain_stats zero_stats{};

    const auto zero_domain =
        operational_domain_grid_search(lat, std::vector{create_and_tt()}, op_domain_params, &zero_stats);

    CHECK(zero_domain.size() == default_domain.size());
    CHECK(zero_stats.num_evaluated_parameter_combinations == default_stats.num_evaluated_parameter_combinations);
}

// NOLINTNEXTLINE(*-function-size)
TEST_CASE("BDL wire operational domain computation", "[operational-domain]")
{

    layout lyt{lattice::si_100_2x1(), "BDL wire"};

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

    const auto& lat = lyt;

    simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {{.dimension = sweep_parameter::EPSILON_R},
                                                      {.dimension = sweep_parameter::LAMBDA_TF}};

    CHECK(op_domain_params.sweep_dimensions[0].dimension == sweep_parameter::EPSILON_R);
    CHECK(op_domain_params.sweep_dimensions[1].dimension == sweep_parameter::LAMBDA_TF);

    operational_domain_stats op_domain_stats{};

    SECTION("operational area, only one parameter point")
    {
        // set x-dimension
        op_domain_params.sweep_dimensions[0].min  = 5.5;
        op_domain_params.sweep_dimensions[0].max  = 5.5;
        op_domain_params.sweep_dimensions[0].step = 0.1;

        CHECK(op_domain_params.sweep_dimensions[0].min == 5.5);
        CHECK(op_domain_params.sweep_dimensions[0].max == 5.5);
        CHECK(op_domain_params.sweep_dimensions[0].step == 0.1);

        // set y-dimension
        op_domain_params.sweep_dimensions[1].min  = 5.0;
        op_domain_params.sweep_dimensions[1].max  = 5.0;
        op_domain_params.sweep_dimensions[1].step = 0.1;

        CHECK(op_domain_params.sweep_dimensions[1].min == 5.0);
        CHECK(op_domain_params.sweep_dimensions[1].max == 5.0);
        CHECK(op_domain_params.sweep_dimensions[1].step == 0.1);

        SECTION("grid_search")
        {
            const auto op_domain =
                operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("reject kinks")
            {
                op_domain_params.operational_params.op_condition =
                    is_operational_params::operational_condition::REJECT_KINKS;

                const auto op_domain_kinks = operational_domain_grid_search(lat, std::vector{create_id_tt()},
                                                                            op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_kinks.size() == 1);

                // for the selected range, all samples should be within the parameters and operational
                check_op_domain_params_and_operational_status(op_domain_kinks, op_domain_params,
                                                              operational_status::OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations == 2);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
            }

            SECTION("3-dimensional")
            {
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.32,
                                                                            .max       = -0.32,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.size() == 1);

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
            const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 1);

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
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.32,
                                                                            .max       = -0.32,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.size() == 1);

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
            const auto op_domain =
                operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 1);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 2);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 1);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 1);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("3-dimensional")
            {
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.32,
                                                                            .max       = -0.32,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                SECTION("one random sample")
                {
                    const auto op_domain_3d = operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1,
                                                                            op_domain_params, &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.size() == 1);

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
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 1);

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
            const auto op_domain =
                operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 100);

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
            const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() <= 100);

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
            const auto op_domain =
                operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 100);

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
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() <= 100);

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
            const auto op_domain =
                operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("3-dimensional")
            {
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.35,
                                                                            .max       = -0.29,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.size() == 350);

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
            const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() <= 100);

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
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.35,
                                                                            .max       = -0.29,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.size() <= 350);

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
            const auto op_domain =
                operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 1, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 50);

            // for the selected range, all samples should be within the parameters and operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations == 100);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 50);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 50);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);

            SECTION("3-dimensional")
            {
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.35,
                                                                            .max       = -0.29,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                SECTION("one random sample")
                {
                    const auto op_domain_3d = operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 100,
                                                                            op_domain_params, &op_domain_stats);

                    // check if the operational domain has the correct size
                    CHECK(op_domain_3d.size() == 350);

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
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() <= 50);

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
            const auto op_domain =
                operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.size() == 100);

            // for the selected range, all samples should be within the parameters and non-operational
            check_op_domain_params_and_operational_status(op_domain, op_domain_params,
                                                          operational_status::NON_OPERATIONAL);

            CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
            CHECK(op_domain_stats.num_simulator_invocations <= 200);
            CHECK(op_domain_stats.num_evaluated_parameter_combinations == 100);
            CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
            CHECK(op_domain_stats.num_non_operational_parameter_combinations == 100);

            SECTION("3-dimensional")
            {
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.14,
                                                                            .max       = -0.10,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_grid_search(lat, std::vector{create_id_tt()},
                                                                         op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain_3d.size() == 500);

                // for the selected range, all samples should be within the parameters and non-operational
                check_op_domain_params_and_operational_status(op_domain_3d, op_domain_params,
                                                              operational_status::NON_OPERATIONAL);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 1000);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations == 500);
                CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations == 500);
            }
        }
        SECTION("random_sampling")
        {
            const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 5000,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.size() <= 5000);

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
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.14,
                                                                            .max       = -0.10,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 5000,
                                                                             op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct maximum size
                CHECK(op_domain_3d.size() <= 5000);

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
            const auto op_domain =
                operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 25, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.size() <= 100);

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
                constexpr auto z_dimension = operational_domain_value_range{.dimension = sweep_parameter::MU_MINUS,
                                                                            .min       = -0.14,
                                                                            .max       = -0.10,
                                                                            .step      = 0.01};

                op_domain_params.sweep_dimensions.push_back(z_dimension);

                const auto op_domain_3d = operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 25,
                                                                        op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct maximum size
                CHECK(op_domain_3d.size() <= 500);

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
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 25,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.size() <= 25);

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
            const auto op_domain = operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 10000,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size
            CHECK(op_domain.size() == 3);

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
            const auto op_domain =
                operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (16 steps in each dimension)
            CHECK(op_domain.size() == 256);

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
            const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_id_tt()}, 100,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct maximum size
            CHECK(op_domain.size() <= 100);

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
                const auto op_domain = operational_domain_flood_fill(lat, std::vector{create_id_tt()}, 50,
                                                                     op_domain_params, &op_domain_stats);

                // check if the operational domain has the correct size
                CHECK(op_domain.size() <= 256);

                // for the selected range, all samples should be within the parameters
                check_op_domain_params_and_operational_status(op_domain, op_domain_params, std::nullopt);

                CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
                CHECK(op_domain_stats.num_simulator_invocations <= 512);
                CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 256);
                CHECK(op_domain_stats.num_operational_parameter_combinations <= 80);
                CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 176);
            }
        }
        SECTION("contour_tracing")
        {
            const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_id_tt()}, 50,
                                                                      op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (max 10 steps in each dimension)
            CHECK(op_domain.size() <= 256);

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

TEST_CASE("Contour tracing does not retrace an already enclosed area", "[operational-domain]")
{

    layout lyt{lattice::si_100_2x1(), "BDL wire"};

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

    const auto& lat = lyt;

    simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    // 16 x 16 steps; the operational area is a single connected island of 80 parameter points
    op_domain_params.sweep_dimensions = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 0.5, .max = 4.25, .step = 0.25},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 0.5, .max = 4.25, .step = 0.25}};

    // ground truth to compare the contour tracing results against
    const auto grid_search_domain = operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params);

    // the random samples are drawn from an unseeded generator; repeat to make the assertions meaningful
    for (auto i = 0; i < 5; ++i)
    {
        operational_domain_stats op_domain_stats{};

        sidb::simulation::logic::detail::operational_domain_impl<tt, operational_domain> impl{
            lat, std::vector{create_id_tt()}, op_domain_params, op_domain_stats};

        const auto op_domain = impl.contour_tracing(50);

        const auto inferred_points = impl.inferred_operational_parameter_points();

        // the contour must have been traced at all
        REQUIRE(op_domain_stats.num_operational_parameter_combinations > 0);

        // once a contour has been traced, the area it encloses must be marked as inferred operational so that further
        // samples landing inside it do not trigger another trace of the very same contour
        CHECK(!inferred_points.empty());

        // the inference must not leak out of the traced contour, i.e., it must never assume a non-operational
        // parameter point to be operational
        for (const auto& pp : inferred_points)
        {
            const auto ground_truth = grid_search_domain.contains(pp);

            REQUIRE(ground_truth.has_value());

            // the `REQUIRE` above already aborts on an empty optional, but the static analyzer cannot see that
            if (ground_truth.has_value())
            {
                CHECK(std::get<0>(*ground_truth) == operational_status::OPERATIONAL);
            }
        }

        // inferred points are never added to the operational domain, so every reported status must match the ground
        // truth
        op_domain.for_each(
            [&grid_search_domain](const auto& coord, const auto& op_value)
            {
                const auto ground_truth = grid_search_domain.contains(coord);

                REQUIRE(ground_truth.has_value());
                CHECK(std::get<0>(op_value) == std::get<0>(ground_truth.value()));
            });
    }
}

TEST_CASE("Parallel flood fill yields deterministic results", "[operational-domain]")
{

    layout lyt{lattice::si_100_2x1(), "BDL wire"};

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

    const auto& lat = lyt;

    simulation_parameters sim_params{};
    sim_params.base = 2;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    // 16 x 16 steps; the operational area is a single connected island of 80 parameter points
    op_domain_params.sweep_dimensions = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 0.5, .max = 4.25, .step = 0.25},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 0.5, .max = 4.25, .step = 0.25}};

    // ground truth to compare the flood fill results against
    const auto grid_search_domain = operational_domain_grid_search(lat, std::vector{create_id_tt()}, op_domain_params);

    // collect all operational parameter points of the ground truth in a deterministic order
    std::vector<parameter_point> operational_points{};

    grid_search_domain.for_each(
        [&operational_points](const auto& coord, const auto& op_value)
        {
            if (std::get<0>(op_value) == operational_status::OPERATIONAL)
            {
                operational_points.push_back(coord);
            }
        });

    REQUIRE(operational_points.size() == 80);

    std::ranges::sort(operational_points, std::ranges::less{}, &parameter_point::get_parameters);

    // seeding the flood fill with a known operational point and taking no random samples makes it fully
    // deterministic, so any run-to-run difference can only stem from the parallelization
    const auto& seed_point = operational_points.front();

    std::optional<std::vector<std::pair<parameter_point, operational_status>>> reference{};

    for (auto i = 0; i < 10; ++i)
    {
        operational_domain_stats op_domain_stats{};

        sidb::simulation::logic::detail::operational_domain_impl<tt, operational_domain> impl{
            lat, std::vector{create_id_tt()}, op_domain_params, op_domain_stats};

        const auto op_domain = impl.flood_fill(0, seed_point);

        std::vector<std::pair<parameter_point, operational_status>> result{};

        op_domain.for_each([&result](const auto& coord, const auto& op_value)
                           { result.emplace_back(coord, std::get<0>(op_value)); });

        std::ranges::sort(result, std::ranges::less{}, [](const auto& entry) { return entry.first.get_parameters(); });

        // every point the parallel flood fill reports must match the ground truth
        for (const auto& [pp, status] : result)
        {
            const auto ground_truth = grid_search_domain.contains(pp);

            REQUIRE(ground_truth.has_value());

            // the `REQUIRE` above already aborts on an empty optional, but the static analyzer cannot see that
            if (ground_truth.has_value())
            {
                CHECK(status == std::get<0>(*ground_truth));
            }
        }

        // the single operational island is connected, so flood fill must find all 80 of its points
        CHECK(op_domain_stats.num_operational_parameter_combinations == 80);

        // no parameter point may be simulated twice
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == op_domain.size());

        if (!reference.has_value())
        {
            reference = result;
        }
        else
        {
            // the explored set does not depend on the order of exploration, so every run must produce the exact same
            // result regardless of how the work happened to be distributed among the threads
            CHECK(result == *reference);
        }
    }
}

TEST_CASE("SiQAD's AND gate operational domain computation", "[operational-domain]")
{

    layout lyt{lattice::si_100_2x1(), "AND gate"};

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

    const auto& lat = lyt;

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.28;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 5.1, .max = 6.0, .step = 0.1},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.5, .max = 5.4, .step = 0.1}};

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain =
            operational_domain_grid_search(lat, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 100);

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
        const auto op_domain = operational_domain_random_sampling(lat, std::vector{create_and_tt()}, 100,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.size() <= 100);

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
            operational_domain_flood_fill(lat, std::vector{create_and_tt()}, 1, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 100);

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
        const auto op_domain = operational_domain_contour_tracing(lat, std::vector{create_and_tt()}, 1,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.size() <= 100);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 400);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 100);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}

TEST_CASE("AND gate on the H-Si(111)-1x1 surface", "[operational-domain]")
{
    const auto layout = blueprints::and_gate_111();

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 5.60, .max = 5.61, .step = 0.01},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 5.0, .max = 5.01, .step = 0.01}};

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain =
            operational_domain_grid_search(layout, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 4);

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
        const auto op_domain = operational_domain_random_sampling(layout, std::vector{create_and_tt()}, 100,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.size() <= 100);

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
            const auto op_domain = operational_domain_flood_fill(layout, std::vector{create_and_tt()}, 1,
                                                                 op_domain_params, &op_domain_stats);

            // check if the operational domain has the correct size (10 steps in each dimension)
            CHECK(op_domain.size() == 4);

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
        const auto op_domain = operational_domain_contour_tracing(layout, std::vector{create_and_tt()}, 1,
                                                                  op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.size() <= 4);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 16);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 4);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 4);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}

TEST_CASE("AND gate with Bestagon shape and kink states at default physical parameters", "[operational-domain]")
{
    const auto layout = blueprints::and_gate_with_kink_states();

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 4.0, .max = 6.0, .step = 0.4},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.0, .max = 6.0, .step = 0.4}};

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search, allow kinks")
    {
        const auto op_domain =
            operational_domain_grid_search(layout, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 36);

        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 36);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 3);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 33);
    }

    SECTION("grid_search, reject kinks")
    {
        op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

        const auto op_domain =
            operational_domain_grid_search(layout, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 36);

        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 36);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 36);
    }
}

TEST_CASE("Grid search to determine the operational domain. The operational status is determined by physical "
          "simulation and the efficient but approximate method of pruning only.",
          "[operational-domain]")
{
    const auto layout = blueprints::bestagon_and();

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 4.0, .max = 6.0, .step = 0.4},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.0, .max = 6.0, .step = 0.4}};

    op_domain_params.operational_params.op_condition = is_operational_params::operational_condition::REJECT_KINKS;

    operational_domain_stats op_domain_stats{};

    SECTION("grid search, determine operational status with physical simulation")
    {
        const auto op_domain =
            operational_domain_grid_search(layout, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 36);

        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 36);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 5);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 31);
    }

    SECTION("grid search, determine operational status with only pruning")
    {
        op_domain_params.operational_params.strategy_to_analyze_operational_status =
            is_operational_params::operational_analysis_strategy::FILTER_ONLY;

        const auto op_domain =
            operational_domain_grid_search(layout, std::vector{create_and_tt()}, op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 36);

        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 36);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 5);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 31);

        // this test was created to cover a special case: Strange behavior was observed when no clone was used in the
        // `is_physical_validity_feasible` function.
        op_domain.for_each(
            [](const auto& pp, [[maybe_unused]] const auto& status)
            {
                CHECK(pp.get_parameters()[0] >= 4.0);
                CHECK(pp.get_parameters()[1] >= 4.0);
            });
    }
}

TEST_CASE("critical_temperature_domain class member functions", "[operational-domain]")
{
    critical_temperature_domain ctdom{};

    CHECK(ctdom.empty());
    CHECK(ctdom.get_number_of_dimensions() == 0);
    REQUIRE_THROWS_AS(ctdom.get_dimension(0), std::out_of_range);
    REQUIRE_THROWS_AS(ctdom.get_dimension(1), std::out_of_range);

    ctdom.add_dimension(sweep_parameter::EPSILON_R);
    CHECK(ctdom.get_number_of_dimensions() == 1);
    CHECK(ctdom.get_dimension(0) == sweep_parameter::EPSILON_R);
    REQUIRE_THROWS_AS(ctdom.get_dimension(1), std::out_of_range);

    ctdom = critical_temperature_domain({sweep_parameter::LAMBDA_TF, sweep_parameter::MU_MINUS});
    CHECK(ctdom.get_number_of_dimensions() == 2);
    CHECK(ctdom.get_dimension(0) == sweep_parameter::LAMBDA_TF);
    CHECK(ctdom.get_dimension(1) == sweep_parameter::MU_MINUS);

    ctdom = critical_temperature_domain(
        {sweep_parameter::LAMBDA_TF, sweep_parameter::EPSILON_R, sweep_parameter::MU_MINUS});
    CHECK(ctdom.get_number_of_dimensions() == 3);
    CHECK(ctdom.get_dimension(0) == sweep_parameter::LAMBDA_TF);
    CHECK(ctdom.get_dimension(1) == sweep_parameter::EPSILON_R);
    CHECK(ctdom.get_dimension(2) == sweep_parameter::MU_MINUS);
}

TEST_CASE("Bestagon AND gate operational domain and temperature computation, using siqad coordinates",
          "[operational-domain]")
{
    const auto lyt = blueprints::bestagon_and();

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions              = {
        {.dimension = sweep_parameter::EPSILON_R, .min = 5.6, .max = 5.8, .step = 0.1},
        {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.9, .max = 5.1, .step = 0.1}};

    operational_domain_stats op_domain_stats{};

    SECTION("grid_search")
    {
        const auto op_domain = critical_temperature_domain_grid_search(lyt, std::vector{create_and_tt()},
                                                                       op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 9);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 36);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 9);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 9);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
        CHECK_THAT(op_domain.minimum_ct(), Catch::Matchers::WithinAbs(54.40, 0.01));
        CHECK_THAT(op_domain.maximum_ct(), Catch::Matchers::WithinAbs(60.21, 0.01));
    }
    SECTION("random_sampling in non-operational regime")
    {
        op_domain_params.sweep_dimensions = {
            {.dimension = sweep_parameter::EPSILON_R, .min = 5.0, .max = 5.2, .step = 0.1},
            {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.9, .max = 5.1, .step = 0.1}};

        const auto op_domain = critical_temperature_domain_random_sampling(lyt, std::vector{create_and_tt()}, 10,
                                                                           op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (max 10 steps in each dimension)
        CHECK(op_domain.size() <= 9);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::NON_OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 36);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 9);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations <= 9);
    }
    SECTION("flood_fill")
    {
        op_domain_params.sweep_dimensions = {
            {.dimension = sweep_parameter::EPSILON_R, .min = 5.6, .max = 5.8, .step = 0.1},
            {.dimension = sweep_parameter::LAMBDA_TF, .min = 4.9, .max = 5.1, .step = 0.1}};

        const auto op_domain = critical_temperature_domain_flood_fill(lyt, std::vector{create_and_tt()}, 1,
                                                                      op_domain_params, &op_domain_stats);

        // check if the operational domain has the correct size (10 steps in each dimension)
        CHECK(op_domain.size() == 9);

        // for the selected range, all samples should be within the parameters and operational
        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations <= 36);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations <= 9);
        CHECK(op_domain_stats.num_operational_parameter_combinations <= 9);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 0);
    }
}

TEST_CASE("Two BDL pair wire with degeneracy for input 1", "[operational-domain]")
{
    layout lyt{};

    lyt.assign_cell_type({0, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({2, 0, 0}, sidb_technology::cell_type::INPUT);
    lyt.assign_cell_type({6, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({8, 0, 0}, sidb_technology::cell_type::NORMAL);
    lyt.assign_cell_type({12, 0, 0}, sidb_technology::cell_type::OUTPUT);
    lyt.assign_cell_type({14, 0, 0}, sidb_technology::cell_type::OUTPUT);

    lyt.assign_cell_type({18, 0, 0}, sidb_technology::cell_type::NORMAL);

    simulation_parameters sim_params{};
    sim_params.base     = 2;
    sim_params.mu_minus = -0.32;

    operational_domain_params op_domain_params{};
    op_domain_params.operational_params.sim_params = sim_params;
    op_domain_params.sweep_dimensions = {{.dimension = sweep_parameter::EPSILON_R, .min = 1, .max = 10, .step = 0.1},
                                         {.dimension = sweep_parameter::LAMBDA_TF, .min = 1, .max = 10, .step = 0.1}};

    SECTION("grid search, input is set via the distance of the perturbers")
    {
        operational_domain_stats op_domain_stats{};

        op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config =
            bdl_input_iterator_params::input_bdl_configuration::PERTURBER_DISTANCE_ENCODED;

        const auto op_domain =
            operational_domain_grid_search(lyt, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::NON_OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 10034);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 8281);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 8281);
    }
    SECTION("grid search, input is set via the absence of perturbers")
    {
        operational_domain_stats op_domain_stats{};

        op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config =
            bdl_input_iterator_params::input_bdl_configuration::PERTURBER_ABSENCE_ENCODED;

        const auto op_domain =
            operational_domain_grid_search(lyt, std::vector{create_id_tt()}, op_domain_params, &op_domain_stats);

        check_op_domain_params_and_operational_status(op_domain, op_domain_params, operational_status::NON_OPERATIONAL);

        CHECK(mockturtle::to_seconds(op_domain_stats.time_total) > 0.0);
        CHECK(op_domain_stats.num_simulator_invocations == 9828);
        CHECK(op_domain_stats.num_evaluated_parameter_combinations == 8281);
        CHECK(op_domain_stats.num_operational_parameter_combinations == 0);
        CHECK(op_domain_stats.num_non_operational_parameter_combinations == 8281);
    }
}
