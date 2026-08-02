//
// Created by simon on 09.10.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_fgl_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void read_fgl_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    py::exception<fiction::fgl_parsing_error>(
        m, "fgl_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h

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
