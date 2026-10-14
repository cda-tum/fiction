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

/**
 * @brief Binds bdl input iterator helpers.
 * @param m Python module.
 */
void register_bdl_input_iterator(nb::module_& m);
/**
 * @brief Binds detect bdl pairs helpers.
 * @param m Python module.
 */
void register_detect_bdl_pairs(nb::module_& m);
/**
 * @brief Binds detect bdl wires helpers.
 * @param m Python module.
 */
void register_detect_bdl_wires(nb::module_& m);
/**
 * @brief Binds is operational helpers.
 * @param m Python module.
 */
void register_is_operational(nb::module_& m);
/**
 * @brief Binds operational domain helpers.
 * @param m Python module.
 */
void register_operational_domain(nb::module_& m);
/**
 * @brief Binds operational domain ratio helpers.
 * @param m Python module.
 */
void register_operational_domain_ratio(nb::module_& m);

void register_sidb_simulation_logic(nb::module_& m)
{
    register_bdl_input_iterator(m);
    register_detect_bdl_pairs(m);
    register_detect_bdl_wires(m);
    register_is_operational(m);
    register_operational_domain(m);
    register_operational_domain_ratio(m);
}

}  // namespace pyfiction
