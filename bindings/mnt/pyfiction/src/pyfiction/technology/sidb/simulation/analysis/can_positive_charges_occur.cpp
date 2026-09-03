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

#include <fiction/technology/sidb/layout.hpp>
#include <fiction/technology/sidb/model/simulation_parameters.hpp>
#include <fiction/technology/sidb/simulation/analysis/can_positive_charges_occur.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

void can_positive_charges_occur(nanobind::module_& m)
{
    namespace py = nanobind;

    m.def(
        "can_positive_charges_occur",
        [](const fiction::sidb::layout& lyt, const fiction::sidb::model::simulation_parameters& sim_params)
        { return fiction::sidb::simulation::analysis::can_positive_charges_occur(lyt, sim_params); }, py::arg("lyt"),
        py::arg("sim_params"), DOC(fiction_sidb_simulation_analysis_can_positive_charges_occur_2));
}

}  // namespace pyfiction
