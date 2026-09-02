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

void displacement_robustness_domain(nanobind::module_& m);

void register_sidb_simulation_defects(nanobind::module_& m)
{
    displacement_robustness_domain(m);
}

}  // namespace pyfiction
