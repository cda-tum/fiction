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
#include <fiction/technology/sidb/simulation/result.hpp>

#include <optional>
#include <sstream>
#include <vector>

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
           const decltype(quicksim_options{}.on_progress)&                  quicksim_on_progress,
           const decltype(quicksim_options{}.on_worker_progress)&           on_worker_progress,
           const decltype(time_to_solution_options{}.timeout)               timeout,
           const decltype(time_to_solution_options{}.engine)                engine,
           const decltype(time_to_solution_options{}.repetitions)           repetitions,
           const decltype(time_to_solution_options{}.confidence_level)      confidence_level,
           const decltype(time_to_solution_options{}.on_progress)&          on_progress,
           fiction::sidb::simulation::analysis::time_to_solution_stats*     ps)
        {
            validate_confidence_level(confidence_level);
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
        R"doc(Compute QuickSim time-to-solution from one exact run and repetitions heuristic runs.
quicksim_timeout bounds each heuristic run; timeout bounds the complete evaluation.
Failed attempts contribute elapsed wall time, while successful attempts retain the engine runtime.
ps receives completed statistics; expiration of the complete evaluation raises TimeoutError.
Raises ValueError for confidence outside (0, 1) or ClusterComplete with a finite timeout.

Args:
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    number_threads: Number of QuickSim workers; None uses the available hardware threads.
    quicksim_timeout: Execution budget for each heuristic run in milliseconds.
    quicksim_on_progress: Progress callback for heuristic iterations; None disables these reports.
    on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    timeout: Execution budget in milliseconds.
    engine: Exhaustive simulation algorithm used to simulate the ground state as reference.
    repetitions: Number of iterations of the heuristic algorithm used to determine the simulation accuracy (`repetitions = 100` means that accuracy is precise to 1 %).
    confidence_level: Probability of finding the ground state used to compute time-to-solution, between 0 and 1.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    ps: Statistics object filled by the algorithm; None discards statistics.
    iteration_steps: Number of QuickSim iterations.
    alpha: QuickSim heuristic alpha parameter.
)doc");
    m.def(
        "time_to_solution_for_given_simulation_results",
        [](const fiction::sidb::simulation::result&              results_exact,
           const std::vector<fiction::sidb::simulation::result>& results_heuristic, const double confidence_level,
           fiction::sidb::simulation::analysis::time_to_solution_stats* ps)
        {
            validate_confidence_level(confidence_level);
            fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results(
                results_exact, results_heuristic, confidence_level, ps);
        },
        nb::arg("results_exact"), nb::arg("results_heuristic"), nb::kw_only(), nb::arg("confidence_level") = 0.997,
        nb::arg("ps") = nullptr,
        R"doc(Evaluate heuristic time-to-solution against an exact simulation result.
confidence_level must be strictly between 0 and 1; invalid values raise ValueError.
ps receives accuracy, runtimes, and time-to-solution statistics.
Empty heuristic results yield zero accuracy and infinite time-to-solution.
)doc");
}

}  // namespace pyfiction
