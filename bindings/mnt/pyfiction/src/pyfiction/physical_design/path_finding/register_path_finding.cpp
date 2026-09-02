/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void distance(nanobind::module_& m);
void a_star(nanobind::module_& m);
void k_shortest_paths(nanobind::module_& m);
void enumerate_all_paths(nanobind::module_& m);

void register_path_finding(nanobind::module_& m)
{
    distance(m);
    a_star(m);
    k_shortest_paths(m);
    enumerate_all_paths(m);
}

}  // namespace pyfiction
