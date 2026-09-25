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

namespace pyfiction
{

void logic_networks(nanobind::module_& m);
void name_utils(nanobind::module_& m);
void network_utils(nanobind::module_& m);
void logic_simulation(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.networks.io` submodule.
 *
 * @param m Python submodule.
 */
void register_networks_io(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(networks, m)
{
    m.doc() = "Logic networks and their simulation.";

    // Registers the types this module names in signatures and default arguments.
    nanobind::module_::import_("mnt.pyfiction.layouts");

    // Overloads on the cell-level layouts of `qca`, `mol_qca`, `inml`, and `sidb` import nothing: those
    // modules come later in the import chain, and an argument of their type implies that they are loaded.

    pyfiction::logic_networks(m);
    pyfiction::name_utils(m);
    pyfiction::network_utils(m);
    pyfiction::logic_simulation(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers, writers, and drawers of logic networks.");
    pyfiction::register_networks_io(io);
}

#pragma GCC diagnostic pop
