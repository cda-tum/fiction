//
// Created by simon on 13.06.24.
//

#ifndef PYFICTION_GRAPH_ENHANCED_LAYOUT_SEARCH_HPP
#define PYFICTION_GRAPH_ENHANCED_LAYOUT_SEARCH_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/graph_enhanced_layout_search.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace pyfiction
{

/**
 * Graph-enhanced layout search.
 */
inline void graph_enhanced_layout_search(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::graph_enhanced_layout_search_params>(m, "graph_enhanced_layout_search_params",
                                                             DOC(fiction_graph_enhanced_layout_search_params))
        .def(py::init<>())
        .def_readwrite("timeout", &fiction::graph_enhanced_layout_search_params::timeout,
                       DOC(fiction_graph_enhanced_layout_search_params_timeout))
        .def_readwrite("verbose", &fiction::graph_enhanced_layout_search_params::verbose,
                       DOC(fiction_graph_enhanced_layout_search_params_verbose))
        .def_readwrite("high_effort", &fiction::graph_enhanced_layout_search_params::high_effort,
                       DOC(fiction_graph_enhanced_layout_search_params_high_effort))
        .def_readwrite("return_first", &fiction::graph_enhanced_layout_search_params::return_first,
                       DOC(fiction_graph_enhanced_layout_search_params_return_first))

        ;

    py::class_<fiction::graph_enhanced_layout_search_stats>(m, "graph_enhanced_layout_search_stats",
                                                            DOC(fiction_graph_enhanced_layout_search_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::graph_enhanced_layout_search_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("graph_enhanced_layout_search",
          &fiction::graph_enhanced_layout_search<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::graph_enhanced_layout_search_params{}, "statistics"_a = nullptr,
          DOC(fiction_graph_enhanced_layout_search));
}

}  // namespace pyfiction

#endif  // PYFICTION_GRAPH_ENHANCED_LAYOUT_SEARCH_HPP
