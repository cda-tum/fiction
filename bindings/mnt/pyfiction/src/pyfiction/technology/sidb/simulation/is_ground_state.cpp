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
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/is_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

void is_ground_state(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def("is_ground_state",
          static_cast<bool (*)(const fiction::sidb::simulation::result&, const fiction::sidb::simulation::result&)>(
              &fiction::sidb::simulation::is_ground_state),
          py::arg("heuristic_results"), py::arg("exhaustive_results"), DOC(fiction_sidb_simulation_is_ground_state_2));
    m.def("is_ground_state",
          static_cast<bool (*)(const fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>&,
                               const fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>&)>(
              &fiction::sidb::simulation::is_ground_state<py_sidb_100_lattice>),
          py::arg("heuristic_results"), py::arg("exhaustive_results"), DOC(fiction_sidb_simulation_is_ground_state));
    m.def("is_ground_state",
          static_cast<bool (*)(const fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>&,
                               const fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>&)>(
              &fiction::sidb::simulation::is_ground_state<py_sidb_111_lattice>),
          py::arg("heuristic_results"), py::arg("exhaustive_results"), DOC(fiction_sidb_simulation_is_ground_state));
}

}  // namespace pyfiction
