//
// Created by marcel on 25.04.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_sqd_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

void read_sqd_layout_100(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    py_sidb_100_lattice (*const read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_sqd_layout<py_sidb_100_lattice>;

    m.def("read_sqd_layout_100", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_sqd_layout_3));
}

void read_sqd_layout_111(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    py_sidb_111_lattice (*const read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_sqd_layout<py_sidb_111_lattice>;

    m.def("read_sqd_layout_111", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_sqd_layout_3));
}

}  // namespace detail

void read_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    py::exception<fiction::sqd_parsing_error>(
        m, "sqd_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h

    detail::read_sqd_layout_100(m);
    detail::read_sqd_layout_111(m);
}

}  // namespace pyfiction
