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

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::layouts::io::fgl_parsing_error>(
        m, "fgl_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    // NOLINTNEXTLINE(misc-const-correctness)
    py_cartesian_gate_layout (*const read_cartesian_fgl_layout_function_pointer)(const std::string_view&,
                                                                                 const std::string_view&) =
        &fiction::layouts::io::read_fgl_layout<py_cartesian_gate_layout>;
    // NOLINTNEXTLINE(misc-const-correctness)
    py_shifted_cartesian_gate_layout (*const read_shifted_cartesian_fgl_layout_function_pointer)(
        const std::string_view&, const std::string_view&) =
        &fiction::layouts::io::read_fgl_layout<py_shifted_cartesian_gate_layout>;
    // NOLINTNEXTLINE(misc-const-correctness)
    py_hexagonal_gate_layout (*const read_hexagonal_fgl_layout_function_pointer)(const std::string_view&,
                                                                                 const std::string_view&) =
        &fiction::layouts::io::read_fgl_layout<py_hexagonal_gate_layout>;

    m.def("read_cartesian_fgl_layout", read_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3));
    m.def("read_shifted_cartesian_fgl_layout", read_shifted_cartesian_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3));
    m.def("read_hexagonal_fgl_layout", read_hexagonal_fgl_layout_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_layouts_io_read_fgl_layout_3));
}

}  // namespace pyfiction
