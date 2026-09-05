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
 * @brief Python bindings for `fiction/technology/sidb/io/read_surface_defects.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/io/read_surface_defects.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

/**
 * @brief Registers surface-defect import.
 *
 * @param m Python module.
 */
void read_surface_defects(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translators with the module; they are not meant to be thrown here
    py::exception<fiction::sidb::io::unsupported_defect_index_exception>(
        m, "unsupported_defect_index_exception",
        PyExc_ValueError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    py::exception<fiction::sidb::io::missing_position_exception>(
        m, "missing_position_exception",
        PyExc_ValueError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::layout (*const read_surface_defects_function_pointer)(
        const std::string_view&, const std::string_view&) = &fiction::sidb::io::read_surface_defects;

    m.def("read_surface_defects", read_surface_defects_function_pointer, py::arg("filename"),
          py::arg("layout_name") = "", DOC(fiction_sidb_io_read_surface_defects_4));
}

}  // namespace pyfiction
