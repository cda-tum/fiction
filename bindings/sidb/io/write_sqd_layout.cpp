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
 * @brief Python bindings for `fiction/technology/sidb/io/write_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/utils/progress.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds write sqd layout helpers.
 * @param m Python module.
 */
void bind_write_sqd_layout(nb::module_& m)
{
    m.def(
        "write_sqd_layout",
        [](const fiction::sidb::layout& lyt, const std::string& filename,
           const fiction::utils::progress_callback& on_progress)
        { fiction::sidb::io::write_sqd_layout(lyt, filename, on_progress); },
        nb::arg("layout"), nb::arg("filename"), nb::arg("on_progress").none() = nb::none(),
        DOC(fiction_sidb_io_write_sqd_layout_2), nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace pyfiction
