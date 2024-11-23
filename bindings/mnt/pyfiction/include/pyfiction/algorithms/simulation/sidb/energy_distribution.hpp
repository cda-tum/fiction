//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_ENERGY_DISTRIBUTION_HPP
#define PYFICTION_ENERGY_DISTRIBUTION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/energy_distribution.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void energy_distribution(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("energy_distribution", &fiction::energy_distribution<Lyt>, "input_vec"_a, DOC(fiction_energy_distribution));
}

}  // namespace detail

inline void energy_distribution(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::energy_distribution<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ENERGY_DISTRIBUTION_HPP
