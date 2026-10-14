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

#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/utils/progress.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner): enables keyword options
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using simulation_model = fiction::sidb::model::simulation_parameters;

/**
 * @brief Registers exhaustive ground-state simulation without an internal deadline argument.
 *
 * @param m Python module.
 */
void register_exhaustive_ground_state_simulation(nb::module_& m)
{
    m.def(
        "exhaustive_ground_state_simulation",
        [](const fiction::sidb::layout& lyt, const decltype(simulation_model{}.base) base,
           const decltype(simulation_model{}.mu_minus) mu_minus, const decltype(simulation_model{}.epsilon_r) epsilon_r,
           const decltype(simulation_model{}.lambda_tf) lambda_tf, const fiction::utils::progress_callback& on_progress)
        {
            const auto                   params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::engines::exhaustive_ground_state_simulation(lyt, params, on_progress);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = simulation_model{}.base,
        nb::arg("mu_minus") = simulation_model{}.mu_minus, nb::arg("epsilon_r") = simulation_model{}.epsilon_r,
        nb::arg("lambda_tf") = simulation_model{}.lambda_tf, nb::arg("on_progress").none() = nb::none(),
        R"doc(Find all minimum-energy charge distributions by exhaustive simulation.
The physical model is supplied by the scalar keyword values.

Args:
    base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
    mu_minus: Negative-charge transition energy in eV.
    epsilon_r: Relative permittivity (dimensionless).
    lambda_tf: Thomas-Fermi screening distance in nm.
    on_progress: Callback receiving (task, completed, total); None disables progress reporting.
)doc");
}

}  // namespace pyfiction
