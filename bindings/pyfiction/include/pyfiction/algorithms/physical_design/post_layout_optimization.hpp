//
// Created by simon on 22.09.23.
//

#ifndef PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
#define PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>
#include <fiction/types.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

/**
 * Post-layout optimization.
 */
inline void post_layout_optimization(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::post_layout_optimization_params>(m, "post_layout_optimization_params",
                                                         DOC(fiction_post_layout_optimization_params))
        .def(py::init<>())

        ;

    py::class_<fiction::post_layout_optimization_stats>(m, "post_layout_optimization_stats",
                                                        DOC(fiction_post_layout_optimization_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::post_layout_optimization_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def_readonly("time_total", &fiction::post_layout_optimization_stats::time_total)

        ;

    m.def("post_layout_optimization", &fiction::post_layout_optimization<py_cartesian_gate_layout>, "layout"_a,
          "parameters"_a = fiction::post_layout_optimization_params{}, "statistics"_a = nullptr,
          DOC(fiction_post_layout_optimization));
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
