//
// Created by marcel on 19.09.23.
//

#ifndef PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
#define PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>

#include <sstream>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

/**
 * Post-layout optimization for Cartesian 2DDWave-clocked gate-level layouts.
 */
inline void post_layout_optimization(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::post_layout_optimization_stats>(m, "post_layout_optimization_stats")
        //, DOC(fiction_post_layout_optimization_stats))

        .def(py::init<>())
        .def_readonly("time_total", &fiction::post_layout_optimization_stats::time_total)
        // , DOC(fiction_post_layout_optimization_stats_time_total))
        .def("__repr__",
             [](const fiction::post_layout_optimization_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("post_layout_optimization", &fiction::post_layout_optimization<py_cartesian_gate_layout>, "lyt"_a,
          "statistics"_a = nullptr);
    //, DOC(fiction_post_layout_optimization));
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
