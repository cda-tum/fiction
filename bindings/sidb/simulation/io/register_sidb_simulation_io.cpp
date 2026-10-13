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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.io` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void write_sqd_sim_result(nb::module_& m);
void write_operational_domain(nb::module_& m);
void write_location_and_ground_state(nb::module_& m);

void register_sidb_simulation_io(nb::module_& m)
{
    write_sqd_sim_result(m);
    write_operational_domain(m);
    write_location_and_ground_state(m);
}

}  // namespace pyfiction
