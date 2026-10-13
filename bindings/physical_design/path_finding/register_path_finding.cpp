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
 * @brief Registers the bindings of the `mnt.pyfiction.physical_design.path_finding` submodule.
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
 * @brief Binds distance helpers.
 * @param m Python module.
 */
void register_distance(nb::module_& m);
/**
 * @brief Binds a star helpers.
 * @param m Python module.
 */
void register_a_star(nb::module_& m);
/**
 * @brief Binds k shortest paths helpers.
 * @param m Python module.
 */
void register_k_shortest_paths(nb::module_& m);
/**
 * @brief Binds enumerate all paths helpers.
 * @param m Python module.
 */
void register_enumerate_all_paths(nb::module_& m);

void register_path_finding(nb::module_& m)
{
    register_distance(m);
    register_a_star(m);
    register_k_shortest_paths(m);
    register_enumerate_all_paths(m);
}

}  // namespace pyfiction
