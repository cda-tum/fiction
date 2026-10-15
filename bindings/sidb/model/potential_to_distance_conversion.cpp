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

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds potential to distance conversion helpers.
 * @param m Python module.
 */
void register_potential_to_distance_conversion(nb::module_& m)
{
    m.def("potential_to_distance_conversion", &fiction::sidb::model::potential_to_distance_conversion,
          nb::arg("potential"), nb::arg("params"), nb::arg("precision"),
          value_error_doc(DOC(fiction_sidb_model_potential_to_distance_conversion)).c_str());
}

}  // namespace pyfiction
