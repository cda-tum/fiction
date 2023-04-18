//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_WRITE_DOT_LAYOUT_HPP
#define PYFICTION_WRITE_DOT_LAYOUT_HPP

#include "pyfiction/docs.h"
#include "pyfiction/types.hpp"

#include <fiction/io/dot_drawers.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_dot_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string& filename)
        {
            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_cartesian_drawer<Lyt>>(lyt, filename);
            }
            else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_hexagonal_drawer<Lyt>>(lyt, filename);
            }
        },
        "layout"_a, "filename"_a, DOC(fiction_write_dot_layout));
}

}  // namespace detail

inline void write_dot_layout(pybind11::module& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_DOT_LAYOUT_HPP
