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
 * @brief Registers the bindings of the `mnt.pyfiction.mol_qca.io` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds write mol qca layout svg helpers.
 * @param m Python module.
 */
void bind_write_mol_qca_layout_svg(nb::module_& m);

/**
 * @brief Registers the molQCA writers.
 *
 * @param m Python module.
 */
void register_mol_qca_io(nb::module_& m)
{
    bind_write_mol_qca_layout_svg(m);
}

}  // namespace pyfiction
