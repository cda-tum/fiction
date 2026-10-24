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
 * @brief Python bindings for `fiction/technology/sidb/model/charge_state.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/charge_state.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds charge state helpers.
 * @param m Python module.
 */
void register_charge_state(nb::module_& m)
{
    // Keep the GIL for scalar charge conversions; formatting a charge configuration releases it.
    nb::enum_<fiction::sidb::model::charge_state>(m, "sidb_charge_state", DOC(fiction_sidb_model_charge_state))
        .value("NEGATIVE", fiction::sidb::model::charge_state::NEGATIVE, DOC(fiction_sidb_model_charge_state_NEGATIVE))
        .value("NEUTRAL", fiction::sidb::model::charge_state::NEUTRAL, DOC(fiction_sidb_model_charge_state_NEUTRAL))
        .value("POSITIVE", fiction::sidb::model::charge_state::POSITIVE, DOC(fiction_sidb_model_charge_state_POSITIVE))
        .value("NONE", fiction::sidb::model::charge_state::NONE, DOC(fiction_sidb_model_charge_state_NONE))

        ;
    m.attr("SIDB_CHARGE_STATES_BASE_2") = nb::cast(fiction::sidb::model::CHARGE_STATES_BASE_2);
    m.attr("SIDB_CHARGE_STATES_BASE_3") = nb::cast(fiction::sidb::model::CHARGE_STATES_BASE_3);

    m.def("sidb_charge_states_for_base_number", &fiction::sidb::model::charge_states_for_base_number, nb::arg("base"),
          DOC(fiction_sidb_model_charge_states_for_base_number));
    m.def("charge_state_to_sign", &fiction::sidb::model::charge_state_to_sign, nb::arg("cs"),
          DOC(fiction_sidb_model_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sidb::model::sign_to_charge_state, nb::arg("sg"),
          DOC(fiction_sidb_model_sign_to_charge_state));
    m.def(
        "charge_configuration_to_string", &fiction::sidb::model::charge_configuration_to_string,
        nb::call_guard<nb::gil_scoped_release>(), nb::arg("cc"),
        parameter_doc(DOC(fiction_sidb_model_charge_configuration_to_string), {{"charge_distribution", "cc"}}).c_str());
}

}  // namespace pyfiction
