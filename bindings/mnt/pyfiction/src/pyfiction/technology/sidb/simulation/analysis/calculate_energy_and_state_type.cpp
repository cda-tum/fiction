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
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner): nanobind return-value caster.
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner): nanobind return-value caster.

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

    py::enum_<fiction::sidb::simulation::analysis::state_type>(m, "state_type",
                                                               DOC(fiction_sidb_simulation_analysis_state_type))
        .value("ACCEPTED", fiction::sidb::simulation::analysis::state_type::ACCEPTED,
               DOC(fiction_sidb_simulation_analysis_state_type_ACCEPTED))
        .value("REJECTED", fiction::sidb::simulation::analysis::state_type::REJECTED,
               DOC(fiction_sidb_simulation_analysis_state_type_REJECTED));

    m.def("calculate_energy_and_state_type_with_kinks_accepted",
          &fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_accepted,
          py::arg("energy_distribution"), py::arg("valid_charge_distributions"), py::arg("output_bdl_pairs"),
          py::arg("spec"), py::arg("input_index"),
          DOC(fiction_sidb_simulation_analysis_calculate_energy_and_state_type_with_kinks_accepted));
    m.def("calculate_energy_and_state_type_with_kinks_rejected",
          &fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_rejected, py::arg("lyt"),
          py::arg("energy_distribution"), py::arg("valid_charge_distributions"), py::arg("spec"),
          py::arg("input_index"), py::arg("input_bdl_wires"), py::arg("output_bdl_wires"),
          DOC(fiction_sidb_simulation_analysis_calculate_energy_and_state_type_with_kinks_rejected));
}

}  // namespace pyfiction
