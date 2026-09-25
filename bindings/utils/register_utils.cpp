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
 * @brief Entry point of the `mnt.pyfiction.utils` extension module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void version_info(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(utils, m)
{
    m.doc() = "Version information and shared utilities.";

    pyfiction::version_info(m);
}

#pragma GCC diagnostic pop
