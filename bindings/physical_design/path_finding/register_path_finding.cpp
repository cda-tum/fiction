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

void distance(nb::module_& m);
void a_star(nb::module_& m);
void k_shortest_paths(nb::module_& m);
void enumerate_all_paths(nb::module_& m);

void register_path_finding(nb::module_& m)
{
    distance(m);
    a_star(m);
    k_shortest_paths(m);
    enumerate_all_paths(m);
}

}  // namespace pyfiction
