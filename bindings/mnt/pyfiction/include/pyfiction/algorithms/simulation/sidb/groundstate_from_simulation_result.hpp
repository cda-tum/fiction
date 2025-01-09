//
// Created by Jan Drewniok on 08.04.24.
//

#ifndef PYFICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP
#define PYFICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/groundstate_from_simulation_result.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void groundstate_from_simulation_result(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("groundstate_from_simulation_result", &fiction::groundstate_from_simulation_result<Lyt>,
          py::arg("simulation_results"), DOC(fiction_groundstate_from_simulation_result));
}

}  // namespace detail

inline void groundstate_from_simulation_result(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::groundstate_from_simulation_result<py_sidb_100_lattice>(m);
    detail::groundstate_from_simulation_result<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_GROUNDSTATE_FROM_SIMULATION_RESULT_HPP
