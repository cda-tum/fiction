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
 * @brief Registers the `technology/qca/io` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_qca_layout(nanobind::module_& m);
void write_fqca_layout(nanobind::module_& m);
void write_qca_layout_svg(nanobind::module_& m);
void read_fqca_layout(nanobind::module_& m);

void register_qca_io(nanobind::module_& m)
{
    write_qca_layout(m);
    write_fqca_layout(m);
    write_qca_layout_svg(m);
    read_fqca_layout(m);
}

}  // namespace pyfiction
