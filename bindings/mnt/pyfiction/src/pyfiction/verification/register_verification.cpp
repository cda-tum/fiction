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
 * @brief Registers the `verification` bindings with the `mnt.pyfiction` module.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void critical_path_length_and_throughput(nanobind::module_& m);
void design_rule_violations(nanobind::module_& m);
void equivalence_checking(nanobind::module_& m);

void register_verification(nanobind::module_& m)
{
    critical_path_length_and_throughput(m);
    design_rule_violations(m);
    equivalence_checking(m);
}

}  // namespace pyfiction
