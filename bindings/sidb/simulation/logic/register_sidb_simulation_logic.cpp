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
 * @brief Registers the bindings of the `mnt.pyfiction.sidb.simulation.logic` submodule.
 * @author Marcel Walter (marcelwa)
 */

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

void bdl_input_iterator(nb::module_& m);
void detect_bdl_pairs(nb::module_& m);
void detect_bdl_wires(nb::module_& m);
void is_operational(nb::module_& m);
void operational_domain(nb::module_& m);
void operational_domain_ratio(nb::module_& m);

void register_sidb_simulation_logic(nb::module_& m)
{
    bdl_input_iterator(m);
    detect_bdl_pairs(m);
    detect_bdl_wires(m);
    is_operational(m);
    operational_domain(m);
    operational_domain_ratio(m);
}

}  // namespace pyfiction
