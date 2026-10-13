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
            if (base != 2 && base != 3)
            {
                throw nb::value_error("base must be 2 or 3");
            }
            simulation_model params{base, mu_minus, epsilon_r, lambda_tf};
            return fiction::sidb::model::potential_to_distance_conversion(potential, params, precision);
        },
        nb::arg("potential"), nb::arg("precision"), nb::kw_only(), nb::arg("base") = simulation_model{}.base,
        nb::arg("mu_minus") = simulation_model{}.mu_minus, nb::arg("epsilon_r") = simulation_model{}.epsilon_r,
        nb::arg("lambda_tf") = simulation_model{}.lambda_tf,
        R"doc(The electrostatic potential on hydrogen-passivated silicon is
typically modeled using a screened Coulomb potential. This
electrostatic potential is commonly employed to determine the
electrostatic potential for a given distance (between SiDB and point
under consideration) and given physical parameters. However, the
function provided here serves the inverse purpose by calculating the
distance for a given potential and given physical parameters.

Args:
    potential: The electrostatic potential (unit: V) to be converted
               to a distance.
    precision: The precision level for the conversion, specifying the
               number of decimal places.

Returns:
    The distance (unit: nm) corresponding to the given electrostatic
    potential.

Note:
    Runtime depends exponentially on the provided precision.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace pyfiction
