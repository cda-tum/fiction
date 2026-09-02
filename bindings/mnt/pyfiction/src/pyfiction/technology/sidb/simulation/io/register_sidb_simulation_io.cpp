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

void write_sqd_sim_result(nanobind::module_& m);
void write_operational_domain(nanobind::module_& m);
void write_location_and_ground_state(nanobind::module_& m);

void register_sidb_simulation_io(nanobind::module_& m)
{
    write_sqd_sim_result(m);
    write_operational_domain(m);
    write_location_and_ground_state(m);
}

}  // namespace pyfiction
