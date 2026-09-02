/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void layout_drawers(nanobind::module_& m);
void write_fgl_layout(nanobind::module_& m);
void read_fgl_layout(nanobind::module_& m);

void register_layouts_io(nanobind::module_& m)
{
    layout_drawers(m);
    write_fgl_layout(m);
    read_fgl_layout(m);
}

}  // namespace pyfiction
