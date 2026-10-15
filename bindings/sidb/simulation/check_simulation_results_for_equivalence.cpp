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
 * @brief Python bindings for `fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/check_simulation_results_for_equivalence.hpp>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers charge-distribution equivalence checking.
 *
 * @param m Target Python module.
 */
void register_check_simulation_results_for_equivalence(nb::module_& m)
{
    m.def("check_simulation_results_for_equivalence",
          &fiction::sidb::simulation::check_simulation_results_for_equivalence,
          nb::call_guard<nb::gil_scoped_release>(), nb::arg("result1"), nb::arg("result2"),
          DOC(fiction_sidb_simulation_check_simulation_results_for_equivalence));
}

}  // namespace pyfiction
