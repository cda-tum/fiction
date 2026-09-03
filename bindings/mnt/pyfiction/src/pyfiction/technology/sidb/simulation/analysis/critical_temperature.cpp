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

#include <fiction/technology/sidb/lattice.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/analysis/critical_temperature.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp>
#include <fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp>
#include <fiction/traits.hpp>

#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Registers the critical temperature algorithms on `sidb_layout`.
 *
 * @param m The module.
 */
inline void critical_temperature_functions(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::sidb::lattice_site;
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
        py::arg("lyt"), py::arg("spec"), py::arg("params") = critical_temperature_params{}, py::arg("stats") = nullptr,
        DOC(fiction_sidb_simulation_analysis_critical_temperature_gate_based));
    m.def(
        "critical_temperature_gate_based",
        [](const std::vector<layout>& input_pattern_layouts, const std::vector<py_tt>& spec,
           const critical_temperature_params& params, const std::vector<bdl_pair<lattice_site>>& output_bdl_pairs,
           const std::vector<bdl_wire>& input_bdl_wires, const std::vector<bdl_wire>& output_bdl_wires,
           critical_temperature_stats* stats)
        {
            return fiction::sidb::simulation::analysis::critical_temperature_gate_based(
                input_pattern_layouts, spec, params, output_bdl_pairs, input_bdl_wires, output_bdl_wires, stats);
        },
        py::arg("input_pattern_layouts"), py::arg("spec"), py::arg("params"), py::arg("output_bdl_pairs"),
        py::arg("input_bdl_wires"), py::arg("output_bdl_wires"), py::arg("stats") = nullptr,
        DOC(fiction_sidb_simulation_analysis_critical_temperature_gate_based_2));
    m.def(
        "critical_temperature_non_gate_based",
        [](const layout& lyt, const critical_temperature_params& params, critical_temperature_stats* stats)
        { return fiction::sidb::simulation::analysis::critical_temperature_non_gate_based(lyt, params, stats); },
        py::arg("lyt"), py::arg("params") = critical_temperature_params{}, py::arg("stats") = nullptr,
        DOC(fiction_sidb_simulation_analysis_critical_temperature_non_gate_based));
}

}  // namespace detail

void critical_temperature(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Critical temperature statistics.
     */
    py::class_<fiction::sidb::simulation::analysis::critical_temperature_stats>(
        m, "critical_temperature_stats", DOC(fiction_sidb_simulation_analysis_critical_temperature_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::sidb::simulation::analysis::critical_temperature_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::sidb::simulation::analysis::critical_temperature_stats::report,
             DOC(fiction_sidb_simulation_analysis_critical_temperature_stats_report))
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
    py::class_<fiction::sidb::simulation::analysis::critical_temperature_params>(
        m, "critical_temperature_params", DOC(fiction_sidb_simulation_analysis_critical_temperature_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_params",
                &fiction::sidb::simulation::analysis::critical_temperature_params::operational_params,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params))
        .def_rw("confidence_level", &fiction::sidb::simulation::analysis::critical_temperature_params::confidence_level,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_confidence_level))
        .def_rw("max_temperature", &fiction::sidb::simulation::analysis::critical_temperature_params::max_temperature,
                DOC(fiction_sidb_simulation_analysis_critical_temperature_params_max_temperature));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::critical_temperature_functions(m);
}

}  // namespace pyfiction
