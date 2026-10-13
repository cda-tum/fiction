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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds `network_reader`.
 * @param m Python module.
 */
void bind_network_reader(nb::module_& m);
/**
 * @brief Binds `network_writers`.
 * @param m Python module.
 */
void bind_network_writers(nb::module_& m);
/**
 * @brief Binds `dot_drawers`.
 * @param m Python module.
 */
void bind_dot_drawers(nb::module_& m);

void register_networks_io(nb::module_& m)
{
    bind_network_reader(m);
    bind_network_writers(m);
    bind_dot_drawers(m);
}

}  // namespace pyfiction
