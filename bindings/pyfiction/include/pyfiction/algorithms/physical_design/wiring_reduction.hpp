//
// Created by simon on 23.04.24.
//

#ifndef PYFICTION_WIRING_REDUCTION_HPP
#define PYFICTION_WIRING_REDUCTION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/wiring_reduction.hpp>
#include <fiction/types.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

/**
 * Wiring reduction.
 */
inline void wiring_reduction(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::wiring_reduction_stats>(m, "wiring_reduction_stats", DOC(fiction_wiring_reduction_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::wiring_reduction_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def_readonly("time_total", &fiction::wiring_reduction_stats::time_total)

        ;

    m.def("wiring_reduction", &fiction::wiring_reduction<py_cartesian_gate_layout>, "layout"_a,
          "statistics"_a = nullptr, DOC(fiction_wiring_reduction));
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
