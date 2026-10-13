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

/**
 * @brief Binds `defect`.
 * @param m Python module.
 */
void bind_defect(nb::module_& m);
/**
 * @brief Binds `charge_state`.
 * @param m Python module.
 */
void bind_charge_state(nb::module_& m);
/**
 * @brief Binds `simulation_parameters`.
 * @param m Python module.
 */
void bind_simulation_parameters(nb::module_& m);
/**
 * @brief Binds `potential_to_distance_conversion`.
 * @param m Python module.
 */
void bind_potential_to_distance_conversion(nb::module_& m);

void register_sidb_model(nb::module_& m)
{
    bind_defect(m);
    bind_charge_state(m);
    bind_simulation_parameters(m);
    bind_potential_to_distance_conversion(m);
}

}  // namespace pyfiction
