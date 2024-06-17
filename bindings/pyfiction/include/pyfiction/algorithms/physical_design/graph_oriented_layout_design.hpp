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
        .def_readwrite("verbose", &fiction::graph_oriented_layout_design_params::verbose,
                       DOC(fiction_graph_oriented_layout_design_params_verbose))
        .def_readwrite("high_effort", &fiction::graph_oriented_layout_design_params::high_effort,
                       DOC(fiction_graph_oriented_layout_design_params_high_effort))
        .def_readwrite("return_first", &fiction::graph_oriented_layout_design_params::return_first,
                       DOC(fiction_graph_oriented_layout_design_params_return_first))

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

        ;

    m.def("graph_oriented_layout_design",
          &fiction::graph_oriented_layout_design<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::graph_oriented_layout_design_params{}, "statistics"_a = nullptr,
          DOC(fiction_graph_oriented_layout_design));
}

}  // namespace pyfiction

#endif  // PYFICTION_GRAPH_ORIENTED_LAYOUT_DESIGN_HPP
