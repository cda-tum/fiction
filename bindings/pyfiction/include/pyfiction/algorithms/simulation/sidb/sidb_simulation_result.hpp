//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_SIDB_SIMULATION_RESULT_HPP
#define PYFICTION_SIDB_SIMULATION_RESULT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>

#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void sidb_simulation_result(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::sidb_simulation_result<Lyt>>(m, "sidb_simulation_result", DOC(fiction_sidb_simulation_result))
        .def(py::init<>())
        .def_readwrite("algorithm_name", &fiction::sidb_simulation_result<Lyt>::algorithm_name,
                       DOC(fiction_sidb_simulation_result_algorithm_name))
        .def_readwrite("simulation_runtime", &fiction::sidb_simulation_result<Lyt>::simulation_runtime,
                       DOC(fiction_sidb_simulation_result_simulation_runtime))
        .def_readwrite("charge_distributions", &fiction::sidb_simulation_result<Lyt>::charge_distributions,
                       DOC(fiction_sidb_simulation_result_charge_distributions))
        .def_readwrite("physical_parameters", &fiction::sidb_simulation_result<Lyt>::physical_parameters,
                       DOC(fiction_sidb_simulation_result_physical_parameters))
        .def_readwrite("additional_simulation_parameters",
                       &fiction::sidb_simulation_result<Lyt>::additional_simulation_parameters,
                       DOC(fiction_sidb_simulation_result_additional_simulation_parameters))

        ;
}

}  // namespace detail

inline void sidb_simulation_result(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::sidb_simulation_result<py_cds_base>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_SIMULATION_RESULT_HPP
