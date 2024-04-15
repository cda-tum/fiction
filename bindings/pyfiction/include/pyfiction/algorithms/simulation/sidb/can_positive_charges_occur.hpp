//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
#define PYFICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void can_positive_charges_occur(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("can_positive_charges_occur", &fiction::can_positive_charges_occur<Lyt>, "lyt"_a, "sim_params"_a, DOC(fiction_can_positive_charges_occur));
}

}  // namespace detail

inline void can_positive_charges_occur(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::can_positive_charges_occur<py_sidb_100_lattice>(m);
    detail::can_positive_charges_occur<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CAN_POSITIVE_CHARGES_OCCUR_HPP
