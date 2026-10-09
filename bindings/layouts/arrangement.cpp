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
 * @brief Python bindings for `fiction/layouts/arrangement.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/layouts/arrangement.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers the arrangement of shifted rows or columns.
 *
 * @param m Python module.
 */
void arrangement(nanobind::module_& m)
{
    namespace py = nanobind;

    using fiction::layouts::arrangement;

    py::enum_<arrangement>(m, "arrangement", DOC(fiction_layouts_arrangement))
        .value("ODD_ROW", arrangement::ODD_ROW, DOC(fiction_layouts_arrangement_ODD_ROW))
        .value("EVEN_ROW", arrangement::EVEN_ROW, DOC(fiction_layouts_arrangement_EVEN_ROW))
        .value("ODD_COLUMN", arrangement::ODD_COLUMN, DOC(fiction_layouts_arrangement_ODD_COLUMN))
        .value("EVEN_COLUMN", arrangement::EVEN_COLUMN, DOC(fiction_layouts_arrangement_EVEN_COLUMN));
}

}  // namespace pyfiction
