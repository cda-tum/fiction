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
 * @brief Registers the bindings of the `mnt.pyfiction.qca.io` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void write_qca_layout(nb::module_& m);
void write_qca_layout_svg(nb::module_& m);

void register_qca_io(nb::module_& m)
{
    write_qca_layout(m);
    write_qca_layout_svg(m);
}

}  // namespace pyfiction
