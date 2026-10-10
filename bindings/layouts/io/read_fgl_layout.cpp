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
 * @brief Python bindings for `fiction/layouts/io/read_fgl_layout.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/read_fgl_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts filename and layout name arguments.

namespace pyfiction
{

/** @brief Register FGL readers and their parsing exception. @param m Layout I/O module. */
void read_fgl_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::layouts::io::fgl_parsing_error>(
        m, "fgl_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    m.def("read_cartesian_fgl_layout",
          py::overload_cast<const std::string_view&, const std::string_view&>(
              &fiction::layouts::io::read_fgl_layout<py_cartesian_gate_layout>),
          py::arg("filename"), py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3),
          py::call_guard<py::gil_scoped_release>());
    m.def("read_shifted_cartesian_fgl_layout",
          py::overload_cast<const std::string_view&, const std::string_view&>(
              &fiction::layouts::io::read_fgl_layout<py_shifted_cartesian_gate_layout>),
          py::arg("filename"), py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3),
          py::call_guard<py::gil_scoped_release>());
    m.def("read_hexagonal_fgl_layout",
          py::overload_cast<const std::string_view&, const std::string_view&>(
              &fiction::layouts::io::read_fgl_layout<py_hexagonal_gate_layout>),
          py::arg("filename"), py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3),
          py::call_guard<py::gil_scoped_release>());
}

}  // namespace pyfiction
