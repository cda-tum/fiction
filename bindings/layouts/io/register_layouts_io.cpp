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
 * @brief Registers the bindings of the `mnt.pyfiction.layouts.io` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void layout_drawers(nb::module_& m);
void write_fgl_layout(nb::module_& m);
void read_fgl_layout(nb::module_& m);

void register_layouts_io(nb::module_& m)
{
    layout_drawers(m);
    write_fgl_layout(m);
    read_fgl_layout(m);
}

}  // namespace pyfiction
