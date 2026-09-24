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
 * @brief Registers the `physical_design` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void exact(nanobind::module_& m);
void orthogonal(nanobind::module_& m);
void graph_oriented_layout_design(nanobind::module_& m);
void apply_gate_library(nanobind::module_& m);
void color_routing(nanobind::module_& m);
void hexagonalization(nanobind::module_& m);
void post_layout_optimization(nanobind::module_& m);
void wiring_reduction(nanobind::module_& m);
void routing_utils(nanobind::module_& m);
void placement_utils(nanobind::module_& m);

void register_physical_design(nanobind::module_& m)
{
    exact(m);
    orthogonal(m);
    graph_oriented_layout_design(m);
    apply_gate_library(m);
    color_routing(m);
    hexagonalization(m);
    post_layout_optimization(m);
    wiring_reduction(m);
    routing_utils(m);
    placement_utils(m);
}

}  // namespace pyfiction
