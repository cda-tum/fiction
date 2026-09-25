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
 * @brief Python bindings for `occupation_probability_of_excited_states.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

void occupation_probability_of_excited_states_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("occupation_probability_gate_based", &fiction::sidb::simulation::analysis::occupation_probability_gate_based,
          py::arg("energy_and_state_type"), py::arg("temperature"),
          DOC(fiction_sidb_simulation_analysis_occupation_probability_gate_based));

    m.def("occupation_probability_non_gate_based",
          &fiction::sidb::simulation::analysis::occupation_probability_non_gate_based,
          py::arg("calculate_energy_distribution"),
          py::arg("temperature"));  // TODO DOC(fiction_sidb_simulation_analysis_occupation_probability_non_gate_based)
}

}  // namespace detail

void occupation_probability_of_excited_states(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::occupation_probability_of_excited_states_impl(m);
}

}  // namespace pyfiction
