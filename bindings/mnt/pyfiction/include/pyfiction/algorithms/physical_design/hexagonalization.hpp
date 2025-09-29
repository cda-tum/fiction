//
// Created by simon on 22.09.23.
//

#ifndef PYFICTION_HEXAGONALIZATION_HPP
#define PYFICTION_HEXAGONALIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/hexagonalization.hpp>

#include <pybind11/pybind11.h>

#include <sstream>

namespace pyfiction
{

inline void hexagonalization(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::hexagonalization_io_pin_routing_error>(m, "hexagonalization_io_pin_routing_error",
                                                                           PyExc_RuntimeError);

    py::enum_<fiction::hexagonalization_params::io_pin_extension_mode>(
        m, "hexagonalization_io_pin_extension_mode", DOC(fiction_hexagonalization_params_io_pin_extension_mode))
        .value("NONE", fiction::hexagonalization_params::io_pin_extension_mode::NONE,
               DOC(fiction_hexagonalization_params_io_pin_extension_mode_NONE))
        .value("EXTEND", fiction::hexagonalization_params::io_pin_extension_mode::EXTEND,
               DOC(fiction_hexagonalization_params_io_pin_extension_mode_EXTEND))
        .value("EXTEND_PLANAR", fiction::hexagonalization_params::io_pin_extension_mode::EXTEND_PLANAR,
               DOC(fiction_hexagonalization_params_io_pin_extension_mode_EXTEND_PLANAR))

        ;

    py::class_<fiction::hexagonalization_params>(m, "hexagonalization_params", DOC(fiction_hexagonalization_params))
        .def(py::init<>())
        .def_readwrite("input_pin_extension", &fiction::hexagonalization_params::input_pin_extension,
                       DOC(fiction_hexagonalization_params_input_pin_extension))
        .def_readwrite("output_pin_extension", &fiction::hexagonalization_params::output_pin_extension,
                       DOC(fiction_hexagonalization_params_output_pin_extension));

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
                      DOC(fiction_hexagonalization_stats_duration))
        .def_readonly("x_size", &fiction::hexagonalization_stats::x_size, DOC(fiction_hexagonalization_stats_x_size))
        .def_readonly("y_size", &fiction::hexagonalization_stats::y_size, DOC(fiction_hexagonalization_stats_y_size))
        .def_readonly("num_gates", &fiction::hexagonalization_stats::num_gates,
                      DOC(fiction_hexagonalization_stats_num_gates))
        .def_readonly("num_wires", &fiction::hexagonalization_stats::num_wires,
                      DOC(fiction_hexagonalization_stats_num_wires))
        .def_readonly("num_crossings", &fiction::hexagonalization_stats::num_crossings,
                      DOC(fiction_hexagonalization_stats_num_crossings));

    m.def("hexagonalization", &fiction::hexagonalization<py_hexagonal_gate_layout, py_cartesian_gate_layout>,
          py::arg("layout"), py::arg("parameters") = fiction::hexagonalization_params{},
          py::arg("statistics") = nullptr, DOC(fiction_hexagonalization));
}

}  // namespace pyfiction

#endif  // PYFICTION_HEXAGONALIZATION_HPP
