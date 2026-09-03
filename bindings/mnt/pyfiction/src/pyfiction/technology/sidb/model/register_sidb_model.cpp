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
 * @brief Registers the `technology/sidb/model` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void defect(nanobind::module_& m);
void charge_state(nanobind::module_& m);
void nm_position(nanobind::module_& m);
void nm_distance(nanobind::module_& m);
void simulation_parameters(nanobind::module_& m);
void potential_to_distance_conversion(nanobind::module_& m);

void register_sidb_model(nanobind::module_& m)
{
    defect(m);
    charge_state(m);
    nm_position(m);
    nm_distance(m);
    simulation_parameters(m);
    potential_to_distance_conversion(m);
}

}  // namespace pyfiction
