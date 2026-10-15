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
 * @brief Entry point of the `mnt.pyfiction.networks` extension module.
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
 * @brief Binds logic networks helpers.
 * @param m Python module.
 */
void register_logic_networks(nb::module_& m);
/**
 * @brief Binds name utils helpers.
 * @param m Python module.
 */
void register_name_utils(nb::module_& m);
/**
 * @brief Binds network utils helpers.
 * @param m Python module.
 */
void register_network_utils(nb::module_& m);
/**
 * @brief Binds logic simulation helpers.
 * @param m Python module.
 */
void register_logic_simulation(nb::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.networks.io` submodule.
 *
 * @param m Python submodule.
 */
void register_networks_io(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(networks, m)
{
    m.doc() = "Logic networks and their simulation.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.layouts");

    // Overloads on the cell-level layouts of `qca`, `mol_qca`, `inml`, and `sidb` import nothing: those
    // modules come later in the import chain, and an argument of their type implies that they are loaded.

    pyfiction::register_logic_networks(m);
    pyfiction::register_name_utils(m);
    pyfiction::register_network_utils(m);
    pyfiction::register_logic_simulation(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers, writers, and drawers of logic networks.");
    pyfiction::register_networks_io(io);
}

#pragma GCC diagnostic pop
