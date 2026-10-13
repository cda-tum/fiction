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

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>
#include <fiction/utils/progress.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Registers exhaustive ground-state simulation without an internal deadline argument.
 *
 * @param m Python module.
 */
void bind_exhaustive_ground_state_simulation(nb::module_& m)
{
    m.def(
        "exhaustive_ground_state_simulation",
        [](const fiction::sidb::layout& lyt, const fiction::sidb::model::simulation_parameters& params,
           const fiction::utils::progress_callback& on_progress)
        { return fiction::sidb::simulation::engines::exhaustive_ground_state_simulation(lyt, params, on_progress); },
        nb::arg("lyt"), nb::arg("params") = fiction::sidb::model::simulation_parameters{},
        nb::arg("on_progress").none() = nb::none(), nb::call_guard<nb::gil_scoped_release>(),
        DOC(fiction_sidb_simulation_engines_exhaustive_ground_state_simulation));
}

}  // namespace pyfiction
