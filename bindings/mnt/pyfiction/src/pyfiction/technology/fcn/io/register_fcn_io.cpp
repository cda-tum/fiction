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
 * @brief Registers the `technology/fcn/io` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void write_qll_layout(nanobind::module_& m);

void register_fcn_io(nanobind::module_& m)
{
    write_qll_layout(m);
}

}  // namespace pyfiction
