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
void dynamic_truth_table(nb::module_& m);

void truth_tables(nb::module_& m);
void fanout_substitution(nb::module_& m);
void network_balancing(nb::module_& m);
void technology_mapping(nb::module_& m);
void network_conversion(nb::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(synthesis, m)
{
    m.doc() = "Truth tables and logic network transformations.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.networks");

    pyfiction::dynamic_truth_table(m);
    pyfiction::truth_tables(m);
    pyfiction::fanout_substitution(m);
    pyfiction::network_balancing(m);
    pyfiction::technology_mapping(m);
    pyfiction::network_conversion(m);
}

#pragma GCC diagnostic pop
