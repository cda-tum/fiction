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
    namespace py = pybind11;

    py::class_<fiction::post_layout_optimization_params>(m, "post_layout_optimization_params",
                                                         DOC(fiction_post_layout_optimization_params))
        .def(py::init<>())
        .def_readwrite("max_gate_relocations", &fiction::post_layout_optimization_params::max_gate_relocations,
                       DOC(fiction_post_layout_optimization_params_max_gate_relocations))
        .def_readwrite("optimize_pos_only", &fiction::post_layout_optimization_params::optimize_pos_only,
                       DOC(fiction_post_layout_optimization_params_optimize_pos_only))
        .def_readwrite("planar_optimization", &fiction::post_layout_optimization_params::planar_optimization,
                       DOC(fiction_post_layout_optimization_params_planar_optimization))
        .def_readwrite("timeout", &fiction::post_layout_optimization_params::timeout,
                       DOC(fiction_post_layout_optimization_params_timeout));

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
        .def("report", &fiction::post_layout_optimization_stats::report,
             DOC(fiction_post_layout_optimization_stats_report))
        .def_readonly("time_total", &fiction::post_layout_optimization_stats::time_total,
                      DOC(fiction_post_layout_optimization_stats_duration))
        .def_readonly("x_size_before", &fiction::post_layout_optimization_stats::x_size_before,
                      DOC(fiction_post_layout_optimization_stats_x_size_before))
        .def_readonly("y_size_before", &fiction::post_layout_optimization_stats::y_size_before,
                      DOC(fiction_post_layout_optimization_stats_y_size_before))
        .def_readonly("x_size_after", &fiction::post_layout_optimization_stats::x_size_after,
                      DOC(fiction_post_layout_optimization_stats_x_size_after))
        .def_readonly("y_size_after", &fiction::post_layout_optimization_stats::y_size_after,
                      DOC(fiction_post_layout_optimization_stats_y_size_after))
        .def_readonly("area_improvement", &fiction::post_layout_optimization_stats::area_improvement,
                      DOC(fiction_post_layout_optimization_stats_area_improvement))
        .def_readonly("num_wires_before", &fiction::post_layout_optimization_stats::num_wires_before,
                      DOC(fiction_post_layout_optimization_stats_num_wires_before))
        .def_readonly("num_wires_after", &fiction::post_layout_optimization_stats::num_wires_after,
                      DOC(fiction_post_layout_optimization_stats_num_wires_after))
        .def_readonly("num_crossings_before", &fiction::post_layout_optimization_stats::num_crossings_before,
                      DOC(fiction_post_layout_optimization_stats_num_crossings_before))
        .def_readonly("num_crossings_after", &fiction::post_layout_optimization_stats::num_crossings_after,
                      DOC(fiction_post_layout_optimization_stats_num_crossings_after))

        ;

    m.def("post_layout_optimization", &fiction::post_layout_optimization<py_cartesian_gate_layout>, py::arg("layout"),
          py::arg("parameters") = fiction::post_layout_optimization_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_post_layout_optimization));
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
