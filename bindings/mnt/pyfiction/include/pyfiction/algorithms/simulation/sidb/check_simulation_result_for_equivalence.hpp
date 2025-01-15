//
// Created by Jan Drewniok on 11.04.24.
//

#ifndef PYICTION_CHECK_SIMULATION_RESULTS_FOR_EQUIVALENCE_HPP
#define PYICTION_CHECK_SIMULATION_RESULTS_FOR_EQUIVALENCE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void check_for_equivalence(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("check_simulation_results_for_equivalence", &fiction::check_simulation_results_for_equivalence<Lyt>,
          py::arg("result1") = fiction::sidb_simulation_result<Lyt>{},
          py::arg("result2") = fiction::sidb_simulation_result<Lyt>{},
          DOC(fiction_check_simulation_results_for_equivalence));
}

}  // namespace detail

inline void check_simulation_results_for_equivalence(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::check_for_equivalence<py_sidb_100_lattice>(m);
    detail::check_for_equivalence<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_CHECK_SIMULATION_RESULTS_FOR_EQUIVALENCE_HPP
