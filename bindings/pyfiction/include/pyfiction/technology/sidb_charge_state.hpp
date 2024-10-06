//
// Created by marcel on 19.09.23.
//

#ifndef PYFICTION_SIDB_CHARGE_STATE_HPP
#define PYFICTION_SIDB_CHARGE_STATE_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb_charge_state.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

inline void sidb_charge_state(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::sidb_charge_state>(m, "sidb_charge_state", DOC(fiction_sidb_charge_state))
        .value("NEGATIVE", fiction::sidb_charge_state::NEGATIVE, DOC(fiction_sidb_charge_state_NEGATIVE))
        .value("NEUTRAL", fiction::sidb_charge_state::NEUTRAL, DOC(fiction_sidb_charge_state_NEUTRAL))
        .value("POSITIVE", fiction::sidb_charge_state::POSITIVE, DOC(fiction_sidb_charge_state_POSITIVE))
        .value("NONE", fiction::sidb_charge_state::NONE, DOC(fiction_sidb_charge_state_NONE))

        ;

    m.attr("SIDB_CHARGE_STATES_BASE_2") = py::cast(fiction::SIDB_CHARGE_STATES_BASE_2);
    m.attr("SIDB_CHARGE_STATES_BASE_3") = py::cast(fiction::SIDB_CHARGE_STATES_BASE_3);

    m.def("sidb_charge_states_for_base_number", &fiction::sidb_charge_states_for_base_number, "base"_a,
          DOC(fiction_sidb_charge_states_for_base_number));
    m.def("charge_state_to_sign", &fiction::charge_state_to_sign, "cs"_a, DOC(fiction_charge_state_to_sign));
    m.def("sign_to_charge_state", &fiction::sign_to_charge_state, "sg"_a, DOC(fiction_sign_to_charge_state));
    m.def("charge_configuration_to_string", &fiction::charge_configuration_to_string, "cc"_a,
          DOC(fiction_charge_configuration_to_string));
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_CHARGE_STATE_HPP
