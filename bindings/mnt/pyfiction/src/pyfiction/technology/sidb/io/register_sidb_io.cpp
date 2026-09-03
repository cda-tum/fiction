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
 * @brief Registers the `technology/sidb/io` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_sqd_layout(nanobind::module_& m);
void read_sqd_layout(nanobind::module_& m);
void write_sidb_layout_svg(nanobind::module_& m);

void register_sidb_io(nanobind::module_& m)
{
    write_sqd_layout(m);
    read_sqd_layout(m);
    write_sidb_layout_svg(m);
}

}  // namespace pyfiction
