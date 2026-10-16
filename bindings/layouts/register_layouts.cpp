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
 * @brief Binds coordinate helpers.
 * @param m Python module.
 */
void register_coordinate(nb::module_& m);
/**
 * @brief Binds coordinate utility helpers.
 * @param m Python module.
 */
void register_coordinate_utility(nb::module_& m);
/**
 * @brief Binds arrangement helpers.
 * @param m Python module.
 */
void register_arrangement(nb::module_& m);
/**
 * @brief Binds cartesian layout helpers.
 * @param m Python module.
 */
void register_cartesian_layout(nb::module_& m);
/**
 * @brief Binds shifted cartesian layout helpers.
 * @param m Python module.
 */
void register_shifted_cartesian_layout(nb::module_& m);
/**
 * @brief Binds hexagonal layout helpers.
 * @param m Python module.
 */
void register_hexagonal_layout(nb::module_& m);
/**
 * @brief Binds gate level layout helpers.
 * @param m Python module.
 */
void register_gate_layout(nb::module_& m);
/**
 * @brief Binds obstructions helpers.
 * @param m Python module.
 */
void register_obstructions(nb::module_& m);
/**
 * @brief Binds layout utils helpers.
 * @param m Python module.
 */
void register_layout_utils(nb::module_& m);

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

    pyfiction::register_coordinate(m);
    pyfiction::register_coordinate_utility(m);
    pyfiction::register_arrangement(m);
    pyfiction::register_cartesian_layout(m);
    pyfiction::register_shifted_cartesian_layout(m);
    pyfiction::register_hexagonal_layout(m);
    pyfiction::register_gate_layout(m);
    pyfiction::register_obstructions(m);
    pyfiction::register_layout_utils(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers, writers, and drawers of gate-level layouts.");
    pyfiction::register_layouts_io(io);
}

#pragma GCC diagnostic pop
