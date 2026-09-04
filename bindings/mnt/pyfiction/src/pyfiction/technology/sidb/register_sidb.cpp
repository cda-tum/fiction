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
 * @brief Registry for the bindings of `fiction/technology/sidb/`.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers SiDB lattice sites, lattice geometry, and row conversions.
 *
 * @param m Python module.
 */
void lattice(nanobind::module_& m);
/**
 * @brief Registers SiDB layouts and their cell and defect operations.
 *
 * @param m Python module.
 */
void layout(nanobind::module_& m);

/**
 * @brief Registers SiDB lattice types before the layouts that use them.
 *
 * @param m Python module.
 */
void register_sidb(nanobind::module_& m)
{
    lattice(m);
    layout(m);
}

}  // namespace pyfiction
