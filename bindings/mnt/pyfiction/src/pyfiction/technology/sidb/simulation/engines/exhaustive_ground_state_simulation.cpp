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

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers exhaustive ground-state simulation without an internal deadline argument.
 *
 * @param m Python module.
 */
void exhaustive_ground_state_simulation(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "exhaustive_ground_state_simulation",
        [](const fiction::sidb::layout& layout, const fiction::sidb::model::simulation_parameters& params)
        { return fiction::sidb::simulation::engines::exhaustive_ground_state_simulation(layout, params); },
        py::arg("lyt"), py::arg("params") = fiction::sidb::model::simulation_parameters{},
        DOC(fiction_sidb_simulation_engines_exhaustive_ground_state_simulation));
}

}  // namespace pyfiction
