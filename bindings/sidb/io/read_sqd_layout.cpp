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

#include <fiction/technology/sidb/io/read_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds read sqd layout helpers.
 * @param m Python module.
 */
void register_read_sqd_layout(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::sidb::io::sqd_parsing_error>(
        m, "sqd_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    m.def("read_sqd_layout",
          nb::overload_cast<const std::string_view&, const std::string_view&>(&fiction::sidb::io::read_sqd_layout),
          nb::arg("filename"), nb::arg("layout_name") = "", DOC(fiction_sidb_io_read_sqd_layout_2),
          nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace pyfiction
