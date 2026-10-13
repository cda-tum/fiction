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
 */

#include "pyfiction/submodule.hpp"

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `coordinate`.
 * @param m Python module.
 */
void bind_coordinate(nb::module_& m);
/**
 * @brief Binds `coordinate_utility`.
 * @param m Python module.
 */
void bind_coordinate_utility(nb::module_& m);
/**
 * @brief Binds `arrangement`.
 * @param m Python module.
 */
void bind_arrangement(nb::module_& m);
/**
 * @brief Binds `cartesian_layout`.
 * @param m Python module.
 */
void bind_cartesian_layout(nb::module_& m);
/**
 * @brief Binds `shifted_cartesian_layout`.
 * @param m Python module.
 */
void bind_shifted_cartesian_layout(nb::module_& m);
/**
 * @brief Binds `hexagonal_layout`.
 * @param m Python module.
 */
void bind_hexagonal_layout(nb::module_& m);
/**
 * @brief Binds `gate_level_layout`.
 * @param m Python module.
 */
void bind_gate_level_layout(nb::module_& m);
/**
 * @brief Binds `obstructions`.
 * @param m Python module.
 */
void bind_obstructions(nb::module_& m);
/**
 * @brief Binds `layout_utils`.
 * @param m Python module.
 */
void bind_layout_utils(nb::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.layouts.io` submodule.
 *
 * @param m Python submodule.
 */
void register_layouts_io(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(layouts, m)
{
    m.doc() = "Coordinates, layout topologies, clocking, and gate-level layouts.";

    // Overloads on the cell-level layouts of `qca`, `mol_qca`, `inml`, and `sidb` import nothing: those
    // modules come later in the import chain, and an argument of their type implies that they are loaded.

    pyfiction::bind_coordinate(m);
    pyfiction::bind_coordinate_utility(m);
    pyfiction::bind_arrangement(m);
    pyfiction::bind_cartesian_layout(m);
    pyfiction::bind_shifted_cartesian_layout(m);
    pyfiction::bind_hexagonal_layout(m);
    pyfiction::bind_gate_level_layout(m);
    pyfiction::bind_obstructions(m);
    pyfiction::bind_layout_utils(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers, writers, and drawers of gate-level layouts.");
    pyfiction::register_layouts_io(io);
}

#pragma GCC diagnostic pop
