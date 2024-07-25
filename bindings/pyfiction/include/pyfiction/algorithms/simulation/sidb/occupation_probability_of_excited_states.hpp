//
// Created by marcel on 14.06.23.
//

#ifndef PYFICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP
#define PYFICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

void occupation_probability_of_excited_states(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("occupation_probability_gate_based", &fiction::occupation_probability_gate_based, "energy_and_state_type"_a,
          "temperature"_a, DOC(fiction_occupation_probability_gate_based));

    m.def("occupation_probability_non_gate_based", &fiction::occupation_probability_non_gate_based,
          "energy_distribution"_a, "temperature"_a);
    // TODO DOC(fiction_occupation_probability_non_gate_based)
}

}  // namespace detail

inline void occupation_probability_of_excited_states(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::occupation_probability_of_excited_states(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_OCCUPATION_PROBABILITY_OF_EXCITED_STATES_HPP
