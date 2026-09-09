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

namespace pyfiction
{

void minimum_energy(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "minimum_energy",
        [](const std::vector<fiction::sidb::charge_distribution>& charge_distributions) -> double
        {
            return fiction::sidb::simulation::analysis::minimum_energy(charge_distributions.cbegin(),
                                                                       charge_distributions.cend());
        },
        py::arg("charge_distributions"), DOC(fiction_sidb_simulation_analysis_minimum_energy));
}

}  // namespace pyfiction
