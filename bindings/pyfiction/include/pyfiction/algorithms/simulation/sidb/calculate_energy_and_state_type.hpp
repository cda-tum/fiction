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

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void calculate_energy_and_state_type(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    m.def("calculate_energy_and_state_type", &fiction::calculate_energy_and_state_type<Lyt>, "energy_distribution"_a,
          "valid_lyts"_a, "output_cells"_a, "output_bits"_a, DOC(fiction_calculate_energy_and_state_type));
}

}  // namespace detail

inline void calculate_energy_and_state_type(pybind11::module& m)
{
    namespace py = pybind11;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::calculate_energy_and_state_type<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CALCULATE_ENERGY_AND_STATE_TYPE_HPP
