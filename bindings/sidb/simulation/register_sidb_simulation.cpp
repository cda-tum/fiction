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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void engine(nb::module_& m);
void potential_landscape(nb::module_& m);
void result(nb::module_& m);
void is_ground_state(nb::module_& m);
void check_simulation_results_for_equivalence(nb::module_& m);

void register_sidb_simulation(nb::module_& m)
{
    engine(m);
    potential_landscape(m);
    result(m);
    is_ground_state(m);
    check_simulation_results_for_equivalence(m);
}

}  // namespace pyfiction
