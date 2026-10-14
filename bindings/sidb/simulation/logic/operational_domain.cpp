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
 * @brief Python bindings for `fiction/technology/sidb/simulation/logic/operational_domain.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/operational_parameters.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/logic/is_operational.hpp>
#include <fiction/technology/sidb/simulation/logic/operational_domain.hpp>

#include <fmt/format.h>

#include <cstddef>
#include <optional>
#include <tuple>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
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

namespace detail
{

/**
 * @brief Registers the operational domain and critical temperature domain algorithms on `sidb_layout`.
 *
 * @param m The module.
 */
inline void register_operational_domain_functions(nb::module_& m)
{
    using fiction::sidb::layout;

    using fiction::sidb::simulation::logic::operational_domain_stats;

    m.def(
        "operational_domain_grid_search",
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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
            return fiction::sidb::simulation::logic::operational_domain_grid_search(lyt, spec, params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(),
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
        nb::arg("stats") = nullptr,
        R"doc(Compute the operational domain by exhaustively sweeping every parameter combination.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "operational_domain_random_sampling",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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

            return fiction::sidb::simulation::logic::operational_domain_random_sampling(lyt, spec, samples, params,
                                                                                        stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr, R"doc(Compute the operational domain by randomly sampling parameter combinations.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "operational_domain_flood_fill",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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
            return fiction::sidb::simulation::logic::operational_domain_flood_fill(lyt, spec, samples, params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr, R"doc(Compute the operational domain by expanding from sampled operational points.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "operational_domain_contour_tracing",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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

            return fiction::sidb::simulation::logic::operational_domain_contour_tracing(lyt, spec, samples, params,
                                                                                        stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr, R"doc(Compute the operational domain by tracing operational boundaries.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_domain_grid_search",
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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
            return fiction::sidb::simulation::logic::critical_temperature_domain_grid_search(lyt, spec, params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::kw_only(),
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
        nb::arg("stats") = nullptr,
        R"doc(Compute the critical-temperature domain by exhaustively sweeping every parameter combination.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Confidence uses the native critical-temperature default of 0.99.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_domain_random_sampling",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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

            return fiction::sidb::simulation::logic::critical_temperature_domain_random_sampling(lyt, spec, samples,
                                                                                                 params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr,
        R"doc(Compute the critical-temperature domain by randomly sampling parameter combinations.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Confidence uses the native critical-temperature default of 0.99.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_domain_flood_fill",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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

            return fiction::sidb::simulation::logic::critical_temperature_domain_flood_fill(lyt, spec, samples, params,
                                                                                            stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr,
        R"doc(Compute the critical-temperature domain by expanding from sampled operational points.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Confidence uses the native critical-temperature default of 0.99.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
    m.def(
        "critical_temperature_domain_contour_tracing",
        [](const layout& lyt, const std::vector<py_tt>& spec, const std::size_t samples,
           const decltype(operational_options{}.timeout)              timeout,
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
                              .strategy_to_analyze_operational_status) strategy_to_analyze_operational_status,
           const std::optional<decltype(operational_domain_options{}.sweep_dimensions)>& sweep_dimensions,
           const std::optional<decltype(operational_domain_options{}.number_of_threads)> number_of_threads,
           const decltype(operational_domain_options{}.on_progress)&                     on_progress,
           const decltype(operational_domain_options{}.on_worker_progress)&              on_worker_progress,
           operational_domain_stats*                                                     stats)
        {
            operational_domain_options params{};
            params.operational_params =
                make_operational_parameters(timeout, base, mu_minus, epsilon_r, lambda_tf, sim_engine,
                                            threshold_bdl_interdistance, minimum_distance, maximum_distance,
                                            input_bdl_config, op_condition, strategy_to_analyze_operational_status);

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

            return fiction::sidb::simulation::logic::critical_temperature_domain_contour_tracing(lyt, spec, samples,
                                                                                                 params, stats);
        },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("samples"), nb::kw_only(),
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
        nb::arg("stats") = nullptr, R"doc(Compute the critical-temperature domain by tracing operational boundaries.
The keyword options define the sweep, physical values, BDL encoding, analysis, and progress callbacks.
Confidence uses the native critical-temperature default of 0.99.
Raises ValueError for invalid sweep dimensions, bounds, or steps.

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
    sweep_dimensions: Ordered parameter ranges; None sweeps relative permittivity and screening distance with the native ranges.
    number_of_threads: Number of workers; None uses the native default.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    stats: Statistics object filled by the algorithm; None discards statistics.
    sim_engine: SiDB simulation engine used for operational analysis.
    op_condition: Require the selected operational condition for every input pattern.
    strategy_to_analyze_operational_status: Operational-status analysis strategy.
)doc");
}

}  // namespace detail

/**
 * @brief Binds operational domain helpers.
 * @param m Python module.
 */
void register_operational_domain(nb::module_& m)
{
    // Keep the GIL for domain lookups and Python conversion; native domain scans release it.
    nb::class_<fiction::sidb::simulation::logic::parameter_point>(m, "parameter_point",
                                                                  DOC(fiction_sidb_simulation_logic_parameter_point))
        .def(nb::init<>(), DOC(fiction_sidb_simulation_logic_parameter_point_parameter_point))
        .def(nb::init<const std::vector<double>>(), nb::arg("values"),
             DOC(fiction_sidb_simulation_logic_parameter_point_parameter_point_2))
        .def("get_parameters", &fiction::sidb::simulation::logic::parameter_point::get_parameters,
             DOC(fiction_sidb_simulation_logic_parameter_point_get_parameters))

        // NOLINTBEGIN(misc-redundant-expression)
        .def(nb::self == nb::self, nb::arg("other"), DOC(fiction_sidb_simulation_logic_parameter_point_operator_eq))
        // `parameter_point` no longer declares an `operator!=`; C++20 rewrites `a != b` as `!(a == b)`, so the
        // binding still works, but there is no generated docstring to reference for it anymore
        .def(nb::self != nb::self, nb::arg("other"),
             "Inequality operator. Checks if this parameter point is not equal to another point within the "
             "tolerance defined by `constants::ERROR_MARGIN`.\n\n"
             ":param other: Other parameter point to compare with.\n"
             ":return: ``True`` iff the parameter points are not equal.")
        // NOLINTEND(misc-redundant-expression)

        .def(
            "__hash__", [](const fiction::sidb::simulation::logic::parameter_point& self)
            { return std::hash<fiction::sidb::simulation::logic::parameter_point>{}(self); },
            "Returns a hash value of the parameter point.")
        .def(
            "__str__", [](const fiction::sidb::simulation::logic::parameter_point& self)
            { return fmt::format("{}", self.get_parameters()); },
            "Returns a string representation of the parameter point.")
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::parameter_point& self, const std::size_t index)
            {
                const auto& parameters = self.get_parameters();
                if (index >= parameters.size())
                {
                    throw nb::index_error("parameter index out of range");
                }
                return parameters[index];
            },
            "Returns the value of the parameter at the given index. Raises `IndexError` if the index is out of range.")

        ;

    nb::enum_<fiction::sidb::simulation::logic::sweep_parameter>(m, "sweep_parameter",
                                                                 DOC(fiction_sidb_simulation_logic_sweep_parameter))
        .value("EPSILON_R", fiction::sidb::simulation::logic::sweep_parameter::EPSILON_R,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_EPSILON_R))
        .value("LAMBDA_TF", fiction::sidb::simulation::logic::sweep_parameter::LAMBDA_TF,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_LAMBDA_TF))
        .value("MU_MINUS", fiction::sidb::simulation::logic::sweep_parameter::MU_MINUS,
               DOC(fiction_sidb_simulation_logic_sweep_parameter_MU_MINUS))

        ;

    nb::class_<fiction::sidb::simulation::logic::critical_temperature_domain>(
        m, "critical_temperature_domain", DOC(fiction_sidb_simulation_logic_critical_temperature_domain))
        .def(nb::init<>(), "Default constructor.")
        .def(nb::init<const std::vector<fiction::sidb::simulation::logic::sweep_parameter>>(), nb::arg("dims"),
             "Constructs a critical temperature domain with the given sweep dimensions.")
        .def("get_dimension", &fiction::sidb::simulation::logic::critical_temperature_domain::get_dimension,
             nb::arg("index"), DOC(fiction_sidb_simulation_logic_critical_temperature_domain_get_dimension))
        .def("get_number_of_dimensions",
             &fiction::sidb::simulation::logic::critical_temperature_domain::get_number_of_dimensions,
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_get_number_of_dimensions))
        .def("minimum_ct", &fiction::sidb::simulation::logic::critical_temperature_domain::minimum_ct,
             nb::call_guard<nb::gil_scoped_release>(),
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_minimum_ct))
        .def("maximum_ct", &fiction::sidb::simulation::logic::critical_temperature_domain::maximum_ct,
             nb::call_guard<nb::gil_scoped_release>(),
             DOC(fiction_sidb_simulation_logic_critical_temperature_domain_maximum_ct))

        // Pythonic interface functions
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self,
               const fiction::sidb::simulation::logic::parameter_point&             key)
            {
                const auto val = self.contains(key);
                if (!val.has_value())
                {
                    throw nb::key_error("Key not found");
                }
                return val.value();
            },
            "Returns the value stored for the given parameter point, raising a KeyError if it does not exist.")
        .def(
            "__setitem__",
            [](fiction::sidb::simulation::logic::critical_temperature_domain&                  self,
               const fiction::sidb::simulation::logic::parameter_point&                        key,
               const std::tuple<fiction::sidb::simulation::logic::operational_status, double>& value)
            { self.add_value(key, value); },
            "Sets the value stored for the given parameter point.")
        .def(
            "__contains__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self,
               const fiction::sidb::simulation::logic::parameter_point& key) { return self.contains(key).has_value(); },
            "Checks whether the given parameter point is contained in the domain.")
        .def(
            "__len__", [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            { return self.size(); }, "Returns the number of parameter points stored in the domain.")
        .def(
            "__iter__",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                {
                    const nb::gil_scoped_release release{};
                    keys.reserve(self.size());
                    self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                }

                // Keep the GIL: casting keys and creating the iterator call the Python API.
                const nb::object py_keys = nb::cast(keys);
                return nb::iter(py_keys);
            },
            nb::sig(
                "def __iter__(self) -> collections.abc.Iterator[mnt.pyfiction.sidb.simulation.logic.parameter_point]"),
            "Returns an iterator over the parameter points stored in the domain.")
        .def(
            "keys",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                keys.reserve(self.size());
                self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                return keys;
            },
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "values",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            {
                std::vector<std::tuple<fiction::sidb::simulation::logic::operational_status, double>> values{};
                values.reserve(self.size());
                self.for_each([&values](const auto&, const auto& value) { values.push_back(value); });
                return values;
            },
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "items",
            [](const fiction::sidb::simulation::logic::critical_temperature_domain& self)
            {
                std::vector<std::pair<fiction::sidb::simulation::logic::parameter_point,
                                      std::tuple<fiction::sidb::simulation::logic::operational_status, double>>>
                    items{};
                items.reserve(self.size());
                self.for_each([&items](const auto& key, const auto& value) { items.emplace_back(key, value); });
                return items;
            },
            nb::call_guard<nb::gil_scoped_release>())

        ;

    nb::class_<fiction::sidb::simulation::logic::operational_domain>(
        m, "operational_domain", DOC(fiction_sidb_simulation_logic_operational_domain))
        .def(nb::init<>(), DOC(fiction_sidb_simulation_logic_operational_domain_operational_domain))
        .def(nb::init<const std::vector<fiction::sidb::simulation::logic::sweep_parameter>>(), nb::arg("dims"),
             DOC(fiction_sidb_simulation_logic_operational_domain_operational_domain_2))
        .def("add_dimension", &fiction::sidb::simulation::logic::operational_domain::add_dimension, nb::arg("dim"),
             DOC(fiction_sidb_simulation_logic_operational_domain_add_dimension))
        .def("get_dimension", &fiction::sidb::simulation::logic::operational_domain::get_dimension, nb::arg("index"),
             DOC(fiction_sidb_simulation_logic_operational_domain_get_dimension))
        .def("get_number_of_dimensions",
             &fiction::sidb::simulation::logic::operational_domain::get_number_of_dimensions,
             DOC(fiction_sidb_simulation_logic_operational_domain_get_number_of_dimensions))

        // Pythonic interface functions
        .def(
            "__getitem__",
            [](const fiction::sidb::simulation::logic::operational_domain& self,
               const fiction::sidb::simulation::logic::parameter_point&    key)
            {
                const auto val = self.contains(key);
                if (!val.has_value())
                {
                    throw nb::key_error("Key not found");
                }

                return std::get<0>(val.value());
            },
            "Returns the operational status stored for the given parameter point, raising a KeyError if it does "
            "not exist.")
        .def(
            "__setitem__",
            [](fiction::sidb::simulation::logic::operational_domain&       self,
               const fiction::sidb::simulation::logic::parameter_point&    key,
               const fiction::sidb::simulation::logic::operational_status& value) { self.add_value(key, {value}); },
            "Sets the operational status stored for the given parameter point.")
        .def(
            "__contains__",
            [](const fiction::sidb::simulation::logic::operational_domain& self,
               const fiction::sidb::simulation::logic::parameter_point& key) { return self.contains(key).has_value(); },
            "Checks whether the given parameter point is contained in the domain.")
        .def(
            "__len__", [](const fiction::sidb::simulation::logic::operational_domain& self) { return self.size(); },
            "Returns the number of parameter points stored in the domain.")
        .def(
            "__iter__",
            [](const fiction::sidb::simulation::logic::operational_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                {
                    const nb::gil_scoped_release release{};
                    keys.reserve(self.size());
                    self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                }

                // Keep the GIL: casting keys and creating the iterator call the Python API.
                const nb::object py_keys = nb::cast(keys);
                return nb::iter(py_keys);
            },
            nb::sig(
                "def __iter__(self) -> collections.abc.Iterator[mnt.pyfiction.sidb.simulation.logic.parameter_point]"),
            "Returns an iterator over the parameter points stored in the domain.")
        .def(
            "keys",
            [](const fiction::sidb::simulation::logic::operational_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::parameter_point> keys{};
                keys.reserve(self.size());
                self.for_each([&keys](const auto& key, const auto&) { keys.push_back(key); });
                return keys;
            },
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "values",
            [](const fiction::sidb::simulation::logic::operational_domain& self)
            {
                std::vector<fiction::sidb::simulation::logic::operational_status> values{};
                values.reserve(self.size());
                self.for_each([&values](const auto&, const auto& value) { values.push_back(std::get<0>(value)); });
                return values;
            },
            nb::call_guard<nb::gil_scoped_release>())
        .def(
            "items",
            [](const fiction::sidb::simulation::logic::operational_domain& self)
            {
                std::vector<std::pair<fiction::sidb::simulation::logic::parameter_point,
                                      fiction::sidb::simulation::logic::operational_status>>
                    items{};
                items.reserve(self.size());
                self.for_each([&items](const auto& key, const auto& value)
                              { items.emplace_back(key, std::get<0>(value)); });
                return items;
            },
            nb::call_guard<nb::gil_scoped_release>())

        ;

    nb::class_<fiction::sidb::simulation::logic::operational_domain_value_range>(
        m, "operational_domain_value_range", DOC(fiction_sidb_simulation_logic_operational_domain_value_range))
        .def(nb::init<fiction::sidb::simulation::logic::sweep_parameter>(), nb::arg("dimension"))
        .def(nb::init<fiction::sidb::simulation::logic::sweep_parameter, double, double, double>(),
             nb::arg("dimension"), nb::arg("min"), nb::arg("max"), nb::arg("step"))
        .def_rw("dimension", &fiction::sidb::simulation::logic::operational_domain_value_range::dimension,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_dimension))
        .def_rw("min", &fiction::sidb::simulation::logic::operational_domain_value_range::min,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_min))
        .def_rw("max", &fiction::sidb::simulation::logic::operational_domain_value_range::max,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_max))
        .def_rw("step", &fiction::sidb::simulation::logic::operational_domain_value_range::step,
                DOC(fiction_sidb_simulation_logic_operational_domain_value_range_step))

        ;

    nb::class_<fiction::sidb::simulation::logic::operational_domain_stats>(
        m, "operational_domain_stats", DOC(fiction_sidb_simulation_logic_operational_domain_stats))
        .def(nb::init<>(), "Default constructor.")
        .def_ro("time_total", &fiction::sidb::simulation::logic::operational_domain_stats::time_total,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_time_total))
        .def_ro("num_simulator_invocations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_simulator_invocations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_simulator_invocations))
        .def_ro("num_evaluated_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_evaluated_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_evaluated_parameter_combinations))
        .def_ro("num_operational_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_operational_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_operational_parameter_combinations))
        .def_ro("num_non_operational_parameter_combinations",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_non_operational_parameter_combinations,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_non_operational_parameter_combinations))
        .def_ro("num_total_parameter_points",
                &fiction::sidb::simulation::logic::operational_domain_stats::num_total_parameter_points,
                DOC(fiction_sidb_simulation_logic_operational_domain_stats_num_total_parameter_points))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_operational_domain_functions(m);
}

}  // namespace pyfiction
