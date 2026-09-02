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
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/wiring_reduction.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/chrono.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void wiring_reduction(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::wiring_reduction_params>(m, "wiring_reduction_params",
                                                                  DOC(fiction_physical_design_wiring_reduction_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("timeout", &fiction::physical_design::wiring_reduction_params::timeout,
                DOC(fiction_physical_design_wiring_reduction_params_timeout));

    py::class_<fiction::physical_design::wiring_reduction_stats>(m, "wiring_reduction_stats",
                                                                 DOC(fiction_physical_design_wiring_reduction_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::wiring_reduction_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::physical_design::wiring_reduction_stats::report,
             DOC(fiction_physical_design_wiring_reduction_stats_report))
        .def_ro("time_total", &fiction::physical_design::wiring_reduction_stats::time_total,
                DOC(fiction_physical_design_wiring_reduction_stats_time_total))
        .def_ro("x_size_before", &fiction::physical_design::wiring_reduction_stats::x_size_before,
                DOC(fiction_physical_design_wiring_reduction_stats_x_size_before))
        .def_ro("y_size_before", &fiction::physical_design::wiring_reduction_stats::y_size_before,
                DOC(fiction_physical_design_wiring_reduction_stats_y_size_before))
        .def_ro("x_size_after", &fiction::physical_design::wiring_reduction_stats::x_size_after,
                DOC(fiction_physical_design_wiring_reduction_stats_x_size_after))
        .def_ro("y_size_after", &fiction::physical_design::wiring_reduction_stats::y_size_after,
                DOC(fiction_physical_design_wiring_reduction_stats_y_size_after))
        .def_ro("num_wires_before", &fiction::physical_design::wiring_reduction_stats::num_wires_before,
                DOC(fiction_physical_design_wiring_reduction_stats_num_wires_before))
        .def_ro("num_wires_after", &fiction::physical_design::wiring_reduction_stats::num_wires_after,
                DOC(fiction_physical_design_wiring_reduction_stats_num_wires_after))
        .def_ro("wiring_improvement", &fiction::physical_design::wiring_reduction_stats::wiring_improvement,
                DOC(fiction_physical_design_wiring_reduction_stats_wiring_improvement))
        .def_ro("area_improvement", &fiction::physical_design::wiring_reduction_stats::area_improvement,
                DOC(fiction_physical_design_wiring_reduction_stats_area_improvement))

        ;

    m.def("wiring_reduction", &fiction::physical_design::wiring_reduction<py_cartesian_gate_layout>, py::arg("layout"),
          py::arg("parameters") = fiction::physical_design::wiring_reduction_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_physical_design_wiring_reduction));
}

}  // namespace pyfiction
