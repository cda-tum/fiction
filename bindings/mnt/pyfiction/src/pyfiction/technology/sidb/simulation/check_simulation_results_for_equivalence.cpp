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
#include "pyfiction/types.hpp"

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
          DOC(fiction_sidb_simulation_check_simulation_results_for_equivalence_2));
    m.def("check_simulation_results_for_equivalence",
          static_cast<bool (*)(fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>,
                               fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>)>(
              &fiction::sidb::simulation::check_simulation_results_for_equivalence<py_sidb_100_lattice>),
          py::arg("result1"), py::arg("result2"),
          DOC(fiction_sidb_simulation_check_simulation_results_for_equivalence));
    m.def("check_simulation_results_for_equivalence",
          static_cast<bool (*)(fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>,
                               fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>)>(
              &fiction::sidb::simulation::check_simulation_results_for_equivalence<py_sidb_111_lattice>),
          py::arg("result1"), py::arg("result2"),
          DOC(fiction_sidb_simulation_check_simulation_results_for_equivalence));
}

}  // namespace pyfiction
