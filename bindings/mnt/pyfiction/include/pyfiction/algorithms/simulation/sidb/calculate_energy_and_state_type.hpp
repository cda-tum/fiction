//
// Created by marcel on 24.05.23.
//

#ifndef PYFICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
#define PYFICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void calculate_energy_and_state_type(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("calculate_energy_and_state_type_with_kinks_accepted",
          &fiction::calculate_energy_and_state_type_with_kinks_accepted<Lyt, py_tt>, "energy_distribution"_a,
          "valid_charge_distributions"_a, "output_bdl_pairs"_a, "spec"_a, "input_index"_a,
          DOC(fiction_calculate_energy_and_state_type_with_kinks_accepted));

    m.def("calculate_energy_and_state_type_with_kinks_rejected",
          &fiction::calculate_energy_and_state_type_with_kinks_rejected<Lyt, py_tt>, "energy_distribution"_a,
          "valid_charge_distributions"_a, "spec"_a, "input_index"_a, "input_bdl_wires"_a, "output_bdl_wires"_a,
          DOC(fiction_calculate_energy_and_state_type_with_kinks_rejected));
}

}  // namespace detail

inline void calculate_energy_and_state_type(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::calculate_energy_and_state_type<py_sidb_100_lattice>(m);
    detail::calculate_energy_and_state_type<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
