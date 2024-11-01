//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP
#define PYFICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/maximum_defect_influence_position_and_distance.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void maximum_defect_influence_distance(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("maximum_defect_influence_position_and_distance",
          &fiction::maximum_defect_influence_position_and_distance<Lyt>, "lyt"_a,
          "params"_a = fiction::maximum_defect_influence_position_and_distance_params{}, "stats"_a = nullptr,
          DOC(fiction_maximum_defect_influence_position_and_distance));
}

}  // namespace detail

inline void maximum_defect_influence_distance(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::maximum_defect_influence_position_and_distance_params>(
        m, "maximum_defect_influence_distance_params")
        .def(py::init<>())
        .def_readwrite("defect", &fiction::maximum_defect_influence_position_and_distance_params::defect)
        .def_readwrite("sim_params",
                       &fiction::maximum_defect_influence_position_and_distance_params::simulation_parameters)
        .def_readwrite("additional_scanning_area",
                       &fiction::maximum_defect_influence_position_and_distance_params::additional_scanning_area);

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::maximum_defect_influence_distance<py_sidb_100_lattice_cube>(m);
    detail::maximum_defect_influence_distance<py_sidb_111_lattice_cube>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_MAXIMUM_DEFECT_INFLUENCE_POSITION_AND_DISTANCE_HPP
