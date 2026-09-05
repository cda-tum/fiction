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
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

void check_simulation_results_for_equivalence(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def("check_simulation_results_for_equivalence",
          static_cast<bool (*)(fiction::sidb::simulation::result, fiction::sidb::simulation::result)>(
              &fiction::sidb::simulation::check_simulation_results_for_equivalence),
          py::arg("result1"), py::arg("result2"),
          DOC(fiction_sidb_simulation_check_simulation_results_for_equivalence));
}

}  // namespace pyfiction
