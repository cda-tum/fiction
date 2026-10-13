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

/**
 * @brief Binds `engine`.
 * @param m Python module.
 */
void bind_engine(nb::module_& m);
/**
 * @brief Binds `potential_landscape`.
 * @param m Python module.
 */
void bind_potential_landscape(nb::module_& m);
/**
 * @brief Binds `result`.
 * @param m Python module.
 */
void bind_result(nb::module_& m);
/**
 * @brief Binds `is_ground_state`.
 * @param m Python module.
 */
void bind_is_ground_state(nb::module_& m);
/**
 * @brief Binds `check_simulation_results_for_equivalence`.
 * @param m Python module.
 */
void bind_check_simulation_results_for_equivalence(nb::module_& m);

void register_sidb_simulation(nb::module_& m)
{
    bind_engine(m);
    bind_potential_landscape(m);
    bind_result(m);
    bind_is_ground_state(m);
    bind_check_simulation_results_for_equivalence(m);
}

}  // namespace pyfiction
