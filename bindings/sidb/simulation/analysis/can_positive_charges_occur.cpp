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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>

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
 * @brief Binds can positive charges occur helpers.
 * @param m Python module.
 */
void register_can_positive_charges_occur(nb::module_& m)
{
    m.def(
        "can_positive_charges_occur",
        [](const fiction::sidb::layout& lyt, const decltype(simulation_model{}.base) base,
           const decltype(simulation_model{}.mu_minus) mu_minus, const decltype(simulation_model{}.epsilon_r) epsilon_r,
           const decltype(simulation_model{}.lambda_tf) lambda_tf)
        {
            const auto sim_params = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            return fiction::sidb::simulation::analysis::can_positive_charges_occur(lyt, sim_params);
        },
        nb::arg("lyt"), nb::kw_only(), nb::arg("base") = simulation_model{}.base,
        nb::arg("mu_minus") = simulation_model{}.mu_minus, nb::arg("epsilon_r") = simulation_model{}.epsilon_r,
        nb::arg("lambda_tf") = simulation_model{}.lambda_tf,
        R"doc(Check whether positively charged SiDBs can occur under the scalar physical model.
)doc");
}

}  // namespace pyfiction
