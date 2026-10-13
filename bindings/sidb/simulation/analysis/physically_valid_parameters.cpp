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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/simulation/analysis/physically_valid_parameters.hpp>
#include <fiction/technology/sidb/simulation/domain.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <cstdint>
#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner): converts progress callbacks
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
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
using operational_domain_options = fiction::sidb::simulation::logic::operational_domain_params;

/**
 * @brief Binds physically valid parameters helpers.
 * @param m Python module.
 */
void register_physically_valid_parameters(nb::module_& m)
{
    nb::class_<fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>>(
        m, "physically_valid_parameters_domain")
        .def(nb::init<>(), "Default constructor.")
        .def(
            "get_excited_state_number_for_parameter",
            [](const fiction::sidb::simulation::domain<fiction::sidb::simulation::logic::parameter_point, uint64_t>&
                                                                        domain,
               const fiction::sidb::simulation::logic::parameter_point& pp)
            {
                if (const auto result = domain.contains(pp); result.has_value())
                {
                    return std::get<0>(result.value());
                }
                // If no value is present, raise a Python ValueError
                throw nb::value_error(
                    "Invalid parameter point: no excited state number available for the provided parameter.");
            },
            nb::arg("pp"));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    m.def(
        "physically_valid_parameters",
        [](const fiction::sidb::layout& lyt, const fiction::sidb::charge_distribution& cd,
           const decltype(operational_domain_options{}.operational_params.timeout)              timeout,
           const decltype(operational_domain_options{}.operational_params.sim_params.base)      base,
           const decltype(operational_domain_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(operational_domain_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(operational_domain_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(operational_domain_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(operational_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance) threshold_bdl_interdistance,
           const decltype(operational_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)            minimum_distance,
           const decltype(operational_domain_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)            maximum_distance,
           const decltype(operational_domain_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(operational_domain_options{}.operational_params.op_condition)       op_condition,
           const decltype(operational_domain_options{}.operational_params.strategy_to_analyze_operational_status)
               strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>  sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)                      on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)               on_worker_progress)
        {
            operational_domain_options params{};
            params.operational_params.sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.operational_params.timeout    = timeout;
            params.operational_params.sim_engine = sim_engine;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance =
                threshold_bdl_interdistance;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance =
                minimum_distance;
            params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance =
                maximum_distance;
            params.operational_params.input_bdl_iterator_params.input_bdl_config = input_bdl_config;
            params.operational_params.op_condition                               = op_condition;
            params.operational_params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            if (sweep_dimensions)
            {
                params.sweep_dimensions = *sweep_dimensions;
            }
            if (number_of_threads)
            {
                params.number_of_threads = *number_of_threads;
            }
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::physically_valid_parameters(lyt, cd, params);
        },
        nb::arg("lyt"), nb::arg("cd"), nb::kw_only(),
        nb::arg("timeout")    = operational_domain_options{}.operational_params.timeout,
        nb::arg("base")       = operational_domain_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = operational_domain_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = operational_domain_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = operational_domain_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = operational_domain_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            operational_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            operational_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            operational_domain_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            operational_domain_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = operational_domain_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            operational_domain_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("sweep_dimensions") = nb::none(), nb::arg("number_of_threads") = nb::none(),
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        R"doc(Determines the physical parameters under which a given charge
distribution of a layout is physically valid: every parameter point of
the sweep dimensions in the keyword options is checked, and for each one where
the charge distribution is valid, its excited-state number (the number
of valid configurations with lower energy under those parameters) is
recorded.

Args:
    lyt: The layout.
    cd: The charge distribution whose validity is checked.

Returns:
    The parameter points where `cd` is physically valid, each with its
    excited-state number; empty if `cd` does not cover the layout's
    SiDBs in raster order.

Raises:
    std::invalid_argument: if a sweep range has non-finite or reversed
                           bounds, a non-positive or non-finite step,
                           or more points than the storage range
                           permits.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
