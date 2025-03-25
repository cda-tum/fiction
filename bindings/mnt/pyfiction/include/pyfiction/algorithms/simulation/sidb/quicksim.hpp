//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_QUICKSIM_HPP
#define PYFICTION_QUICKSIM_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/quicksim.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quicksim(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("quicksim", &fiction::quicksim<Lyt>, py::arg("lyt"), py::arg("params") = fiction::quicksim_params{},
          DOC(fiction_quicksim));
}

}  // namespace detail

inline void quicksim(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * QuickSim parameters.
     */
    py::class_<fiction::quicksim_params>(m, "quicksim_params", DOC(fiction_quicksim_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::quicksim_params::simulation_parameters,
                       DOC(fiction_quicksim_params_simulation_parameters))
        .def_readwrite("iteration_steps", &fiction::quicksim_params::iteration_steps,
                       DOC(fiction_quicksim_params_iteration_steps))
        .def_readwrite("alpha", &fiction::quicksim_params::alpha, DOC(fiction_quicksim_params_alpha))
        .def_readwrite("number_threads", &fiction::quicksim_params::number_threads,
                       DOC(fiction_quicksim_params_number_threads))
        .def_readwrite("timeout", &fiction::quicksim_params::timeout, DOC(fiction_quicksim_params_timeout));

    ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::quicksim<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::quicksim<py_sidb_111_lattice<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_QUICKSIM_HPP
