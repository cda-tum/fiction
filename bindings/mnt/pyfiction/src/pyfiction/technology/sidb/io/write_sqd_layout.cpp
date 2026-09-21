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

namespace pyfiction
{

void write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "write_sqd_layout",
        [](const fiction::sidb::layout& lyt, const std::string& filename,
           const fiction::utils::progress_callback& on_progress)
        { fiction::sidb::io::write_sqd_layout(lyt, filename, on_progress); },
        py::arg("layout"), py::arg("filename"), py::arg("on_progress").none() = py::none(),
        DOC(fiction_sidb_io_write_sqd_layout_2), py::call_guard<py::gil_scoped_release>());
}

}  // namespace pyfiction
