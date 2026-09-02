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

#include <fiction/physical_design/graph_oriented_layout_design.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void graph_oriented_layout_design(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::physical_design::graph_oriented_layout_design_params::effort_mode>(
        m, "gold_effort_mode", DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode))
        .value("HIGH_EFFICIENCY",
               fiction::physical_design::graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGH_EFFICIENCY))
        .value("HIGH_EFFORT", fiction::physical_design::graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGH_EFFORT))
        .value("HIGHEST_EFFORT",
               fiction::physical_design::graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_HIGHEST_EFFORT))
        .value("MAXIMUM_EFFORT",
               fiction::physical_design::graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_effort_mode_MAXIMUM_EFFORT));

    py::enum_<fiction::physical_design::graph_oriented_layout_design_params::cost_objective>(
        m, "gold_cost_objective", DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective))
        .value("AREA", fiction::physical_design::graph_oriented_layout_design_params::cost_objective::AREA,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_AREA))
        .value("WIRES", fiction::physical_design::graph_oriented_layout_design_params::cost_objective::WIRES,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_WIRES))
        .value("CROSSINGS", fiction::physical_design::graph_oriented_layout_design_params::cost_objective::CROSSINGS,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_CROSSINGS))
        .value("ACP", fiction::physical_design::graph_oriented_layout_design_params::cost_objective::ACP,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_ACP))
        .value("CUSTOM", fiction::physical_design::graph_oriented_layout_design_params::cost_objective::CUSTOM,
               DOC(fiction_physical_design_graph_oriented_layout_design_params_cost_objective_CUSTOM));

    py::class_<fiction::physical_design::graph_oriented_layout_design_params>(
        m, "graph_oriented_layout_design_params", DOC(fiction_physical_design_graph_oriented_layout_design_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("timeout", &fiction::physical_design::graph_oriented_layout_design_params::timeout,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_timeout))
        .def_rw("num_vertex_expansions",
                &fiction::physical_design::graph_oriented_layout_design_params::num_vertex_expansions,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_num_vertex_expansions))
        .def_rw("verbose", &fiction::physical_design::graph_oriented_layout_design_params::verbose,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_verbose))
        .def_rw("mode", &fiction::physical_design::graph_oriented_layout_design_params::mode,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_mode))
        .def_rw("cost", &fiction::physical_design::graph_oriented_layout_design_params::cost,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_cost))
        .def_rw("return_first", &fiction::physical_design::graph_oriented_layout_design_params::return_first,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_return_first))
        .def_rw("planar", &fiction::physical_design::graph_oriented_layout_design_params::planar,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_planar))
        .def_rw("enable_multithreading",
                &fiction::physical_design::graph_oriented_layout_design_params::enable_multithreading,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_enable_multithreading))
        .def_rw("seed", &fiction::physical_design::graph_oriented_layout_design_params::seed,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_seed))
        .def_rw("straight_inverters",
                &fiction::physical_design::graph_oriented_layout_design_params::straight_inverters,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_straight_inverters))
        .def_rw("tiles_to_skip_between_pis",
                &fiction::physical_design::graph_oriented_layout_design_params::tiles_to_skip_between_pis,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_tiles_to_skip_between_pis))
        .def_rw("randomize_tiles_to_skip_between_pis",
                &fiction::physical_design::graph_oriented_layout_design_params::randomize_tiles_to_skip_between_pis,
                DOC(fiction_physical_design_graph_oriented_layout_design_params_randomize_tiles_to_skip_between_pis));

    py::class_<fiction::physical_design::graph_oriented_layout_design_stats>(
        m, "graph_oriented_layout_design_stats", DOC(fiction_physical_design_graph_oriented_layout_design_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::graph_oriented_layout_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def_ro("time_total", &fiction::physical_design::graph_oriented_layout_design_stats::time_total,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::graph_oriented_layout_design_stats::x_size,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::graph_oriented_layout_design_stats::y_size,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::graph_oriented_layout_design_stats::num_gates,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::graph_oriented_layout_design_stats::num_wires,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::graph_oriented_layout_design_stats::num_crossings,
                DOC(fiction_physical_design_graph_oriented_layout_design_stats_num_crossings));

    m.def("graph_oriented_layout_design",
          &fiction::physical_design::graph_oriented_layout_design<py_cartesian_gate_layout, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::physical_design::graph_oriented_layout_design_params{},
          py::arg("statistics") = nullptr, py::arg("custom_cost_objective") = nullptr,
          DOC(fiction_physical_design_graph_oriented_layout_design));
}

}  // namespace pyfiction
