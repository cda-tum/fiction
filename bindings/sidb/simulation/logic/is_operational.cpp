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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/is_operational.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>

#include <optional>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
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
using is_operational_options = fiction::sidb::simulation::logic::is_operational_params;

/** @brief Native options constructed for each Python call. */
using simulation_model = fiction::sidb::model::simulation_parameters;

namespace detail
{

/**
 * @brief Registers the operational checks on `sidb_layout`.
 *
 * @param m The module.
 */
inline void register_is_operational_functions(nb::module_& m)
{
    using fiction::sidb::layout;
    using fiction::sidb::simulation::logic::bdl_wire;

    using wires = std::vector<bdl_wire>;

    m.def(
        "is_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            return fiction::sidb::simulation::logic::is_operational(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Determines whether an SiDB layout implements the given Boolean
function(s). Every input pattern is applied to the input BDL pairs,
the ground states are simulated with the configured engine, and the
charge states of the output BDL pairs are compared with the expected
truth-table entries. If the layout carries logic dots, they form the
canvas of the pruning filters that run before any simulation whenever
the parameters ask for filtering and reject kinks.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.

Returns:
    The operational status and the number of simulator invocations.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "is_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const wires& in, const wires& out,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            return fiction::sidb::simulation::logic::is_operational(lyt, spec, params, in, out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt, R"doc(Like the overload above, with the BDL wires and, optionally, the
canvas given by the caller.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.
    input_bdl_wire: The input BDL wires of `lyt`.
    output_bdl_wire: The output BDL wires of `lyt`.
    canvas_lyt: The canvas; defaults to the logic dots of `lyt`.

Returns:
    The operational status and the number of simulator invocations.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "is_operational",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec, const wires& in,
           const wires& out, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;

            return fiction::sidb::simulation::logic::is_operational(input_pattern_layouts, spec, params, in, out,
                                                                    canvas_lyt);
        },
        nb::arg("input_pattern_layouts"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"),
        nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Like the overloads above, but with one layout per input pattern given
by the caller instead of applying the patterns to the input BDL pairs.

Args:
    input_pattern_layouts: One layout per input pattern, pattern `0`
                           first.
    spec: The Boolean function(s) to implement.
    input_bdl_wire: The input BDL wires.
    output_bdl_wire: The output BDL wires.
    canvas_lyt: The canvas; defaults to the logic dots of the first
                layout.

Returns:
    The operational status and the number of simulator invocations.

Raises:
    std::invalid_argument: if `spec` is empty or the number of layouts
                           does not match the number of patterns.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Determines the input patterns for which the layout is operational.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.

Returns:
    The operational input patterns.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const wires& in, const wires& out,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;

            return fiction::sidb::simulation::logic::operational_input_patterns(lyt, spec, params, in, out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt, R"doc(Like the overload above, with the BDL wires and, optionally, the
canvas given by the caller.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.
    input_bdl_wire: The input BDL wires of `lyt`.
    output_bdl_wire: The output BDL wires of `lyt`.
    canvas_lyt: The canvas; defaults to none.

Returns:
    The operational input patterns.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Determines the input patterns for which kinks render the layout non-
operational.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.

Returns:
    The kink-induced non-operational input patterns.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "kink_induced_non_operational_input_patterns",
        [](const layout& lyt, const std::vector<py_tt>& spec, const wires& in, const wires& out,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;

            return fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(lyt, spec, params, in,
                                                                                                 out, canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt,
        R"doc(Like the overload above, with the BDL wires and, optionally, the
canvas given by the caller.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.
    input_bdl_wire: The input BDL wires of `lyt`.
    output_bdl_wire: The output BDL wires of `lyt`.
    canvas_lyt: The canvas; defaults to none.

Returns:
    The kink-induced non-operational input patterns.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const decltype(is_operational_options{}.timeout) timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;
            return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(), nb::arg("timeout") = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        R"doc(Determines whether kinks are the reason the layout is non-operational.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.

Returns:
    `true` if the layout is non-operational because of kinks.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "is_kink_induced_non_operational",
        [](const layout& lyt, const std::vector<py_tt>& spec, const wires& in, const wires& out,
           const decltype(is_operational_options{}.timeout)              timeout,
           const decltype(is_operational_options{}.sim_params.base)      base,
           const decltype(is_operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(is_operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(is_operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(is_operational_options{}.sim_engine)           sim_engine,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance)              threshold_bdl_interdistance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                         minimum_distance,
           const decltype(is_operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                         maximum_distance,
           const decltype(is_operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(is_operational_options{}.op_condition)                               op_condition,
           const decltype(is_operational_options{}
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<layout>&                                canvas_lyt)
        {
            is_operational_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
            params.timeout    = timeout;
            params.sim_engine = sim_engine;
            params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = threshold_bdl_interdistance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = minimum_distance;
            params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = maximum_distance;
            params.input_bdl_iterator_params.input_bdl_config                                  = input_bdl_config;
            params.op_condition                                                                = op_condition;
            params.strategy_to_analyze_operational_status = strategy_to_analyze_operational_status;

            return fiction::sidb::simulation::logic::is_kink_induced_non_operational(lyt, spec, params, in, out,
                                                                                     canvas_lyt);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("input_bdl_wire"), nb::arg("output_bdl_wire"), nb::kw_only(),
        nb::arg("timeout")    = is_operational_options{}.timeout,
        nb::arg("base")       = is_operational_options{}.sim_params.base,
        nb::arg("mu_minus")   = is_operational_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")  = is_operational_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = is_operational_options{}.sim_params.lambda_tf,
        nb::arg("sim_engine") = is_operational_options{}.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            is_operational_options{}.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") = is_operational_options{}.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition")     = is_operational_options{}.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            is_operational_options{}.strategy_to_analyze_operational_status,
        nb::arg("canvas_lyt") = std::nullopt, R"doc(Like the overload above, with the BDL wires and, optionally, the
canvas given by the caller.

Args:
    lyt: The layout to check.
    spec: The Boolean function(s) it has to implement.
    input_bdl_wire: The input BDL wires of `lyt`.
    output_bdl_wire: The output BDL wires of `lyt`.
    canvas_lyt: The canvas; defaults to none.

Returns:
    `true` if the layout is non-operational because of kinks.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds is operational helpers.
 * @param m Python module.
 */
void register_is_operational(nb::module_& m)
{
    nb::enum_<fiction::sidb::simulation::logic::operational_status>(
        m, "operational_status", DOC(fiction_sidb_simulation_logic_operational_status))
        .value("OPERATIONAL", fiction::sidb::simulation::logic::operational_status::OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_OPERATIONAL))
        .value("NON_OPERATIONAL", fiction::sidb::simulation::logic::operational_status::NON_OPERATIONAL,
               DOC(fiction_sidb_simulation_logic_operational_status_NON_OPERATIONAL));

    nb::enum_<is_operational_options::operational_condition>(
        m, "operational_condition", DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition))
        .value("TOLERATE_KINKS", is_operational_options::operational_condition::TOLERATE_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_TOLERATE_KINKS))
        .value("REJECT_KINKS", is_operational_options::operational_condition::REJECT_KINKS,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_condition_REJECT_KINKS));

    nb::enum_<is_operational_options::operational_analysis_strategy>(
        m, "operational_analysis_strategy",
        DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy))
        .value("SIMULATION_ONLY", is_operational_options::operational_analysis_strategy::SIMULATION_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_SIMULATION_ONLY))
        .value("FILTER_ONLY", is_operational_options::operational_analysis_strategy::FILTER_ONLY,
               DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_ONLY))
        .value(
            "FILTER_THEN_SIMULATION", is_operational_options::operational_analysis_strategy::FILTER_THEN_SIMULATION,
            DOC(fiction_sidb_simulation_logic_is_operational_params_operational_analysis_strategy_FILTER_THEN_SIMULATION));

    detail::register_is_operational_functions(m);
}

}  // namespace pyfiction
