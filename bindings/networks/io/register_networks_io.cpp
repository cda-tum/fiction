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
 * @brief Binds network reader helpers.
 * @param m Python module.
 */
void register_network_reader(nb::module_& m);
/**
 * @brief Binds network writers helpers.
 * @param m Python module.
 */
void register_network_writers(nb::module_& m);
/**
 * @brief Binds dot drawers helpers.
 * @param m Python module.
 */
void register_dot_drawers(nb::module_& m);

void register_networks_io(nb::module_& m)
{
    register_network_reader(m);
    register_network_writers(m);
    register_dot_drawers(m);
}

}  // namespace pyfiction
