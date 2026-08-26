//
// Created by marcel on 19.09.23.
//

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/model/charge_state.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void sidb_charge_state(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb::model::charge_state>(m, "sidb_charge_state", DOC(fiction_sidb_charge_state))
        .value("NEGATIVE", fiction::sidb::model::charge_state::NEGATIVE, DOC(fiction_sidb_charge_state_NEGATIVE))
        .value("NEUTRAL", fiction::sidb::model::charge_state::NEUTRAL, DOC(fiction_sidb_charge_state_NEUTRAL))
        .value("POSITIVE", fiction::sidb::model::charge_state::POSITIVE, DOC(fiction_sidb_charge_state_POSITIVE))
        .value("NONE", fiction::sidb::model::charge_state::NONE, DOC(fiction_sidb_charge_state_NONE))

        ;
    m.attr("SIDB_CHARGE_STATES_BASE_2") = py::cast(fiction::sidb::model::SIDB_CHARGE_STATES_BASE_2);
    m.attr("SIDB_CHARGE_STATES_BASE_3") = py::cast(fiction::sidb::model::SIDB_CHARGE_STATES_BASE_3);

    m.def("sidb_charge_states_for_base_number", &fiction::sidb::model::charge_states_for_base_number, py::arg("base"),
          DOC(fiction_sidb_charge_states_for_base_number));
    m.def("charge_state_to_sign", &fiction::sidb::model::charge_state_to_sign, py::arg("cs"),
          DOC(fiction_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sidb::model::sign_to_charge_state, py::arg("sg"),
          DOC(fiction_sign_to_charge_state));
    m.def("charge_configuration_to_string", &fiction::sidb::model::charge_configuration_to_string, py::arg("cc"),
          DOC(fiction_charge_configuration_to_string));
}

}  // namespace pyfiction
