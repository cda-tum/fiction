//
// Created by simon on 13.06.24.
//

#ifndef PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
#define PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>

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
    using namespace pybind11::literals;

    py::class_<fiction::graph_oriented_layout_design_params>(m, "graph_oriented_layout_design_params",
                                                             DOC(fiction_graph_oriented_layout_design_params))
        .def(py::init<>())
        .def_readwrite("timeout", &fiction::graph_oriented_layout_design_params::timeout,
                       DOC(fiction_graph_oriented_layout_design_params_timeout))
        .def_readwrite("num_vertex_expansions", &fiction::graph_oriented_layout_design_params::num_vertex_expansions,
                       DOC(fiction_graph_oriented_layout_design_params_num_vertex_expansions))
        .def_readwrite("verbose", &fiction::graph_oriented_layout_design_params::verbose,
                       DOC(fiction_graph_oriented_layout_design_params_verbose))
        .def_readwrite("high_effort_mode", &fiction::graph_oriented_layout_design_params::high_effort_mode,
                       DOC(fiction_graph_oriented_layout_design_params_high_effort_mode))
        .def_readwrite("return_first", &fiction::graph_oriented_layout_design_params::return_first,
                       DOC(fiction_graph_oriented_layout_design_params_return_first))
        .def_readwrite("planar", &fiction::graph_oriented_layout_design_params::planar,
                       DOC(fiction_graph_oriented_layout_design_params_planar))

        ;

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
          &fiction::graph_oriented_layout_design<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::graph_oriented_layout_design_params{}, "statistics"_a = nullptr,
          DOC(fiction_graph_oriented_layout_design));
}

}  // namespace pyfiction

#endif  // PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
