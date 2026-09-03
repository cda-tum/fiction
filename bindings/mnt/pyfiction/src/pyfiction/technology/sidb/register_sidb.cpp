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

void lattice(nanobind::module_& m);
void layout(nanobind::module_& m);

void register_sidb(nanobind::module_& m)
{
    lattice(m);
    layout(m);
}

}  // namespace pyfiction
