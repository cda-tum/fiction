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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.io` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers SQD export.
 *
 * @param m Python module.
 */
void bind_write_sqd_layout(nb::module_& m);
/**
 * @brief Registers SQD import and parsing errors.
 *
 * @param m Python module.
 */
void bind_read_sqd_layout(nb::module_& m);
/**
 * @brief Registers surface-defect import.
 *
 * @param m Python module.
 */
void bind_read_surface_defects(nb::module_& m);
/**
 * @brief Registers SVG export and its rendering parameters.
 *
 * @param m Python module.
 */
void bind_write_sidb_layout_svg(nb::module_& m);
/**
 * @brief Registers the textual layout printer.
 *
 * @param m Python module.
 */
void bind_print_sidb_layout(nb::module_& m);

/**
 * @brief Registers SiDB layout import and export.
 *
 * @param m Python module.
 */
void register_sidb_io(nb::module_& m)
{
    bind_write_sqd_layout(m);
    bind_read_sqd_layout(m);
    bind_read_surface_defects(m);
    bind_write_sidb_layout_svg(m);
    bind_print_sidb_layout(m);
}

}  // namespace pyfiction
