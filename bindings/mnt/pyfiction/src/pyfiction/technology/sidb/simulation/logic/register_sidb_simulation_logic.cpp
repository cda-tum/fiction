/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

void bdl_input_iterator(nanobind::module_& m);
void detect_bdl_pairs(nanobind::module_& m);
void detect_bdl_wires(nanobind::module_& m);
void is_operational(nanobind::module_& m);
void operational_domain(nanobind::module_& m);
void operational_domain_ratio(nanobind::module_& m);

void register_sidb_simulation_logic(nanobind::module_& m)
{
    bdl_input_iterator(m);
    detect_bdl_pairs(m);
    detect_bdl_wires(m);
    is_operational(m);
    operational_domain(m);
    operational_domain_ratio(m);
}

}  // namespace pyfiction
