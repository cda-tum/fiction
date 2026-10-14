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
 * @brief Python bindings for `fiction/technology/sidb/simulation/is_ground_state.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/is_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds is ground state helpers.
 * @param m Python module.
 */
void register_is_ground_state(nb::module_& m)
{
    m.def("is_ground_state", &fiction::sidb::simulation::is_ground_state, nb::arg("heuristic_results"),
          nb::arg("exhaustive_results"), DOC(fiction_sidb_simulation_is_ground_state));
}

}  // namespace pyfiction
