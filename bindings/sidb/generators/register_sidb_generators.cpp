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

void random_layout_generator(nb::module_& m);
void design_gates(nb::module_& m);
void on_the_fly_circuit_design(nb::module_& m);

void register_sidb_generators(nb::module_& m)
{
    random_layout_generator(m);
    design_gates(m);
    on_the_fly_circuit_design(m);
}

}  // namespace pyfiction
