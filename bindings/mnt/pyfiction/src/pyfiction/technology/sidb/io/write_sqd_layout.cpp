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
 * @author OpenAI (Codex)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "write_sqd_layout", [](const fiction::sidb::layout& lyt, const std::string& filename)
        { fiction::sidb::io::write_sqd_layout(lyt, filename); }, py::arg("layout"), py::arg("filename"),
        DOC(fiction_sidb_io_write_sqd_layout_2));
}

}  // namespace pyfiction
