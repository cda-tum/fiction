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
 * @brief Entry point of the `mnt.pyfiction.mol_qca` extension module.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/submodule.hpp"

#include <nanobind/nanobind.h>

namespace pyfiction
{

void mol_qca_layout(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.mol_qca.io` submodule.
 *
 * @param m Python submodule.
 */
void register_mol_qca_io(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(mol_qca, m)
{
    m.doc() = "Molecular QCA cell-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nanobind::module_::import_("mnt.pyfiction.layouts");

    pyfiction::mol_qca_layout(m);
    auto io = pyfiction::def_submodule(m, "io", "Writers of molecular QCA cell-level layouts.");
    pyfiction::register_mol_qca_io(io);
}

#pragma GCC diagnostic pop
