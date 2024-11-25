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

inline void read_fgl_layout(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::fgl_parsing_error>(m, "fgl_parsing_error", PyExc_RuntimeError);

    py_cartesian_gate_layout (*read_cartesian_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_cartesian_gate_layout>;
    py_shifted_cartesian_gate_layout (*read_shifted_cartesian_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_shifted_cartesian_gate_layout>;
    py_hexagonal_gate_layout (*read_hexagonal_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_hexagonal_gate_layout>;

    m.def("read_cartesian_fgl_layout", read_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
    m.def("read_shifted_cartesian_fgl_layout", read_shifted_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
    m.def("read_hexagonal_fgl_layout", read_hexagonal_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_FGL_LAYOUT_HPP
