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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/time_to_solution.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/time_to_solution.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>

#include <optional>
#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
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
using time_to_solution_options = fiction::sidb::simulation::analysis::time_to_solution_params;

/** @brief Native options constructed for each Python call. */
using quicksim_options = fiction::sidb::simulation::engines::quicksim_params;

/**
 * @brief Binds time to solution helpers.
 * @param m Python module.
 */
void register_time_to_solution(nb::module_& m)
{
    /**
     * Statistics.
     */
    nb::class_<fiction::sidb::simulation::analysis::time_to_solution_stats>(
        m, "time_to_solution_stats", DOC(fiction_sidb_simulation_analysis_time_to_solution_stats))
        .def(nb::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::sidb::simulation::analysis::time_to_solution_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_to_solution", &fiction::sidb::simulation::analysis::time_to_solution_stats::time_to_solution,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_time_to_solution))
        .def_ro("acc", &fiction::sidb::simulation::analysis::time_to_solution_stats::acc,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_acc))
        .def_ro("mean_single_runtime",
                &fiction::sidb::simulation::analysis::time_to_solution_stats::mean_single_runtime,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_mean_single_runtime))
        .def_ro("single_runtime_exact",
                &fiction::sidb::simulation::analysis::time_to_solution_stats::single_runtime_exact,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_single_runtime_exact))
        .def_ro("algorithm", &fiction::sidb::simulation::analysis::time_to_solution_stats::algorithm,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_algorithm))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    m.def(
        "time_to_solution",
        [](const fiction::sidb::layout& lyt, const decltype(quicksim_options{}.sim_params.base) base,
           const decltype(quicksim_options{}.sim_params.mu_minus)           mu_minus,
           const decltype(quicksim_options{}.sim_params.epsilon_r)          epsilon_r,
           const decltype(quicksim_options{}.sim_params.lambda_tf)          lambda_tf,
           const decltype(quicksim_options{}.iteration_steps)               iteration_steps,
           const decltype(quicksim_options{}.alpha)                         alpha,
           const std::optional<decltype(quicksim_options{}.number_threads)> number_threads,
           const decltype(quicksim_options{}.timeout)                       quicksim_timeout,
           const decltype(quicksim_options{}.on_progress)                   quicksim_on_progress,
           const decltype(quicksim_options{}.on_worker_progress)            on_worker_progress,
           const decltype(time_to_solution_options{}.timeout)               timeout,
           const decltype(time_to_solution_options{}.engine)                engine,
           const decltype(time_to_solution_options{}.repetitions)           repetitions,
           const decltype(time_to_solution_options{}.confidence_level)      confidence_level,
           const decltype(time_to_solution_options{}.on_progress)           on_progress,
           fiction::sidb::simulation::analysis::time_to_solution_stats*     ps)
        {
            quicksim_options qs_params{};
            qs_params.sim_params      = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            qs_params.iteration_steps = iteration_steps;
            qs_params.alpha           = alpha;
            if (number_threads)
            {
                qs_params.number_threads = *number_threads;
            }
            qs_params.timeout            = quicksim_timeout;
            qs_params.on_progress        = quicksim_on_progress;
            qs_params.on_worker_progress = on_worker_progress;
            time_to_solution_options tts_params{};
            tts_params.timeout          = timeout;
            tts_params.engine           = engine;
            tts_params.repetitions      = repetitions;
            tts_params.confidence_level = confidence_level;
            tts_params.on_progress      = on_progress;
            const nb::gil_scoped_release release{};
            fiction::sidb::simulation::analysis::time_to_solution(lyt, qs_params, tts_params, ps);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = quicksim_options{}.sim_params.base,
        nb::arg("mu_minus")        = quicksim_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")       = quicksim_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")       = quicksim_options{}.sim_params.lambda_tf,
        nb::arg("iteration_steps") = quicksim_options{}.iteration_steps, nb::arg("alpha") = quicksim_options{}.alpha,
        nb::arg("number_threads") = nb::none(), nb::arg("quicksim_timeout") = quicksim_options{}.timeout,
        nb::arg("quicksim_on_progress").none() = nb::none(), nb::arg("on_worker_progress").none() = nb::none(),
        nb::arg("timeout") = time_to_solution_options{}.timeout, nb::arg("engine") = time_to_solution_options{}.engine,
        nb::arg("repetitions")        = time_to_solution_options{}.repetitions,
        nb::arg("confidence_level")   = time_to_solution_options{}.confidence_level,
        nb::arg("on_progress").none() = nb::none(), nb::arg("ps") = nullptr,
        R"doc(Computes the time-to-solution (TTS) of *QuickSim* for a layout: the
layout is simulated once with the exact engine of the parameters and
`tts_params.repetitions` times with *QuickSim*, and the runs are
combined with `time_to_solution_for_given_simulation_results`.

Failed attempts contribute elapsed wall time; successful attempts
retain the engine runtime.

Args:
    lyt: The layout to simulate.
    qs_params: *QuickSim* parameters.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
    m.def("time_to_solution_for_given_simulation_results",
          &fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results, nb::arg("results_exact"),
          nb::arg("results_heuristic"), nb::arg("confidence_level") = 0.997, nb::arg("ps") = nullptr,
          R"doc(Computes the time-to-solution (TTS) of *QuickSim* from an exact result
and a series of heuristic results: the accuracy is the share of
heuristic runs that found the ground state, and TTS is the mean
heuristic runtime scaled to reach the given confidence level, `t ·
log(1 - c) / log(1 - acc)`. An empty sample yields zero accuracy, zero
mean heuristic runtime, and infinite time-to-solution.

Args:
    results_exact: The result of an exact engine, which provides the
                   ground state.
    results_heuristic: The results of the heuristic runs.
    confidence_level: Confidence level for the TTS computation.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
