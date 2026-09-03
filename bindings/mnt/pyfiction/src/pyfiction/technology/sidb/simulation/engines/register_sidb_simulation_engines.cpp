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
 * @brief Registers the `technology/sidb/simulation/engines` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void quickexact(nanobind::module_& m);
void quicksim(nanobind::module_& m);
void exhaustive_ground_state_simulation(nanobind::module_& m);
void clustercomplete(nanobind::module_& m);

void register_sidb_simulation_engines(nanobind::module_& m)
{
    quickexact(m);
    quicksim(m);
    exhaustive_ground_state_simulation(m);
    clustercomplete(m);
}

}  // namespace pyfiction
