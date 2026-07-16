//
// Created by marcel on 25.04.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_sqd_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

void read_sqd_layout_100(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py_sidb_100_lattice (*const read_sqd_layout_function_pointer)(  // NOLINT(misc-const-correctness)
        const std::string_view&, const std::string_view&) = &fiction::read_sqd_layout<py_sidb_100_lattice>;

    m.def("read_sqd_layout_100", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_sqd_layout_3));
}

void read_sqd_layout_111(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py_sidb_111_lattice (*const read_sqd_layout_function_pointer)(  // NOLINT(misc-const-correctness)
        const std::string_view&, const std::string_view&) = &fiction::read_sqd_layout<py_sidb_111_lattice>;

    m.def("read_sqd_layout_111", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_sqd_layout_3));
}

}  // namespace detail

void read_sqd_layout(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py::register_exception<fiction::sqd_parsing_error>(
        m, "sqd_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through pybind11.h

    detail::read_sqd_layout_100(m);
    detail::read_sqd_layout_111(m);
}

}  // namespace pyfiction
