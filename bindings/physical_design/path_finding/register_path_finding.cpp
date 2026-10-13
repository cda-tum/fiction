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
 * @brief Binds `distance`.
 * @param m Python module.
 */
void bind_distance(nb::module_& m);
/**
 * @brief Binds `a_star`.
 * @param m Python module.
 */
void bind_a_star(nb::module_& m);
/**
 * @brief Binds `k_shortest_paths`.
 * @param m Python module.
 */
void bind_k_shortest_paths(nb::module_& m);
/**
 * @brief Binds `enumerate_all_paths`.
 * @param m Python module.
 */
void bind_enumerate_all_paths(nb::module_& m);

void register_path_finding(nb::module_& m)
{
    bind_distance(m);
    bind_a_star(m);
    bind_k_shortest_paths(m);
    bind_enumerate_all_paths(m);
}

}  // namespace pyfiction
