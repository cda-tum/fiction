//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_SIDB_SIMULATION_PARAMETERS_HPP
#define PYFICTION_SIDB_SIMULATION_PARAMETERS_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp>
#include <fiction/technology/sidb_lattice_orientations.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cstdint>

namespace pyfiction
{

inline void sidb_simulation_parameters(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::sidb_simulation_parameters>(m, "sidb_simulation_parameters",
                                                    DOC(fiction_sidb_simulation_parameters))
        .def(py::init<const uint8_t, const double, const double, const double>(), "base_number"_a = 3,
             "mu_minus"_a = -0.32, "relative_permittivity"_a = 5.6, "screening_distance"_a = 5.0,
             DOC(fiction_sidb_simulation_parameters_sidb_simulation_parameters))
        .def(py::init<>())
        .def_readwrite("epsilon_r", &fiction::sidb_simulation_parameters::epsilon_r,
                       DOC(fiction_sidb_simulation_parameters_epsilon_r))
        .def_readwrite("lambda_tf", &fiction::sidb_simulation_parameters::lambda_tf,
                       DOC(fiction_sidb_simulation_parameters_lambda_tf))
        .def_readwrite("mu_minus", &fiction::sidb_simulation_parameters::mu_minus,
                       DOC(fiction_sidb_simulation_parameters_mu_minus))
        .def_readwrite("base", &fiction::sidb_simulation_parameters::base, DOC(fiction_sidb_simulation_parameters_base))
        .def("k", &fiction::sidb_simulation_parameters::k, DOC(fiction_sidb_simulation_parameters_k))
        .def("mu_plus", &fiction::sidb_simulation_parameters::mu_plus, DOC(fiction_sidb_simulation_parameters_mu_plus));
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_SIMULATION_PARAMETERS_HPP
