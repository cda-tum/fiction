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
 * @brief Registers the `technology/sidb/surfaces` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers H-Si(100)-2x1 and H-Si(111)-1x1 cell-level layouts.
 *
 * @param m Python module.
 */
void lattice_surface(nanobind::module_& m);
/**
 * @brief Registers SiDB charge-distribution surfaces.
 *
 * @param m Python module.
 */
void charge_distribution_surface(nanobind::module_& m);

/**
 * @brief Registers SiDB lattice and charge-distribution surfaces.
 *
 * @param m Python module.
 */
void register_sidb_surfaces(nanobind::module_& m)
{
    lattice_surface(m);
    charge_distribution_surface(m);
}

}  // namespace pyfiction
