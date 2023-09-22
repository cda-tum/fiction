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

    m.def(
        "post_layout_optimization",
        [](const Lyt& lyt)
        {
            fiction::post_layout_optimization<Lyt>(lyt);
        },
        "layout"_a, DOC(fiction_post_layout_optimization));
}

}  // namespace detail

inline void post_layout_optimization(pybind11::module& m)
{
    detail::post_layout_optimization<py_cartesian_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_POST_LAYOUT_OPTIMIZATION_HPP