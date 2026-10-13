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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.model` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void defect(nb::module_& m);
void charge_state(nb::module_& m);
void simulation_parameters(nb::module_& m);
void potential_to_distance_conversion(nb::module_& m);

void register_sidb_model(nb::module_& m)
{
    defect(m);
    charge_state(m);
    simulation_parameters(m);
    potential_to_distance_conversion(m);
}

}  // namespace pyfiction
