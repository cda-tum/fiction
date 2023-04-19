//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_ORTHOGONAL_HPP
#define PYFICTION_ORTHOGONAL_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace pyfiction
{

/**
 * OGD-based physical design.
 */
inline void orthogonal(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

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

        ;

    m.def("orthogonal", &fiction::orthogonal<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::orthogonal_physical_design_params{}, "statistics"_a = nullptr,
          DOC(fiction_orthogonal));
}

}  // namespace pyfiction

#endif  // PYFICTION_ORTHOGONAL_HPP
