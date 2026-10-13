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
 * @brief Python bindings for `fiction/technology/sidb/model/potential_to_distance_conversion.hpp`.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/model/potential_to_distance_conversion.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner): enables keyword options

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using simulation_model = fiction::sidb::model::simulation_parameters;

/**
 * @brief Binds potential to distance conversion helpers.
 * @param m Python module.
 */
void register_potential_to_distance_conversion(nb::module_& m)
{
    m.def(
        "potential_to_distance_conversion",
        [](double potential, double precision, const decltype(simulation_model{}.base) base,
           const decltype(simulation_model{}.mu_minus) mu_minus, const decltype(simulation_model{}.epsilon_r) epsilon_r,
           const decltype(simulation_model{}.lambda_tf) lambda_tf)
        {
            const auto params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            return fiction::sidb::model::potential_to_distance_conversion(potential, params, precision);
        },
        nb::arg("potential"), nb::arg("precision"), nb::kw_only(), nb::arg("base") = simulation_model{}.base,
        nb::arg("mu_minus") = simulation_model{}.mu_minus, nb::arg("epsilon_r") = simulation_model{}.epsilon_r,
        nb::arg("lambda_tf") = simulation_model{}.lambda_tf,
        R"doc(Compute the distance corresponding to the potential with the requested precision.
The physical model is supplied by the scalar keyword values.
)doc");
}

}  // namespace pyfiction
