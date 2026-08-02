//
// Created by marcel on 19.09.23.
//

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb_charge_state.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void sidb_charge_state(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb_charge_state>(m, "sidb_charge_state", DOC(fiction_sidb_charge_state))
        .value("NEGATIVE", fiction::sidb_charge_state::NEGATIVE, DOC(fiction_sidb_charge_state_NEGATIVE))
        .value("NEUTRAL", fiction::sidb_charge_state::NEUTRAL, DOC(fiction_sidb_charge_state_NEUTRAL))
        .value("POSITIVE", fiction::sidb_charge_state::POSITIVE, DOC(fiction_sidb_charge_state_POSITIVE))
        .value("NONE", fiction::sidb_charge_state::NONE, DOC(fiction_sidb_charge_state_NONE))

        ;
    m.attr("SIDB_CHARGE_STATES_BASE_2") = py::cast(fiction::SIDB_CHARGE_STATES_BASE_2);
    m.attr("SIDB_CHARGE_STATES_BASE_3") = py::cast(fiction::SIDB_CHARGE_STATES_BASE_3);

    m.def("sidb_charge_states_for_base_number", &fiction::sidb_charge_states_for_base_number, py::arg("base"),
          DOC(fiction_sidb_charge_states_for_base_number));
    m.def("charge_state_to_sign", &fiction::charge_state_to_sign, py::arg("cs"), DOC(fiction_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sign_to_charge_state, py::arg("sg"), DOC(fiction_sign_to_charge_state));
    m.def("charge_configuration_to_string", &fiction::charge_configuration_to_string, py::arg("cc"),
          DOC(fiction_charge_configuration_to_string));
}

}  // namespace pyfiction
