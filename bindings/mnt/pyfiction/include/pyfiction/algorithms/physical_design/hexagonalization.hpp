//
// Created by simon on 22.09.23.
//

#ifndef PYFICTION_HEXAGONALIZATION_HPP
#define PYFICTION_HEXAGONALIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/hexagonalization.hpp>
#include <fiction/types.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

inline void hexagonalization(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::fgl_parsing_error>(m, "hexagonalization_route_inputs_error", PyExc_RuntimeError);

    py::class_<fiction::hexagonalization_params>(m, "hexagonalization_params", DOC(fiction_hexagonalization_params))
        .def(py::init<>())
        .def_readwrite("place_inputs_in_top_row", &fiction::hexagonalization_params::place_inputs_in_top_row,
                       DOC(fiction_hexagonalization_params_place_inputs_in_top_row));
    .def_readwrite("planar_routing_for_moved_inputs",
                   &fiction::hexagonalization_params::planar_routing_for_moved_inputs,
                   DOC(fiction_hexagonalization_params_planar_routing_for_moved_inputs));

    py::class_<fiction::hexagonalization_stats>(m, "hexagonalization_stats", DOC(fiction_hexagonalization_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::hexagonalization_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def_readonly("time_total", &fiction::hexagonalization_stats::time_total,
                      DOC(fiction_hexagonalization_stats_duration));

    m.def("hexagonalization", &fiction::hexagonalization<py_hexagonal_gate_layout, py_cartesian_gate_layout>,
          py::arg("layout"), py::arg("parameters") = fiction::hexagonalization_params{},
          py::arg("statistics") = nullptr, DOC(fiction_hexagonalization));
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
