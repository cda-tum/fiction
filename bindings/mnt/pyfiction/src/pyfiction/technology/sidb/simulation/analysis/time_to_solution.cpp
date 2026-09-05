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

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/time_to_solution.hpp>
#include <fiction/technology/sidb/simulation/engines/quicksim.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void time_to_solution(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Parameters.
     */
    py::class_<fiction::sidb::simulation::analysis::time_to_solution_params>(
        m, "time_to_solution_params", DOC(fiction_sidb_simulation_analysis_time_to_solution_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("engine", &fiction::sidb::simulation::analysis::time_to_solution_params::engine,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_params_engine))
        .def_rw("repetitions", &fiction::sidb::simulation::analysis::time_to_solution_params::repetitions,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_params_repetitions))
        .def_rw("confidence_level", &fiction::sidb::simulation::analysis::time_to_solution_params::confidence_level,
                DOC(fiction_sidb_simulation_analysis_time_to_solution_params_confidence_level));
    /**
     * Statistics.
     */
    py::class_<fiction::sidb::simulation::analysis::time_to_solution_stats>(
        m, "time_to_solution_stats", DOC(fiction_sidb_simulation_analysis_time_to_solution_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::sidb::simulation::analysis::time_to_solution_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::sidb::simulation::analysis::time_to_solution_stats::report,
             DOC(fiction_sidb_simulation_analysis_time_to_solution_stats_report))
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
        [](const fiction::sidb::layout& lyt, const fiction::sidb::simulation::engines::quicksim_params& qs_params,
           const fiction::sidb::simulation::analysis::time_to_solution_params& tts_params,
           fiction::sidb::simulation::analysis::time_to_solution_stats*        ps)
        { fiction::sidb::simulation::analysis::time_to_solution(lyt, qs_params, tts_params, ps); },
        py::arg("lyt"), py::arg("quicksim_params"),
        py::arg("tts_params") = fiction::sidb::simulation::analysis::time_to_solution_params{}, py::arg("ps") = nullptr,
        DOC(fiction_sidb_simulation_analysis_time_to_solution));
    m.def("time_to_solution_for_given_simulation_results",
          &fiction::sidb::simulation::analysis::time_to_solution_for_given_simulation_results, py::arg("results_exact"),
          py::arg("results_heuristic"), py::arg("confidence_level") = 0.997, py::arg("ps") = nullptr,
          DOC(fiction_sidb_simulation_analysis_time_to_solution_for_given_simulation_results));
}

}  // namespace pyfiction
