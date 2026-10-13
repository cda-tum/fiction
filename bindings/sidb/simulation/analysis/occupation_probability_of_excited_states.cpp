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

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds `occupation_probability_of_excited_states_impl`.
 * @param m Python module.
 */
void bind_occupation_probability_of_excited_states_impl(nb::module_& m)
{
    m.def("occupation_probability_gate_based", &fiction::sidb::simulation::analysis::occupation_probability_gate_based,
          nb::arg("energy_and_state_type"), nb::arg("temperature"),
          DOC(fiction_sidb_simulation_analysis_occupation_probability_gate_based));

    m.def("occupation_probability_non_gate_based",
          &fiction::sidb::simulation::analysis::occupation_probability_non_gate_based,
          nb::arg("calculate_energy_distribution"),
          nb::arg("temperature"));  // TODO DOC(fiction_sidb_simulation_analysis_occupation_probability_non_gate_based)
}

}  // namespace detail

/**
 * @brief Binds `occupation_probability_of_excited_states`.
 * @param m Python module.
 */
void bind_occupation_probability_of_excited_states(nb::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::bind_occupation_probability_of_excited_states_impl(m);
}

}  // namespace pyfiction
