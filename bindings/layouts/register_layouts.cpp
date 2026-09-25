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
 * @brief Entry point of the `mnt.pyfiction.layouts` extension module.
 * @author Marcel Walter (marcelwa)
 * @author OpenAI Codex
 */

#include "pyfiction/submodule.hpp"

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers the bindings of the `mnt.pyfiction.layouts.coords` submodule.
 * @param m Python coordinate submodule.
 */
void register_layouts_coords(nanobind::module_& m);
void cartesian_layout(nanobind::module_& m);
void shifted_cartesian_layout(nanobind::module_& m);
void hexagonal_layout(nanobind::module_& m);
void gate_level_layout(nanobind::module_& m);
void obstructions(nanobind::module_& m);
void layout_utils(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.layouts.io` submodule.
 *
 * @param m Python submodule.
 */
void register_layouts_io(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(layouts, m)
{
    m.doc() = "Coordinates, layout topologies, clocking, and gate-level layouts.";

    // Overloads on the cell-level layouts of `qca`, `mol_qca`, `inml`, and `sidb` import nothing: those
    // modules come later in the import chain, and an argument of their type implies that they are loaded.

    auto coords = pyfiction::def_submodule(m, "coords", "Layout coordinates and their area and volume functions.");
    pyfiction::register_layouts_coords(coords);
    pyfiction::cartesian_layout(m);
    pyfiction::shifted_cartesian_layout(m);
    pyfiction::hexagonal_layout(m);
    pyfiction::gate_level_layout(m);
    pyfiction::obstructions(m);
    pyfiction::layout_utils(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers, writers, and drawers of gate-level layouts.");
    pyfiction::register_layouts_io(io);
}

#pragma GCC diagnostic pop
