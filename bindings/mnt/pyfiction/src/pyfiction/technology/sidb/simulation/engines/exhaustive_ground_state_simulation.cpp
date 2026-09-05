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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void exhaustive_ground_state_simulation(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(misc-const-correctness)
    fiction::sidb::simulation::result (*const exgs_pointer)(const fiction::sidb::layout&,
                                                            const fiction::sidb::model::simulation_parameters&) =
        &fiction::sidb::simulation::engines::exhaustive_ground_state_simulation;

    m.def("exhaustive_ground_state_simulation", exgs_pointer, py::arg("lyt"),
          py::arg("params") = fiction::sidb::model::simulation_parameters{},
          DOC(fiction_sidb_simulation_engines_exhaustive_ground_state_simulation));
}

}  // namespace pyfiction
