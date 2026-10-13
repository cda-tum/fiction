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
           const decltype(quickexact_options{}.sim_params.mu_minus)                     mu_minus,
           const decltype(quickexact_options{}.sim_params.epsilon_r)                    epsilon_r,
           const decltype(quickexact_options{}.sim_params.lambda_tf)                    lambda_tf,
           const decltype(quickexact_options{}.base_number_detection)                   base_number_detection,
           const std::optional<decltype(quickexact_options{}.local_external_potential)> local_external_potential,
           const decltype(quickexact_options{}.global_potential)                        global_potential,
           const decltype(quickexact_options{}.on_progress)                             on_progress)
        {
            quickexact_options params{};
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            params.sim_params = fiction::sidb::model::simulation_parameters{base, mu_minus, epsilon_r, lambda_tf};
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
        R"doc(*QuickExact* is a quick and exact physical simulation algorithm
designed specifically for SiDB layouts. It was proposed in \"The Need
for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic\"
by J. Drewniok, M. Walter, and R. Wille in ASP-DAC 2024
(https://ieeexplore.ieee.org/document/10473946). It determines all
physically valid charge configurations of a given SiDB layout,
providing a significant performance advantage of more than three
orders of magnitude over *ExGS*.

The performance improvement stems from the use of physically informed
search space pruning: SiDBs whose local potential forces them negative
are removed from the enumeration and modeled as fixed negative
charges, one SiDB's charge state follows from the others, and
positively charged SiDBs are enumerated only where they can occur.
Charged surface defects of the layout and external potentials enter
the potential landscape.

Args:
    lyt: Layout to simulate.

Returns:
    Simulation result: every physically valid charge distribution of
    `lyt`.

Raises:
    std::out_of_range: if a site has an invalid lattice basis index.
    utils::timeout_error: if the shared caller deadline expires. No
                          partial result is returned.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
