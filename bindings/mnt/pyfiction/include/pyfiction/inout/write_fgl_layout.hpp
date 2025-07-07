//
// Created by simon on 09.10.22.
//

#ifndef PYFICTION_WRITE_FGL_LAYOUT_HPP
#define PYFICTION_WRITE_FGL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_fgl_layout.hpp>
#include <fiction/traits.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_fgl_layout(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "write_fgl_layout",
        [](const Lyt& lyt, const std::string_view& filename) { fiction::write_fgl_layout<Lyt>(lyt, filename); },
        py::arg("layout"), py::arg("filename"), DOC(fiction_write_fgl_layout_2));
}

}  // namespace detail

inline void write_fgl_layout(pybind11::module& m)
{
    detail::write_fgl_layout<py_cartesian_gate_layout<py_offset_coordinate>>(m);
    detail::write_fgl_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_fgl_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_FGL_LAYOUT_HPP
