//
// Created by simon on 09.10.22.
//

#ifndef PYFICTION_READ_FGL_LAYOUT_HPP
#define PYFICTION_READ_FGL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_fgl_layout.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void read_fgl_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    Lyt (*read_fgl_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_fgl_layout<Lyt>;

    m.def("read_fgl_layout", read_fgl_layout_function_pointer, "filename"_a, "layout_name"_a = "", DOC(fiction_read_fgl_layout_3));
}

}  // namespace detail

inline void read_fgl_layout(pybind11::module& m)
{
    detail::read_fgl_layout<py_cartesian_gate_layout>(m);
    detail::read_fgl_layout<py_shifted_cartesian_gate_layout>(m);
    detail::read_fgl_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_FGL_LAYOUT_HPP
