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

    // NOLINTNEXTLINE(misc-const-correctness)
    bool (*const is_ground_state_pointer)(const fiction::sidb::simulation::result&,
                                          const fiction::sidb::simulation::result&) =
        &fiction::sidb::simulation::is_ground_state;
    bool (*const is_ground_state_100_pointer)(const fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>&,
                                              const fiction::sidb::simulation::legacy_result<py_sidb_100_lattice>&) =
        &fiction::sidb::simulation::is_ground_state<py_sidb_100_lattice>;
    bool (*const is_ground_state_111_pointer)(const fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>&,
                                              const fiction::sidb::simulation::legacy_result<py_sidb_111_lattice>&) =
        &fiction::sidb::simulation::is_ground_state<py_sidb_111_lattice>;

    m.def("is_ground_state", is_ground_state_pointer, py::arg("heuristic_results"), py::arg("exhaustive_results"),
          DOC(fiction_sidb_simulation_is_ground_state_2));
    m.def("is_ground_state", is_ground_state_100_pointer, py::arg("heuristic_results"), py::arg("exhaustive_results"),
          DOC(fiction_sidb_simulation_is_ground_state));
    m.def("is_ground_state", is_ground_state_111_pointer, py::arg("heuristic_results"), py::arg("exhaustive_results"),
          DOC(fiction_sidb_simulation_is_ground_state));
}

}  // namespace pyfiction
