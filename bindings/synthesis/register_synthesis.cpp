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
 * @brief Entry point of the `mnt.pyfiction.synthesis` extension module.
 * @author Marcel Walter (marcelwa)
 * @author Benjamin Hien (hibenj)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Register the dynamic truth-table type.
 * @param m Python module.
 */
void register_dynamic_truth_table(nb::module_& m);

/**
 * @brief Binds truth tables helpers.
 * @param m Python module.
 */
void register_truth_tables(nb::module_& m);
/**
 * @brief Binds fanout substitution helpers.
 * @param m Python module.
 */
void register_fanout_substitution(nb::module_& m);
/**
 * @brief Binds network balancing helpers.
 * @param m Python module.
 */
void register_network_balancing(nb::module_& m);
/**
 * @brief Binds technology mapping helpers.
 * @param m Python module.
 */
void register_technology_mapping(nb::module_& m);
/**
 * @brief Binds network conversion helpers.
 * @param m Python module.
 */
void register_network_conversion(nb::module_& m);
/**
 * @brief Binds the planarization pipeline.
 * @param m Python module.
 */
void register_planarization(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(synthesis, m)
{
    m.doc() = "Truth tables and logic network transformations.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.networks");

    pyfiction::register_dynamic_truth_table(m);
    pyfiction::register_truth_tables(m);
    pyfiction::register_fanout_substitution(m);
    pyfiction::register_network_balancing(m);
    pyfiction::register_technology_mapping(m);
    pyfiction::register_network_conversion(m);
    pyfiction::register_planarization(m);
}

#pragma GCC diagnostic pop
