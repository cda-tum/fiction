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

/**
 * @brief Binds layout drawers helpers.
 * @param m Python module.
 */
void bind_layout_drawers(nb::module_& m);
/**
 * @brief Binds write fgl layout helpers.
 * @param m Python module.
 */
void bind_write_fgl_layout(nb::module_& m);
/**
 * @brief Binds read fgl layout helpers.
 * @param m Python module.
 */
void bind_read_fgl_layout(nb::module_& m);

void register_layouts_io(nb::module_& m)
{
    bind_layout_drawers(m);
    bind_write_fgl_layout(m);
    bind_read_fgl_layout(m);
}

}  // namespace pyfiction
