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
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/potential_to_distance_conversion.hpp>

#include <nanobind/nanobind.h>

namespace pyfiction
{

void potential_to_distance_conversion(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("potential_to_distance_conversion", &fiction::sidb::model::potential_to_distance_conversion,
          py::arg("potential"), py::arg("params"), py::arg("precision"),
          DOC(fiction_sidb_model_potential_to_distance_conversion));
}

}  // namespace pyfiction
