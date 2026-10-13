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
 * @brief Python bindings for `fiction/technology/sidb/simulation/analysis/minimum_energy.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/simulation/analysis/minimum_energy.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds minimum energy helpers.
 * @param m Python module.
 */
void register_minimum_energy(nb::module_& m)
{
    m.def(
        "minimum_energy",
        [](const std::vector<fiction::sidb::charge_distribution>& charge_distributions) -> double
        {
            return fiction::sidb::simulation::analysis::minimum_energy(charge_distributions.cbegin(),
                                                                       charge_distributions.cend());
        },
        nb::arg("charge_distributions"), DOC(fiction_sidb_simulation_analysis_minimum_energy));
}

}  // namespace pyfiction
