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
 * @brief Registry of the logic network reader and writer bindings.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void network_reader(nanobind::module_& m);
void network_writers(nanobind::module_& m);
void dot_drawers(nanobind::module_& m);

void register_networks_io(nanobind::module_& m)
{
    network_reader(m);
    network_writers(m);
    dot_drawers(m);
}

}  // namespace pyfiction
