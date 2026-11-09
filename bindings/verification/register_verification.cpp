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
 * @brief Entry point of the `mnt.pyfiction.verification` extension module.
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
 * @brief Binds count gate types helpers.
 * @param m Python module.
 */
void register_count_gate_types(nb::module_& m);
/**
 * @brief Binds critical path length and throughput helpers.
 * @param m Python module.
 */
void register_critical_path_length_and_throughput(nb::module_& m);
/**
 * @brief Binds design rule violations helpers.
 * @param m Python module.
 */
void register_design_rule_violations(nb::module_& m);
/**
 * @brief Binds equivalence checking helpers.
 * @param m Python module.
 */
void register_equivalence_checking(nb::module_& m);

}  // namespace pyfiction

NB_MODULE(verification, m)
{
    m.doc() = "Design rule, equivalence, and performance checks of gate-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nb::module_::import_("mnt.pyfiction.layouts");
    nb::module_::import_("mnt.pyfiction.networks");

    pyfiction::register_count_gate_types(m);
    pyfiction::register_critical_path_length_and_throughput(m);
    pyfiction::register_design_rule_violations(m);
    pyfiction::register_equivalence_checking(m);
}
