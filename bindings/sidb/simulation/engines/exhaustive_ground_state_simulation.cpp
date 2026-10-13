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
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/utils/progress.hpp>

#include <optional>

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
        R"doc(*Exhaustive Ground State Simulation* (*ExGS*) which was proposed in
\"Computer-Aided Design of Atomic Silicon Quantum Dots and
Computational Applications\" by S. S. H. Ng
(https://dx.doi.org/10.14288/1.0392909) enumerates every charge
distribution of the layout in base 2 or 3 and keeps the physically
valid ones. Charged surface defects of the layout enter the potential
landscape.

Args:
    lyt: Layout to simulate.
    on_progress: Callback that receives the number of enumerated
                 charge configurations.
    deadline: Shared caller deadline. `time_point::max()` leaves the
              simulation unlimited.

Returns:
    The physically valid charge distributions.

Raises:
    std::out_of_range: if a site has an invalid lattice basis index.
    utils::timeout_error: if the shared caller deadline expires. No
                          partial result is returned.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
