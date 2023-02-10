//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_EXACT_HPP
#define PYFICTION_EXACT_HPP

#if (FICTION_Z3_SOLVER)

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
    using namespace pybind11::literals;

    py::class_<fiction::exact_physical_design_params>(m, "exact_params")
        .def(py::init<>())
        .def_readwrite("scheme", &fiction::exact_physical_design_params::scheme)
        .def_readwrite("upper_bound_x", &fiction::exact_physical_design_params::upper_bound_x)
        .def_readwrite("upper_bound_y", &fiction::exact_physical_design_params::upper_bound_y)
        .def_readwrite("fixed_size", &fiction::exact_physical_design_params::fixed_size)
        .def_readwrite("num_threads", &fiction::exact_physical_design_params::num_threads)
        .def_readwrite("crossings", &fiction::exact_physical_design_params::crossings)
        .def_readwrite("border_io", &fiction::exact_physical_design_params::border_io)
        .def_readwrite("straight_inverters", &fiction::exact_physical_design_params::straight_inverters)
        .def_readwrite("desynchronize", &fiction::exact_physical_design_params::desynchronize)
        .def_readwrite("minimize_wires", &fiction::exact_physical_design_params::minimize_wires)
        .def_readwrite("minimize_crossings", &fiction::exact_physical_design_params::minimize_crossings)
        .def_readwrite("timeout", &fiction::exact_physical_design_params::timeout)

        ;

    py::class_<fiction::exact_physical_design_stats>(m, "exact_stats")
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::exact_physical_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("exact_cartesian", &fiction::exact<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::exact_physical_design_params{}, "statistics"_a = nullptr);

    m.def("exact_hexagonal", &fiction::exact<py_hexagonal_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::exact_physical_design_params{}, "statistics"_a = nullptr);
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
