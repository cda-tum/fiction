//
// Created by simon on 09.10.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_fgl_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

void read_fgl_layout(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py::register_exception<fiction::fgl_parsing_error>(
        m, "fgl_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through pybind11.h

    // NOLINTNEXTLINE(misc-const-correctness)
    py_cartesian_gate_layout (*const read_cartesian_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_cartesian_gate_layout>;
    // NOLINTNEXTLINE(misc-const-correctness)
    py_shifted_cartesian_gate_layout (*const read_shifted_cartesian_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_shifted_cartesian_gate_layout>;
    // NOLINTNEXTLINE(misc-const-correctness)
    py_hexagonal_gate_layout (*const read_hexagonal_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::read_fgl_layout<py_hexagonal_gate_layout>;

    m.def("read_cartesian_fgl_layout", read_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
    m.def("read_shifted_cartesian_fgl_layout", read_shifted_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
    m.def("read_hexagonal_fgl_layout", read_hexagonal_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_read_fgl_layout_3));
}

}  // namespace pyfiction
