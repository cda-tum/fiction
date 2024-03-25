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

namespace detail
{

template <typename Lyt>
void post_layout_optimization(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("post_layout_optimization", &fiction::post_layout_optimization<Lyt>, "lyt"_a, "pst"_a = nullptr,
          DOC(fiction_post_layout_optimization));
}

}  // namespace detail

inline void post_layout_optimization(pybind11::module& m)
{
    pybind11::class_<fiction::post_layout_optimization_stats>(m, "post_layout_optimization_stats",
                                                              DOC(fiction_post_layout_optimization_stats))
        .def(pybind11::init<>())
        .def_readonly("time_total", &fiction::post_layout_optimization_stats::time_total)
        .def("__repr__",
             [](const fiction::post_layout_optimization_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    detail::post_layout_optimization<py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
