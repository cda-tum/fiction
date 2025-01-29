//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_ORTHOGONAL_HPP
#define PYFICTION_ORTHOGONAL_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>

#include <pybind11/pybind11.h>

#include <sstream>

namespace pyfiction
{

/**
 * OGD-based physical design.
 */
inline void orthogonal(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::orthogonal_physical_design_params>(m, "orthogonal_params",
                                                           DOC(fiction_orthogonal_physical_design_params))
        .def(py::init<>())

        ;

    py::class_<fiction::orthogonal_physical_design_stats>(m, "orthogonal_stats",
                                                          DOC(fiction_orthogonal_physical_design_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::orthogonal_physical_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def("report", &fiction::orthogonal_physical_design_stats::report,
             DOC(fiction_orthogonal_physical_design_stats_report))
        .def_readonly("time_total", &fiction::orthogonal_physical_design_stats::time_total,
                      DOC(fiction_orthogonal_physical_design_stats_duration))
        .def_readonly("x_size", &fiction::orthogonal_physical_design_stats::x_size,
                      DOC(fiction_orthogonal_physical_design_stats_x_size))
        .def_readonly("y_size", &fiction::orthogonal_physical_design_stats::y_size,
                      DOC(fiction_orthogonal_physical_design_stats_y_size))
        .def_readonly("num_gates", &fiction::orthogonal_physical_design_stats::num_gates,
                      DOC(fiction_orthogonal_physical_design_stats_num_gates))
        .def_readonly("num_wires", &fiction::orthogonal_physical_design_stats::num_wires,
                      DOC(fiction_orthogonal_physical_design_stats_num_wires))
        .def_readonly("num_crossings", &fiction::orthogonal_physical_design_stats::num_crossings,
                      DOC(fiction_orthogonal_physical_design_stats_num_crossings))

        ;

    m.def("orthogonal", &fiction::orthogonal<py_cartesian_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::orthogonal_physical_design_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_orthogonal));
}

}  // namespace pyfiction

#endif  // PYFICTION_ORTHOGONAL_HPP
