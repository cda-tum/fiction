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

namespace pyfiction
{

void count_gate_types(nanobind::module_& m);
void critical_path_length_and_throughput(nanobind::module_& m);
void design_rule_violations(nanobind::module_& m);
void equivalence_checking(nanobind::module_& m);

}  // namespace pyfiction

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

NB_MODULE(verification, m)
{
    m.doc() = "Design rule, equivalence, and performance checks of gate-level layouts.";

    // Registers the types this module names in signatures and default arguments.
    nanobind::module_::import_("mnt.pyfiction.layouts");
    nanobind::module_::import_("mnt.pyfiction.networks");

    pyfiction::count_gate_types(m);
    pyfiction::critical_path_length_and_throughput(m);
    pyfiction::design_rule_violations(m);
    pyfiction::equivalence_checking(m);
}

#pragma GCC diagnostic pop
