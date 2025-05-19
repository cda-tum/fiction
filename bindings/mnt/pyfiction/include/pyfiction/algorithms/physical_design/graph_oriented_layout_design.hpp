//
// Created by simon on 13.06.24.
//

#ifndef PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
#define PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>

#include <pybind11/functional.h>  // Needed for std::function bindings
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace pyfiction
{

/**
 * Graph-oriented layout design.
 */
inline void graph_oriented_layout_design(pybind11::module& m)
{
    namespace py = pybind11;
    namespace py = pybind11;

    py::enum_<fiction::graph_oriented_layout_design_params::effort_mode>(
        m, "gold_effort_mode", DOC(fiction_graph_oriented_layout_design_params_effort_mode))
        .value("HIGH_EFFICIENCY", fiction::graph_oriented_layout_design_params::effort_mode::HIGH_EFFICIENCY,
               DOC(fiction_graph_oriented_layout_design_params_effort_mode_HIGH_EFFICIENCY))
        .value("HIGH_EFFORT", fiction::graph_oriented_layout_design_params::effort_mode::HIGH_EFFORT,
               DOC(fiction_graph_oriented_layout_design_params_effort_mode_HIGH_EFFORT))
        .value("HIGHEST_EFFORT", fiction::graph_oriented_layout_design_params::effort_mode::HIGHEST_EFFORT,
               DOC(fiction_graph_oriented_layout_design_params_effort_mode_HIGHEST_EFFORT))
        .value("MAXIMUM_EFFORT", fiction::graph_oriented_layout_design_params::effort_mode::MAXIMUM_EFFORT,
               DOC(fiction_graph_oriented_layout_design_params_effort_mode_MAXIMUM_EFFORT));

    py::enum_<fiction::graph_oriented_layout_design_params::cost_objective>(
        m, "gold_cost_objective", DOC(fiction_graph_oriented_layout_design_params_cost_objective))
        .value("AREA", fiction::graph_oriented_layout_design_params::cost_objective::AREA,
               DOC(fiction_graph_oriented_layout_design_params_cost_objective_AREA))
        .value("WIRES", fiction::graph_oriented_layout_design_params::cost_objective::WIRES,
               DOC(fiction_graph_oriented_layout_design_params_cost_objective_WIRES))
        .value("CROSSINGS", fiction::graph_oriented_layout_design_params::cost_objective::CROSSINGS,
               DOC(fiction_graph_oriented_layout_design_params_cost_objective_CROSSINGS))
        .value("ACP", fiction::graph_oriented_layout_design_params::cost_objective::ACP,
               DOC(fiction_graph_oriented_layout_design_params_cost_objective_ACP))
        .value("CUSTOM", fiction::graph_oriented_layout_design_params::cost_objective::CUSTOM,
               DOC(fiction_graph_oriented_layout_design_params_cost_objective_CUSTOM))

        ;

    py::class_<fiction::graph_oriented_layout_design_params>(m, "graph_oriented_layout_design_params",
                                                             DOC(fiction_graph_oriented_layout_design_params))
        .def(py::init<>())
        .def_readwrite("timeout", &fiction::graph_oriented_layout_design_params::timeout,
                       DOC(fiction_graph_oriented_layout_design_params_timeout))
        .def_readwrite("num_vertex_expansions", &fiction::graph_oriented_layout_design_params::num_vertex_expansions,
                       DOC(fiction_graph_oriented_layout_design_params_num_vertex_expansions))
        .def_readwrite("verbose", &fiction::graph_oriented_layout_design_params::verbose,
                       DOC(fiction_graph_oriented_layout_design_params_verbose))
        .def_readwrite("mode", &fiction::graph_oriented_layout_design_params::mode,
                       DOC(fiction_graph_oriented_layout_design_params_mode))
        .def_readwrite("cost", &fiction::graph_oriented_layout_design_params::cost,
                       DOC(fiction_graph_oriented_layout_design_params_cost))
        .def_readwrite("return_first", &fiction::graph_oriented_layout_design_params::return_first,
                       DOC(fiction_graph_oriented_layout_design_params_return_first))
        .def_readwrite("planar", &fiction::graph_oriented_layout_design_params::planar,
                       DOC(fiction_graph_oriented_layout_design_params_planar))
        .def_readwrite("enable_multithreading", &fiction::graph_oriented_layout_design_params::enable_multithreading,
                       DOC(fiction_graph_oriented_layout_design_params_enable_multithreading))
        .def_readwrite("seed", &fiction::graph_oriented_layout_design_params::seed,
                       DOC(fiction_graph_oriented_layout_design_params_seed));

    py::class_<fiction::graph_oriented_layout_design_stats>(m, "graph_oriented_layout_design_stats",
                                                            DOC(fiction_graph_oriented_layout_design_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::graph_oriented_layout_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def_readonly("time_total", &fiction::graph_oriented_layout_design_stats::time_total,
                      DOC(fiction_graph_oriented_layout_design_stats_duration))
        .def_readonly("x_size", &fiction::graph_oriented_layout_design_stats::x_size,
                      DOC(fiction_graph_oriented_layout_design_stats_x_size))
        .def_readonly("y_size", &fiction::graph_oriented_layout_design_stats::y_size,
                      DOC(fiction_graph_oriented_layout_design_stats_y_size))
        .def_readonly("num_gates", &fiction::graph_oriented_layout_design_stats::num_gates,
                      DOC(fiction_graph_oriented_layout_design_stats_num_gates))
        .def_readonly("num_wires", &fiction::graph_oriented_layout_design_stats::num_wires,
                      DOC(fiction_graph_oriented_layout_design_stats_num_wires))
        .def_readonly("num_crossings", &fiction::graph_oriented_layout_design_stats::num_crossings,
                      DOC(fiction_graph_oriented_layout_design_stats_num_crossings));

    m.def("graph_oriented_layout_design",
          &fiction::graph_oriented_layout_design<py_cartesian_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::graph_oriented_layout_design_params{}, py::arg("statistics") = nullptr,
          py::arg("custom_cost_objective") = nullptr, DOC(fiction_graph_oriented_layout_design));
}

}  // namespace pyfiction

#endif  // PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
