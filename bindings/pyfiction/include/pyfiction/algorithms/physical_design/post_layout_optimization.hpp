//
// Created by simon on 22.09.23.
//

#ifndef PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP
#define PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/types.hpp>
#include <fiction/algorithms/physical_design/post_layout_optimization.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void post_layout_optimization(pybind11::module& m)
{
    using namespace pybind11::literals;

    pybind11::class_<fiction::post_layout_optimization_stats>(m, "post_layout_optimization_stats",
                                                          DOC(fiction_post_layout_optimization_stats))
        .def(pybind11::init<>())
        .def("__repr__",
             [](const fiction::post_layout_optimization_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def(
        "post_layout_optimization",
        &fiction::post_layout_optimization<Lyt>,
        "layout"_a, "statistics"_a = nullptr, DOC(fiction_post_layout_optimization));
}

}  // namespace detail

inline void post_layout_optimization(pybind11::module& m)
{
    detail::post_layout_optimization<py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP