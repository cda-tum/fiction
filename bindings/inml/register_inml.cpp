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
 * @brief Entry point of the `mnt.pyfiction.inml` extension module.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/submodule.hpp"

#include <nanobind/nanobind.h>

namespace pyfiction
{

void inml_layout(nanobind::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.inml.io` submodule.
 *
 * @param m Python submodule.
 */
void register_inml_io(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(inml, m)
{
    m.doc() = "In-plane Nanomagnet Logic (iNML) cell-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nanobind::module_::import_("mnt.pyfiction.layouts");

    pyfiction::inml_layout(m);
    auto io = pyfiction::def_submodule(m, "io", "Readers and writers of iNML cell-level layouts.");
    pyfiction::register_inml_io(io);
}

#pragma GCC diagnostic pop
