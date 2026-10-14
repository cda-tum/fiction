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
#include "pyfiction/operational_parameters.hpp"
#include "pyfiction/simulation_parameters.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using critical_temperature_options = fiction::sidb::simulation::analysis::critical_temperature_params;

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
        [](const layout& lyt, const std::vector<py_tt>& spec, const decltype(operational_options{}.timeout) timeout,
           const decltype(operational_options{}.sim_params.base)      base,
           const decltype(operational_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(operational_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(operational_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(operational_options{}.sim_engine)           sim_engine,
           const decltype(operational_options{}.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance)
                                                                                            threshold_bdl_interdistance,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                            minimum_distance,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                            maximum_distance,
           const decltype(operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(operational_options{}.op_condition)                               op_condition,
           const decltype(operational_options{}
                              .strategy_to_analyze_operational_status)        strategy_to_analyze_operational_status,
           const decltype(critical_temperature_options{}.confidence_level)    confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)     max_temperature,
           const decltype(critical_temperature_options{}.on_progress)&        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress)& on_worker_progress,
           critical_temperature_stats*                                        stats)
        {
            critical_temperature_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

            params.confidence_level   = confidence_level;
            params.max_temperature    = max_temperature;
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
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
        nb::arg("stats") = nullptr,
        R"doc(Determine the critical temperature of an SiDB gate implementing the truth tables.
The keyword options select physical values, operational analysis, confidence, and temperature bounds.
The overload with input and output wires bypasses automatic wire detection.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    confidence_level: Required probability of a correct gate output, between 0 and 1.
    max_temperature: Maximum temperature examined in K.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_gate_based",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec,
           const std::vector<bdl_pair>& output_bdl_pairs, const std::vector<bdl_wire>& input_bdl_wires,
           const std::vector<bdl_wire>& output_bdl_wires, const decltype(operational_options{}.timeout) timeout,
           const decltype(operational_options{}.sim_params.base)                            base,
           const decltype(operational_options{}.sim_params.mu_minus)                        mu_minus,
           const decltype(operational_options{}.sim_params.epsilon_r)                       epsilon_r,
           const decltype(operational_options{}.sim_params.lambda_tf)                       lambda_tf,
           const decltype(operational_options{}.sim_engine)                                 sim_engine,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .minimum_distance)                                            minimum_distance,
           const decltype(operational_options{}
                              .input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params
                              .maximum_distance)                                            maximum_distance,
           const decltype(operational_options{}.input_bdl_iterator_params.input_bdl_config) input_bdl_config,
           const decltype(operational_options{}.op_condition)                               op_condition,
           const decltype(operational_options{}
                              .strategy_to_analyze_operational_status)        strategy_to_analyze_operational_status,
           const decltype(critical_temperature_options{}.confidence_level)    confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)     max_temperature,
           const decltype(critical_temperature_options{}.on_progress)&        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress)& on_worker_progress,
           critical_temperature_stats*                                        stats)
        {
            critical_temperature_options params{};
            params.operational_params = make_operational_parameters(
                timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                critical_temperature_options{}
                    .operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance,
                minimum_distance, maximum_distance, input_bdl_config, op_condition,
                strategy_to_analyze_operational_status);

            params.confidence_level   = confidence_level;
            params.max_temperature    = max_temperature;
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
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
        nb::arg("stats") = nullptr,
        R"doc(Determine the critical temperature of an SiDB gate implementing the truth tables.
The keyword options select physical values, operational analysis, confidence, and temperature bounds.
The overload with input and output wires bypasses automatic wire detection.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    minimum_distance: Minimum SiDB separation for a BDL pair in nm.
    maximum_distance: Maximum SiDB separation for a BDL pair in nm.
    input_bdl_config: Input encoding: perturber absence or perturber distance.
    confidence_level: Required probability of a correct gate output, between 0 and 1.
    max_temperature: Maximum temperature examined in K.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_non_gate_based",
        [](const layout& lyt, const decltype(operational_options{}.timeout) timeout,
           const decltype(operational_options{}.sim_params.base)              base,
           const decltype(operational_options{}.sim_params.mu_minus)          mu_minus,
           const decltype(operational_options{}.sim_params.epsilon_r)         epsilon_r,
           const decltype(operational_options{}.sim_params.lambda_tf)         lambda_tf,
           const decltype(operational_options{}.sim_engine)                   sim_engine,
           const decltype(critical_temperature_options{}.confidence_level)    confidence_level,
           const decltype(critical_temperature_options{}.max_temperature)     max_temperature,
           const decltype(critical_temperature_options{}.on_progress)&        on_progress,
           const decltype(critical_temperature_options{}.on_worker_progress)& on_worker_progress,
           critical_temperature_stats*                                        stats)
        {
            critical_temperature_options params{};
            params.operational_params.sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.operational_params.timeout    = timeout;
            params.operational_params.sim_engine = sim_engine;

            params.confidence_level   = confidence_level;
            params.max_temperature    = max_temperature;
            params.on_progress        = on_progress;
            params.on_worker_progress = on_worker_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, stats);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("timeout") = critical_temperature_options{}.operational_params.timeout,
        nb::arg("base")               = critical_temperature_options{}.operational_params.sim_params.base,
        nb::arg("mu_minus")           = critical_temperature_options{}.operational_params.sim_params.mu_minus,
        nb::arg("epsilon_r")          = critical_temperature_options{}.operational_params.sim_params.epsilon_r,
        nb::arg("lambda_tf")          = critical_temperature_options{}.operational_params.sim_params.lambda_tf,
        nb::arg("sim_engine")         = critical_temperature_options{}.operational_params.sim_engine,
        nb::arg("confidence_level")   = critical_temperature_options{}.confidence_level,
        nb::arg("max_temperature")    = critical_temperature_options{}.max_temperature,
        nb::arg("on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("stats") = nullptr,
        R"doc(Determine the critical temperature from the population of excited charge distributions.
The keyword options select physical values, confidence, and temperature bounds.

Args:
    timeout: Execution budget in milliseconds.
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    confidence_level: Required ground-state population probability between 0 and 1.
    max_temperature: Maximum temperature examined in K.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
)doc");
}

}  // namespace detail

/**
 * @brief Binds critical temperature helpers.
 * @param m Python module.
 */
void register_critical_temperature(nb::module_& m)
{
    // Keep the GIL for statistics properties; native temperature analyses release it.
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

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_critical_temperature_functions(m);
}

}  // namespace pyfiction
