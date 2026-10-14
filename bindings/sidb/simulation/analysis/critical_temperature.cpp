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
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>

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
    using fiction::sidb::simulation::analysis::critical_temperature_params;
    using fiction::sidb::simulation::analysis::critical_temperature_stats;
    using fiction::sidb::simulation::logic::bdl_pair;
    using fiction::sidb::simulation::logic::bdl_wire;

    m.def(
        "critical_temperature_gate_based",
        [](const layout& lyt, const std::vector<py_tt>& spec, const critical_temperature_params& params,
           critical_temperature_stats* stats)
        { return fiction::sidb::simulation::analysis::critical_temperature_gate_based(lyt, spec, params, stats); },
        nb::arg("lyt"), nb::arg("spec"), nb::arg("params") = critical_temperature_params{}, nb::arg("stats") = nullptr,
        nb::call_guard<nb::gil_scoped_release>(),
        DOC(fiction_sidb_simulation_analysis_critical_temperature_gate_based));
    m.def(
        "critical_temperature_gate_based",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec,
           const critical_temperature_params& params, const std::vector<bdl_pair>& output_bdl_pairs,
           const std::vector<bdl_wire>& input_bdl_wires, const std::vector<bdl_wire>& output_bdl_wires,
           critical_temperature_stats* stats)
        {
            return fiction::sidb::simulation::analysis::critical_temperature_gate_based(
                input_pattern_layouts, spec, params, output_bdl_pairs, input_bdl_wires, output_bdl_wires, stats);
        },
        nb::arg("input_pattern_layouts"), nb::arg("spec"), nb::arg("params"), nb::arg("output_bdl_pairs"),
        nb::arg("input_bdl_wires"), nb::arg("output_bdl_wires"), nb::arg("stats") = nullptr,
        nb::call_guard<nb::gil_scoped_release>(),
        DOC(fiction_sidb_simulation_analysis_critical_temperature_gate_based_2));
    m.def(
        "critical_temperature_non_gate_based",
        [](const layout& lyt, const critical_temperature_params& params, critical_temperature_stats* stats)
        { return fiction::sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, stats); },
        nb::arg("lyt"), nb::arg("params") = critical_temperature_params{}, nb::arg("stats") = nullptr,
        nb::call_guard<nb::gil_scoped_release>(),
        DOC(fiction_sidb_simulation_analysis_critical_temperature_non_gate_based));
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
    nb::class_<fiction::sidb::simulation::analysis::critical_temperature_params>(
        m, "critical_temperature_params",
        pyfiction::progress_type_slots<fiction::sidb::simulation::analysis::critical_temperature_params>(),
        DOC(fiction_sidb_simulation_analysis_critical_temperature_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("operational_params",
                &fiction::sidb::simulation::analysis::critical_temperature_params::operational_params,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params))
        .def_rw("confidence_level", &fiction::sidb::simulation::analysis::critical_temperature_params::confidence_level,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_confidence_level))
        .def_rw("max_temperature", &fiction::sidb::simulation::analysis::critical_temperature_params::max_temperature,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_max_temperature))
        .def_rw("on_progress", &fiction::sidb::simulation::analysis::critical_temperature_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_on_progress))
        .def_rw("on_worker_progress",
                &fiction::sidb::simulation::analysis::critical_temperature_params::on_worker_progress,
                pyfiction::ON_WORKER_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_on_worker_progress));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_critical_temperature_functions(m);
}

}  // namespace pyfiction
