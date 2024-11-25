//
// Created by Jan Drewniok on 08.04.24.
//

#ifndef PYFICTION_DETERMINE_GROUNDSTATE_FROM_SIMULATION_RESULTS_HPP
#define PYFICTION_DETERMINE_GROUNDSTATE_FROM_SIMULATION_RESULTS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/determine_groundstate_from_simulation_results.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void determine_groundstate_from_simulation_results(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("determine_groundstate_from_simulation_results", &fiction::determine_groundstate_from_simulation_results<Lyt>,
          py::arg("simulation_results"), DOC(fiction_determine_groundstate_from_simulation_results));
}

}  // namespace detail

inline void determine_groundstate_from_simulation_results(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::determine_groundstate_from_simulation_results<py_sidb_100_lattice>(m);
    detail::determine_groundstate_from_simulation_results<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_DETERMINE_GROUNDSTATE_FROM_SIMULATION_RESULTS_HPP
