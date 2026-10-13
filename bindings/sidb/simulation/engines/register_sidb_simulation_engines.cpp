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

void quickexact(nb::module_& m);
void quicksim(nb::module_& m);
void exhaustive_ground_state_simulation(nb::module_& m);
void clustercomplete(nb::module_& m);

void register_sidb_simulation_engines(nb::module_& m)
{
    quickexact(m);
    quicksim(m);
    exhaustive_ground_state_simulation(m);
    clustercomplete(m);
}

}  // namespace pyfiction
