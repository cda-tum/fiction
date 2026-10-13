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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.engines` submodule.
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
 * @brief Binds quickexact helpers.
 * @param m Python module.
 */
void register_quickexact(nb::module_& m);
/**
 * @brief Binds quicksim helpers.
 * @param m Python module.
 */
void register_quicksim(nb::module_& m);
/**
 * @brief Binds exhaustive ground state simulation helpers.
 * @param m Python module.
 */
void register_exhaustive_ground_state_simulation(nb::module_& m);
/**
 * @brief Binds clustercomplete helpers.
 * @param m Python module.
 */
void register_clustercomplete(nb::module_& m);

void register_sidb_simulation_engines(nb::module_& m)
{
    register_quickexact(m);
    register_quicksim(m);
    register_exhaustive_ground_state_simulation(m);
    register_clustercomplete(m);
}

}  // namespace pyfiction
