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
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void lattice(nanobind::module_& m);
void charge_distribution_surface(nanobind::module_& m);

void register_sidb_surfaces(nanobind::module_& m)
{
    lattice(m);
    charge_distribution_surface(m);
}

}  // namespace pyfiction
