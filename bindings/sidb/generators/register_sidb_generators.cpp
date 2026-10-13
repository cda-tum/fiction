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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.generators` submodule.
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
 * @brief Binds random layout generator helpers.
 * @param m Python module.
 */
void bind_random_layout_generator(nb::module_& m);
/**
 * @brief Binds design gates helpers.
 * @param m Python module.
 */
void bind_design_gates(nb::module_& m);
/**
 * @brief Binds on the fly circuit design helpers.
 * @param m Python module.
 */
void bind_on_the_fly_circuit_design(nb::module_& m);

void register_sidb_generators(nb::module_& m)
{
    bind_random_layout_generator(m);
    bind_design_gates(m);
    bind_on_the_fly_circuit_design(m);
}

}  // namespace pyfiction
