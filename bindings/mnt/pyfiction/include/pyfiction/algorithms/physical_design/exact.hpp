//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_EXACT_HPP
#define PYFICTION_EXACT_HPP

#if (FICTION_Z3_SOLVER)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/exact.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace pyfiction
{

/**
 * SMT-based exact physical design.
 */
inline void exact(pybind11::module& m)
{
    namespace py = pybind11;
    namespace py = pybind11;

    py::enum_<fiction::technology_constraints>(m, "technology_constraints", DOC(fiction_technology_constraints))
        .value("NONE", fiction::technology_constraints::NONE, DOC(fiction_technology_constraints_NONE))
        .value("TOPOLINANO", fiction::technology_constraints::TOPOLINANO,
               DOC(fiction_technology_constraints_TOPOLINANO))

        ;

    py::class_<fiction::exact_physical_design_params>(m, "exact_params", DOC(fiction_exact_physical_design_params))
        .def(py::init<>())
        .def_readwrite("scheme", &fiction::exact_physical_design_params::scheme,
                       DOC(fiction_exact_physical_design_params_scheme))
        .def_readwrite("upper_bound_x", &fiction::exact_physical_design_params::upper_bound_x,
                       DOC(fiction_exact_physical_design_params_upper_bound_x))
        .def_readwrite("upper_bound_y", &fiction::exact_physical_design_params::upper_bound_y,
                       DOC(fiction_exact_physical_design_params_upper_bound_y))
        .def_readwrite("fixed_size", &fiction::exact_physical_design_params::fixed_size,
                       DOC(fiction_exact_physical_design_params_fixed_size))
        .def_readwrite("num_threads", &fiction::exact_physical_design_params::num_threads,
                       DOC(fiction_exact_physical_design_params_num_threads))
        .def_readwrite("crossings", &fiction::exact_physical_design_params::crossings,
                       DOC(fiction_exact_physical_design_params_crossings))
        .def_readwrite("border_io", &fiction::exact_physical_design_params::border_io,
                       DOC(fiction_exact_physical_design_params_border_io))
        .def_readwrite("straight_inverters", &fiction::exact_physical_design_params::straight_inverters,
                       DOC(fiction_exact_physical_design_params_straight_inverters))
        .def_readwrite("desynchronize", &fiction::exact_physical_design_params::desynchronize,
                       DOC(fiction_exact_physical_design_params_desynchronize))
        .def_readwrite("minimize_wires", &fiction::exact_physical_design_params::minimize_wires,
                       DOC(fiction_exact_physical_design_params_minimize_wires))
        .def_readwrite("minimize_crossings", &fiction::exact_physical_design_params::minimize_crossings,
                       DOC(fiction_exact_physical_design_params_minimize_crossings))
        .def_readwrite("timeout", &fiction::exact_physical_design_params::timeout,
                       DOC(fiction_exact_physical_design_params_timeout))
        .def_readwrite("technology_specifics", &fiction::exact_physical_design_params::technology_specifics,
                       DOC(fiction_exact_physical_design_params_technology_specifics))

        ;

    py::class_<fiction::exact_physical_design_stats>(m, "exact_stats", DOC(fiction_exact_physical_design_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::exact_physical_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def("report", &fiction::exact_physical_design_stats::report, DOC(fiction_exact_physical_design_stats_report))
        .def_readonly("time_total", &fiction::exact_physical_design_stats::time_total,
                      DOC(fiction_exact_physical_design_stats_duration))
        .def_readonly("x_size", &fiction::exact_physical_design_stats::x_size,
                      DOC(fiction_exact_physical_design_stats_x_size))
        .def_readonly("y_size", &fiction::exact_physical_design_stats::y_size,
                      DOC(fiction_exact_physical_design_stats_y_size))
        .def_readonly("num_gates", &fiction::exact_physical_design_stats::num_gates,
                      DOC(fiction_exact_physical_design_stats_num_gates))
        .def_readonly("num_wires", &fiction::exact_physical_design_stats::num_wires,
                      DOC(fiction_exact_physical_design_stats_num_wires))
        .def_readonly("num_crossings", &fiction::exact_physical_design_stats::num_crossings,
                      DOC(fiction_exact_physical_design_stats_num_crossings))
        .def_readonly("num_aspect_ratios", &fiction::exact_physical_design_stats::num_aspect_ratios,
                      DOC(fiction_exact_physical_design_stats_num_aspect_ratios))

        ;

    m.def("exact_cartesian", &fiction::exact<py_cartesian_gate_layout<py_offset_coordinate>, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_exact));

    m.def("exact_shifted_cartesian",
          &fiction::exact<py_shifted_cartesian_gate_layout<py_offset_coordinate>, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::exact_physical_design_params{}, py::arg("statistics") = nullptr);

    m.def("exact_hexagonal", &fiction::exact<py_hexagonal_gate_layout<py_offset_coordinate>, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_exact));
}

}  // namespace pyfiction

#else  // FICTION_Z3_SOLVER

#include <pybind11/pybind11.h>

namespace pyfiction
{

/**
 * Disable SMT-based exact physical design.
 */
inline void exact([[maybe_unused]] pybind11::module& m) {}

}  // namespace pyfiction

#endif  // FICTION_Z3_SOLVER

#endif  // PYFICTION_EXACT_HPP
