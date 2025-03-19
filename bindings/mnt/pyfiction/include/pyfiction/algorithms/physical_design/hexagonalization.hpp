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

    py::register_exception<fiction::hexagonalization_route_inputs_error>(m, "hexagonalization_route_inputs_error",
                                                                         PyExc_RuntimeError);

    py::enum_<fiction::hexagonalization_params::hexagonalization_input_output_mode>(
        m, "hexagonalization_input_output_mode",
        DOC(fiction_hexagonalization_params_hexagonalization_input_output_mode))
        .value("NONE", fiction::hexagonalization_params::hexagonalization_input_output_mode::NONE,
               DOC(fiction_hexagonalization_params_hexagonalization_input_output_mode_NONE))
        .value("EXTEND", fiction::hexagonalization_params::hexagonalization_input_output_mode::EXTEND,
               DOC(fiction_hexagonalization_params_hexagonalization_input_output_mode_EXTEND))
        .value("EXTEND_PLANAR", fiction::hexagonalization_params::hexagonalization_input_output_mode::EXTEND_PLANAR,
               DOC(fiction_hexagonalization_params_hexagonalization_input_output_mode_EXTEND_PLANAR))

        ;

    py::class_<fiction::hexagonalization_params>(m, "hexagonalization_params", DOC(fiction_hexagonalization_params))
        .def(py::init<>())
        .def_readwrite("input_mode", &fiction::hexagonalization_params::input_mode,
                       DOC(fiction_hexagonalization_params_input_mode));
    .def_readwrite("output_mode", &fiction::hexagonalization_params::output_mode,
                   DOC(fiction_hexagonalization_params_output_mode));

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
