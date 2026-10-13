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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner): converts progress callbacks
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using operational_domain_ratio_options = fiction::sidb::simulation::logic::operational_domain_ratio_params;

/**
 * @brief Binds operational domain ratio helpers.
 * @param m Python module.
 */
void register_operational_domain_ratio(nb::module_& m)
{
    using fiction::sidb::layout;

    using fiction::sidb::simulation::logic::parameter_point;

    m.def(
        "operational_domain_ratio",
        [](const layout& lyt, const std::vector<py_tt>& spec, const parameter_point& pp,
           const decltype(operational_domain_ratio_options{}.op_domain_params.operational_params.timeout) timeout,
           const decltype(operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.base) base,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.sim_params.mu_minus)                      mu_minus,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.sim_params.epsilon_r)                     epsilon_r,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.sim_params.lambda_tf)                     lambda_tf,
           const decltype(operational_domain_ratio_options{}.op_domain_params.operational_params.sim_engine) sim_engine,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)                      threshold_bdl_interdistance,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                              .bdl_pairs_params.minimum_distance)                minimum_distance,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                              .bdl_pairs_params.maximum_distance)                maximum_distance,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.input_bdl_iterator_params
                              .input_bdl_config)                                 input_bdl_config,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params.op_condition) op_condition,
           const decltype(operational_domain_ratio_options{}
                              .op_domain_params.operational_params
                              .strategy_to_analyze_operational_status)           strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_ratio_options{}.op_domain_params.sweep_dimensions)>
               sweep_dimensions,
           const std::optional<decltype(operational_domain_ratio_options{}.op_domain_params.number_of_threads)>
                                                                                                  number_of_threads,
           const decltype(operational_domain_ratio_options{}.op_domain_params.on_progress)        on_progress,
           const decltype(operational_domain_ratio_options{}.op_domain_params.on_worker_progress) on_worker_progress)
        {
            operational_domain_ratio_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.op_domain_params.operational_params.sim_params =
                fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
            params.op_domain_params.operational_params.timeout    = timeout;
            params.op_domain_params.operational_params.sim_engine = sim_engine;
            params.op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                .threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                .minimum_distance = minimum_distance;
            params.op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                .maximum_distance                                                                 = maximum_distance;
            params.op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config = input_bdl_config;
            params.op_domain_params.operational_params.op_condition                               = op_condition;
            params.op_domain_params.operational_params.strategy_to_analyze_operational_status =
                strategy_to_analyze_operational_status;
            if (sweep_dimensions)
            {
                params.op_domain_params.sweep_dimensions = *sweep_dimensions;
            }
            if (number_of_threads)
            {
                params.op_domain_params.number_of_threads = *number_of_threads;
            }
            params.op_domain_params.on_progress        = on_progress;
            params.op_domain_params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::logic::operational_domain_ratio(lyt, spec, pp, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("pp"), nb::kw_only(),
        nb::arg("timeout") = operational_domain_ratio_options{}.op_domain_params.operational_params.timeout,
        nb::arg("base")    = operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.base,
        nb::arg("mu_minus") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf") =
            operational_domain_ratio_options{}.op_domain_params.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = operational_domain_ratio_options{}.op_domain_params.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") = operational_domain_ratio_options{}
                                                     .op_domain_params.operational_params.input_bdl_iterator_params
                                                     .bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") = operational_domain_ratio_options{}
                                          .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                                          .bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") = operational_domain_ratio_options{}
                                          .op_domain_params.operational_params.input_bdl_iterator_params.bdl_wire_params
                                          .bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            operational_domain_ratio_options{}
                .op_domain_params.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = operational_domain_ratio_options{}.op_domain_params.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            operational_domain_ratio_options{}
                .op_domain_params.operational_params.strategy_to_analyze_operational_status,
        nb::arg("sweep_dimensions") = nb::none(), nb::arg("number_of_threads") = nb::none(),
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        R"doc(Calculates the ratio of operational parameter points surrounding a
specified parameter point to the total number of parameter points in
the given parameter space. This function is useful for assessing how
robust a gate design is to variations in its parameters.

A ratio close to 1 indicates that the gate is robust, meaning it
functions correctly across a broad range of parameter values. A ratio
close to 0 indicates that the gate is highly sensitive to parameter
variations and may fail to operate correctly.

Args:
    lyt: The SiDB layout for which to compute the ratio of operational
         parameter points surrounding a specified parameter point to
         the total number of parameter points.
    spec: The expected Boolean function of the layout, provided as a
          multi-output truth table.
    pp: The specific parameter point around which the operational
        ratio is computed.

Returns:
    The ratio of operational parameter points to the total number of
    parameter points in the parameter space.

Raises:
    std::invalid_argument: if the sweep has fewer than two dimensions,
                           a range is invalid, or the seed is non-
                           finite, outside the grid, or has the wrong
                           dimension count.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
