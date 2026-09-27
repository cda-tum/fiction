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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Registers charge-distribution energy labels and logic classification.
 *
 * @param m The Python module.
 */
void calculate_energy_and_state_type(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb::simulation::analysis::state_type>(m, "StateType",
                                                               DOC(fiction_sidb_simulation_analysis_state_type))
        .value("ACCEPTED", fiction::sidb::simulation::analysis::state_type::ACCEPTED,
               DOC(fiction_sidb_simulation_analysis_state_type_ACCEPTED))
        .value("REJECTED", fiction::sidb::simulation::analysis::state_type::REJECTED,
               DOC(fiction_sidb_simulation_analysis_state_type_REJECTED));
}

}  // namespace pyfiction
