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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engines/quickexact.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/simulation/engines/quickexact.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
// These headers register nanobind type casters without exposing directly referenced symbols.
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using quickexact_options = fiction::sidb::simulation::engines::quickexact_params;

/**
 * @brief Binds quickexact helpers.
 * @param m Python module.
 */
void register_quickexact(nb::module_& m)
{

    nb::enum_<quickexact_options::automatic_base_number_detection>(
        m, "automatic_base_number_detection",
        DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection))
        .value("ON", quickexact_options::automatic_base_number_detection::ON,
               DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", quickexact_options::automatic_base_number_detection::OFF,
               DOC(fiction_sidb_simulation_engines_quickexact_params_automatic_base_number_detection_OFF));
    m.def(
        "quickexact",
        [](const fiction::sidb::layout& lyt, const decltype(quickexact_options{}.sim_params.base) base,
           const decltype(quickexact_options{}.sim_params.mu_minus)                      mu_minus,
           const decltype(quickexact_options{}.sim_params.epsilon_r)                     epsilon_r,
           const decltype(quickexact_options{}.sim_params.lambda_tf)                     lambda_tf,
           const decltype(quickexact_options{}.base_number_detection)                    base_number_detection,
           const std::optional<decltype(quickexact_options{}.local_external_potential)>& local_external_potential,
           const decltype(quickexact_options{}.global_potential)                         global_potential,
           const decltype(quickexact_options{}.on_progress)&                             on_progress)
        {
            quickexact_options params{};
            params.sim_params            = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.base_number_detection = base_number_detection;
            if (local_external_potential)
            {
                params.local_external_potential = *local_external_potential;
            }
            params.global_potential = global_potential;
            params.on_progress      = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::simulation::engines::quickexact(lyt, params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = quickexact_options{}.sim_params.base,
        nb::arg("mu_minus")                 = quickexact_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")                = quickexact_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")                = quickexact_options{}.sim_params.lambda_tf,
        nb::arg("base_number_detection")    = quickexact_options{}.base_number_detection,
        nb::arg("local_external_potential") = nb::none(),
        nb::arg("global_potential") = quickexact_options{}.global_potential, nb::arg("on_progress").none() = nb::none(),
        R"doc(Simulate the SiDB layout with QuickExact.
The keyword options select physical values, base detection, external potentials, and progress reporting.
Raises ValueError when base is neither 2 nor 3.
)doc");
}

}  // namespace pyfiction
