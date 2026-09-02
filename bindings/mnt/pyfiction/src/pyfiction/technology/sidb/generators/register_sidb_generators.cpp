/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void random_layout_generator(nanobind::module_& m);
void design_gates(nanobind::module_& m);

void register_sidb_generators(nanobind::module_& m)
{
    random_layout_generator(m);
    design_gates(m);
}

}  // namespace pyfiction
