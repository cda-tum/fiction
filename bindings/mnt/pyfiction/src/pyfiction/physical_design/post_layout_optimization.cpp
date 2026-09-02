/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/post_layout_optimization.hpp>

#include <cstdint>
#include <optional>
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

void post_layout_optimization(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::post_layout_optimization_params>(
        m, "post_layout_optimization_params", DOC(fiction_physical_design_post_layout_optimization_params))
        .def(py::init<>(), "Default constructor.")
        .def_prop_rw(
            "max_gate_relocations",
            [](const fiction::physical_design::post_layout_optimization_params& p) -> py::object
            {
                if (p.max_gate_relocations.has_value())
                {
                    return py::cast(p.max_gate_relocations.value());
                }
                return py::none();
            },
            [](fiction::physical_design::post_layout_optimization_params& p, const py::object& value)
            {
                if (value.is_none())
                {
                    p.max_gate_relocations = std::nullopt;
                }
                else
                {
                    p.max_gate_relocations = py::cast<uint64_t>(value);
                }
            },
            DOC(fiction_physical_design_post_layout_optimization_params_max_gate_relocations))
        .def_rw("optimize_pos_only", &fiction::physical_design::post_layout_optimization_params::optimize_pos_only,
                DOC(fiction_physical_design_post_layout_optimization_params_optimize_pos_only))
        .def_rw("planar_optimization", &fiction::physical_design::post_layout_optimization_params::planar_optimization,
                DOC(fiction_physical_design_post_layout_optimization_params_planar_optimization))
        .def_rw("timeout", &fiction::physical_design::post_layout_optimization_params::timeout,
                DOC(fiction_physical_design_post_layout_optimization_params_timeout));

    py::class_<fiction::physical_design::post_layout_optimization_stats>(
        m, "post_layout_optimization_stats", DOC(fiction_physical_design_post_layout_optimization_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::post_layout_optimization_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::physical_design::post_layout_optimization_stats::report,
             DOC(fiction_physical_design_post_layout_optimization_stats_report))
        .def_ro("time_total", &fiction::physical_design::post_layout_optimization_stats::time_total,
                DOC(fiction_physical_design_post_layout_optimization_stats_time_total))
        .def_ro("x_size_before", &fiction::physical_design::post_layout_optimization_stats::x_size_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_x_size_before))
        .def_ro("y_size_before", &fiction::physical_design::post_layout_optimization_stats::y_size_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_y_size_before))
        .def_ro("x_size_after", &fiction::physical_design::post_layout_optimization_stats::x_size_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_x_size_after))
        .def_ro("y_size_after", &fiction::physical_design::post_layout_optimization_stats::y_size_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_y_size_after))
        .def_ro("area_improvement", &fiction::physical_design::post_layout_optimization_stats::area_improvement,
                DOC(fiction_physical_design_post_layout_optimization_stats_area_improvement))
        .def_ro("num_wires_before", &fiction::physical_design::post_layout_optimization_stats::num_wires_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_wires_before))
        .def_ro("num_wires_after", &fiction::physical_design::post_layout_optimization_stats::num_wires_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_wires_after))
        .def_ro("num_crossings_before", &fiction::physical_design::post_layout_optimization_stats::num_crossings_before,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_crossings_before))
        .def_ro("num_crossings_after", &fiction::physical_design::post_layout_optimization_stats::num_crossings_after,
                DOC(fiction_physical_design_post_layout_optimization_stats_num_crossings_after));

    m.def("post_layout_optimization", &fiction::physical_design::post_layout_optimization<py_cartesian_gate_layout>,
          py::arg("layout"), py::arg("parameters") = fiction::physical_design::post_layout_optimization_params{},
          py::arg("statistics") = nullptr, DOC(fiction_physical_design_post_layout_optimization));
}

}  // namespace pyfiction
