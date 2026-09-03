/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/technology/sidb/io/read_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

void read_sqd_layout_100(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    py_sidb_100_lattice (*const read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::sidb::io::read_sqd_layout<py_sidb_100_lattice>;

    m.def("read_sqd_layout_100", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_sidb_io_read_sqd_layout_3));
}

void read_sqd_layout_111(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    py_sidb_111_lattice (*const read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::sidb::io::read_sqd_layout<py_sidb_111_lattice>;

    m.def("read_sqd_layout_111", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_sidb_io_read_sqd_layout_3));
}

}  // namespace detail

void read_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::sidb::io::sqd_parsing_error>(
        m, "sqd_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    detail::read_sqd_layout_100(m);
    detail::read_sqd_layout_111(m);

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::layout (*const read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::sidb::io::read_sqd_layout;

    m.def("read_sqd_layout", read_sqd_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_sidb_io_read_sqd_layout_6));
}

}  // namespace pyfiction
