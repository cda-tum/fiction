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
 * @brief Entry point of the `mnt.pyfiction.fcn` extension module.
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
 * @brief Binds area helpers.
 * @param m Python module.
 */
void bind_area(nb::module_& m);

/**
 * @brief Registers the bindings of the `mnt.pyfiction.fcn.io` submodule.
 *
 * @param m Python submodule.
 */
void register_fcn_io(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(fcn, m)
{
    m.doc() = "Technology-independent FCN layout properties.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.qca");
    nb::module_::import_("mnt.pyfiction.mol_qca");
    nb::module_::import_("mnt.pyfiction.inml");
    nb::module_::import_("mnt.pyfiction.sidb");

    pyfiction::bind_area(m);
    auto io = pyfiction::def_submodule(m, "io", "Technology-independent readers and writers of cell-level layouts.");
    pyfiction::register_fcn_io(io);
}

#pragma GCC diagnostic pop
