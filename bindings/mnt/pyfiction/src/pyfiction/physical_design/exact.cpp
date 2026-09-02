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
 * @author Marcel Walter (marcelwa)
 * @author Simon Hofmann (simon1hofmann)
 */

#if (FICTION_Z3_SOLVER)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/exact.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void exact(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::physical_design::technology_constraints>(m, "technology_constraints",
                                                                DOC(fiction_physical_design_technology_constraints))
        .value("NONE", fiction::physical_design::technology_constraints::NONE,
               DOC(fiction_physical_design_technology_constraints_NONE))
        .value("TOPOLINANO", fiction::physical_design::technology_constraints::TOPOLINANO,
               DOC(fiction_physical_design_technology_constraints_TOPOLINANO));

    py::class_<fiction::physical_design::exact_physical_design_params>(
        m, "exact_params", DOC(fiction_physical_design_exact_physical_design_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("scheme", &fiction::physical_design::exact_physical_design_params::scheme,
                DOC(fiction_physical_design_exact_physical_design_params_scheme))
        .def_rw("upper_bound_x", &fiction::physical_design::exact_physical_design_params::upper_bound_x,
                DOC(fiction_physical_design_exact_physical_design_params_upper_bound_x))
        .def_rw("upper_bound_y", &fiction::physical_design::exact_physical_design_params::upper_bound_y,
                DOC(fiction_physical_design_exact_physical_design_params_upper_bound_y))
        .def_rw("fixed_size", &fiction::physical_design::exact_physical_design_params::fixed_size,
                DOC(fiction_physical_design_exact_physical_design_params_fixed_size))
        .def_rw("num_threads", &fiction::physical_design::exact_physical_design_params::num_threads,
                DOC(fiction_physical_design_exact_physical_design_params_num_threads))
        .def_rw("crossings", &fiction::physical_design::exact_physical_design_params::crossings,
                DOC(fiction_physical_design_exact_physical_design_params_crossings))
        .def_rw("border_io", &fiction::physical_design::exact_physical_design_params::border_io,
                DOC(fiction_physical_design_exact_physical_design_params_border_io))
        .def_rw("straight_inverters", &fiction::physical_design::exact_physical_design_params::straight_inverters,
                DOC(fiction_physical_design_exact_physical_design_params_straight_inverters))
        .def_rw("desynchronize", &fiction::physical_design::exact_physical_design_params::desynchronize,
                DOC(fiction_physical_design_exact_physical_design_params_desynchronize))
        .def_rw("minimize_wires", &fiction::physical_design::exact_physical_design_params::minimize_wires,
                DOC(fiction_physical_design_exact_physical_design_params_minimize_wires))
        .def_rw("minimize_crossings", &fiction::physical_design::exact_physical_design_params::minimize_crossings,
                DOC(fiction_physical_design_exact_physical_design_params_minimize_crossings))
        .def_rw("timeout", &fiction::physical_design::exact_physical_design_params::timeout,
                DOC(fiction_physical_design_exact_physical_design_params_timeout))
        .def_rw("technology_specifics", &fiction::physical_design::exact_physical_design_params::technology_specifics,
                DOC(fiction_physical_design_exact_physical_design_params_technology_specifics));

    py::class_<fiction::physical_design::exact_physical_design_stats>(
        m, "exact_stats", DOC(fiction_physical_design_exact_physical_design_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::exact_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::physical_design::exact_physical_design_stats::report,
             DOC(fiction_physical_design_exact_physical_design_stats_report))
        .def_ro("time_total", &fiction::physical_design::exact_physical_design_stats::time_total,
                DOC(fiction_physical_design_exact_physical_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::exact_physical_design_stats::x_size,
                DOC(fiction_physical_design_exact_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::exact_physical_design_stats::y_size,
                DOC(fiction_physical_design_exact_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::exact_physical_design_stats::num_gates,
                DOC(fiction_physical_design_exact_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::exact_physical_design_stats::num_wires,
                DOC(fiction_physical_design_exact_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::exact_physical_design_stats::num_crossings,
                DOC(fiction_physical_design_exact_physical_design_stats_num_crossings))
        .def_ro("num_aspect_ratios", &fiction::physical_design::exact_physical_design_stats::num_aspect_ratios,
                DOC(fiction_physical_design_exact_physical_design_stats_num_aspect_ratios));

    m.def("exact_cartesian", &fiction::physical_design::exact<py_cartesian_gate_layout, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::physical_design::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_physical_design_exact));

    m.def("exact_shifted_cartesian",
          &fiction::physical_design::exact<py_shifted_cartesian_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::physical_design::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_physical_design_exact));

    m.def("exact_hexagonal", &fiction::physical_design::exact<py_hexagonal_gate_layout, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::physical_design::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_physical_design_exact));
}

}  // namespace pyfiction

#else  // FICTION_Z3_SOLVER

namespace pyfiction
{

/**
 * Disable SMT-based exact physical design.
 */
void exact([[maybe_unused]] nanobind::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_Z3_SOLVER
