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
    namespace py = pybind11;

    m.def("energy_distribution", &fiction::energy_distribution<Lyt>, py::arg("input_vec"),
          DOC(fiction_energy_distribution));
}

}  // namespace detail

inline void energy_distribution(pybind11::module& m)
{
    namespace py = pybind11;

    // todo update docu
    py::class_<fiction::sidb_energy_distribution>(m, "sidb_energy_distribution")
        .def(py::init<>())
        .def("get_nth_state", &fiction::sidb_energy_distribution::get_nth_state, py::arg("state_index"),
             DOC(fiction_sidb_energy_distribution_get_nth_state))
        .def("degeneracy_of_given_energy", &fiction::sidb_energy_distribution::degeneracy_of_given_energy,
             py::arg("energy"), DOC(fiction_sidb_energy_distribution_degeneracy_of_given_energy))
        .def("add_state", &fiction::sidb_energy_distribution::add_state, py::arg("energy"), py::arg("degeneracy"),
             DOC(fiction_sidb_energy_distribution_add_state))
        .def("size", &fiction::sidb_energy_distribution::size, DOC(fiction_sidb_energy_distribution_size))
        .def("empty", &fiction::sidb_energy_distribution::empty, DOC(fiction_sidb_energy_distribution_empty))
        .def("max_energy", &fiction::sidb_energy_distribution::max_energy,
             DOC(fiction_sidb_energy_distribution_max_energy))
        .def("min_energy", &fiction::sidb_energy_distribution::min_energy,
             DOC(fiction_sidb_energy_distribution_min_energy))
        .def("get_distribution", &fiction::sidb_energy_distribution::get_distribution,
             DOC(fiction_sidb_energy_distribution_get_distribution));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::energy_distribution<py_sidb_100_lattice>(m);
    detail::energy_distribution<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ENERGY_DISTRIBUTION_HPP
