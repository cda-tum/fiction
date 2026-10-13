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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/critical_temperature.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <optional>
#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using critical_temperature_options = fiction::sidb::simulation::analysis::critical_temperature_params;

/** @brief Native options constructed for each Python call. */
using simulation_model = fiction::sidb::model::simulation_parameters;

namespace detail
{

/**
 * @brief Registers the critical temperature algorithms on `sidb_layout`.
 *
 * @param m The module.
 */
inline void register_critical_temperature_functions(nb::module_& m)
{
    using fiction::sidb::layout;

    using fiction::sidb::simulation::analysis::critical_temperature_stats;
    using fiction::sidb::simulation::logic::bdl_pair;
    using fiction::sidb::simulation::logic::bdl_wire;

    m.def(
        "critical_temperature_gate_based",
        [](const layout& lyt, const std::vector<py_tt>& spec,
           const decltype(critical_temperature_options{}.operational_params.timeout)              timeout,
           const decltype(critical_temperature_options{}.operational_params.sim_params.base)      base,
           const decltype(critical_temperature_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(critical_temperature_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(critical_temperature_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(critical_temperature_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance) threshold_bdl_interdistance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)            minimum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)            maximum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(critical_temperature_options{}.operational_params.op_condition)     op_condition,
           const decltype(critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status)
                                                                             strategy_to_analyze_operational_status,
           const decltype(critical_temperature_options{}.confidence_level)   confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)    max_temperature,
           const decltype(critical_temperature_options{}.on_progress)        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress) on_worker_progress,
           critical_temperature_stats*                                       stats)
        {
            critical_temperature_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.operational_params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
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
            params.confidence_level                                          = confidence_level;
            params.max_temperature                                           = max_temperature;
            params.on_progress                                               = on_progress;
            params.on_worker_progress                                        = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::critical_temperature_gate_based(lyt, spec, params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(),
        nb::arg("timeout")    = critical_temperature_options{}.operational_params.timeout,
        nb::arg("base")       = critical_temperature_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = critical_temperature_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = critical_temperature_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = critical_temperature_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = critical_temperature_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            critical_temperature_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = critical_temperature_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("confidence_level")   = critical_temperature_options{}.confidence_level,
        nb::arg("max_temperature")    = critical_temperature_options{}.max_temperature,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("stats") = nullptr, R"doc(This algorithm performs temperature-aware SiDB simulation as proposed
in \"Temperature Behavior of Silicon Dangling Bond Logic\" by J.
Drewniok, M. Walter, and R. Wille in IEEE NANO 2023
(https://ieeexplore.ieee.org/document/10231259). It comes in two
flavors: gate-based and non-gate based.

For *Gate-based Critical Temperature* Simulation, the Critical
Temperature is defined as follows: The temperature at which the
erroneous charge distributions are populated by more than :math:`1 -
\eta`, where :math:`\eta \in [0,1]`.

Args:
    lyt: The layout to simulate.
    spec: Expected Boolean function of the layout given as a multi-
          output truth table.
    pst: Statistics.

Returns:
    The critical temperature (unit: K).

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "critical_temperature_gate_based",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec,
           const std::vector<bdl_pair>& output_bdl_pairs, const std::vector<bdl_wire>& input_bdl_wires,
           const std::vector<bdl_wire>&                                                           output_bdl_wires,
           const decltype(critical_temperature_options{}.operational_params.timeout)              timeout,
           const decltype(critical_temperature_options{}.operational_params.sim_params.base)      base,
           const decltype(critical_temperature_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(critical_temperature_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(critical_temperature_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(critical_temperature_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance) threshold_bdl_interdistance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)            minimum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)            maximum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(critical_temperature_options{}.operational_params.op_condition)     op_condition,
           const decltype(critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status)
                                                                             strategy_to_analyze_operational_status,
           const decltype(critical_temperature_options{}.confidence_level)   confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)    max_temperature,
           const decltype(critical_temperature_options{}.on_progress)        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress) on_worker_progress,
           critical_temperature_stats*                                       stats)
        {
            critical_temperature_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.operational_params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
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
            params.confidence_level                                          = confidence_level;
            params.max_temperature                                           = max_temperature;
            params.on_progress                                               = on_progress;
            params.on_worker_progress                                        = on_worker_progress;
            const nb::gil_scoped_release release{};

            return fiction::sidb::simulation::analysis::critical_temperature_gate_based(
                input_pattern_layouts, spec, params, output_bdl_pairs, input_bdl_wires, output_bdl_wires, stats);
        },
        nb::arg("input_pattern_layouts"), nb::arg("spec"), nb::arg("output_bdl_pairs"), nb::arg("input_bdl_wires"),
        nb::arg("output_bdl_wires"), nb::kw_only(),
        nb::arg("timeout")    = critical_temperature_options{}.operational_params.timeout,
        nb::arg("base")       = critical_temperature_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = critical_temperature_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = critical_temperature_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = critical_temperature_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = critical_temperature_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            critical_temperature_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = critical_temperature_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("confidence_level")   = critical_temperature_options{}.confidence_level,
        nb::arg("max_temperature")    = critical_temperature_options{}.max_temperature,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("stats") = nullptr, R"doc(*Gate-based Critical Temperature* simulation of an SiDB layout from
its pre-generated input pattern layouts.

This overload takes one layout per input pattern together with the BDL
detection results, instead of deriving all of them from the layout.
Since neither the input configuration nor the BDL pairs and wires
depend on the simulation parameters, a caller that simulates the same
layout under many parameter settings can determine them once with
`generate_bdl_input_pattern_layouts`, `detect_bdl_pairs` and
`detect_bdl_wires` and pass them to every call, instead of re-deriving
them each time.

The layouts and the detection results are only read and may be shared
by concurrent calls.

Args:
    input_pattern_layouts: One layout per input pattern, indexed by
                           input pattern, as generated by
                           `generate_bdl_input_pattern_layouts`.
    spec: Expected Boolean function of the layout given as a multi-
          output truth table.
    output_bdl_pairs: Output BDL pairs of the layout.
    input_bdl_wires: BDL input wires of the layout.
    output_bdl_wires: BDL output wires of the layout.
    pst: Statistics.

Returns:
    The critical temperature (unit: K).

Raises:
    std::invalid_argument: if `spec` is empty, if the number of input
                           pattern layouts does not match the number
                           of input combinations of `spec`, or if the
                           number of output BDL pairs does not match
                           the number of truth tables.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def(
        "critical_temperature_non_gate_based",
        [](const layout& lyt, const decltype(critical_temperature_options{}.operational_params.timeout) timeout,
           const decltype(critical_temperature_options{}.operational_params.sim_params.base)      base,
           const decltype(critical_temperature_options{}.operational_params.sim_params.mu_minus)  mu_minus,
           const decltype(critical_temperature_options{}.operational_params.sim_params.epsilon_r) epsilon_r,
           const decltype(critical_temperature_options{}.operational_params.sim_params.lambda_tf) lambda_tf,
           const decltype(critical_temperature_options{}.operational_params.sim_engine)           sim_engine,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params
                              .threshold_bdl_interdistance) threshold_bdl_interdistance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)            minimum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)            maximum_distance,
           const decltype(critical_temperature_options{}
                              .operational_params.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(critical_temperature_options{}.operational_params.op_condition)     op_condition,
           const decltype(critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status)
                                                                             strategy_to_analyze_operational_status,
           const decltype(critical_temperature_options{}.confidence_level)   confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)    max_temperature,
           const decltype(critical_temperature_options{}.on_progress)        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress) on_worker_progress,
           critical_temperature_stats*                                       stats)
        {
            critical_temperature_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.operational_params.sim_params = simulation_model{base, mu_minus, epsilon_r, lambda_tf};
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
            params.confidence_level                                          = confidence_level;
            params.max_temperature                                           = max_temperature;
            params.on_progress                                               = on_progress;
            params.on_worker_progress                                        = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, stats);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("timeout") = critical_temperature_options{}.operational_params.timeout,
        nb::arg("base")       = critical_temperature_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")   = critical_temperature_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")  = critical_temperature_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")  = critical_temperature_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine") = critical_temperature_options{}.operational_params.sim_engine,
        nb::arg("threshold_bdl_interdistance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
        nb::arg("minimum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance,
        nb::arg("maximum_distance") =
            critical_temperature_options{}
                .operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance,
        nb::arg("input_bdl_config") =
            critical_temperature_options{}.operational_params.input_bdl_iterator_params.input_bdl_config,
        nb::arg("op_condition") = critical_temperature_options{}.operational_params.op_condition,
        nb::arg("strategy_to_analyze_operational_status") =
            critical_temperature_options{}.operational_params.strategy_to_analyze_operational_status,
        nb::arg("confidence_level")   = critical_temperature_options{}.confidence_level,
        nb::arg("max_temperature")    = critical_temperature_options{}.max_temperature,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("stats") = nullptr, R"doc(For *Non-gate-based Critical Temperature* simulation, the Critical
Temperature is defined as follows: The temperature at which the
excited charge distributions are populated by more than :math:`1 -
\eta`, where :math:`\eta \in [0,1]` is the confidence level for the
presence of a working gate.

Args:
    lyt: The layout to simulate.
    pst: Statistics.

Returns:
    The critical temperature (unit: K)

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds critical temperature helpers.
 * @param m Python module.
 */
void register_critical_temperature(nb::module_& m)
{
    /**
     * Critical temperature statistics.
     */
    nb::class_<fiction::sidb::simulation::analysis::critical_temperature_stats>(
        m, "critical_temperature_stats", DOC(fiction_sidb_simulation_analysis_critical_temperature_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::sidb::simulation::analysis::critical_temperature_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("algorithm_name", &fiction::sidb::simulation::analysis::critical_temperature_stats::algorithm_name,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_stats_algorithm_name))
        .def_ro("num_valid_lyt", &fiction::sidb::simulation::analysis::critical_temperature_stats::num_valid_lyt,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_stats_num_valid_lyt))
        .def_ro(
            "energy_between_ground_state_and_first_erroneous",
            &fiction::sidb::simulation::analysis::critical_temperature_stats::
                energy_between_ground_state_and_first_erroneous,
            DOC(fiction_sidb_simulation_analysis_critical_temperature_stats_energy_between_ground_state_and_first_erroneous))

        ;

    /**
     * Critical temperature parameters.
     */

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_critical_temperature_functions(m);
}

}  // namespace pyfiction
