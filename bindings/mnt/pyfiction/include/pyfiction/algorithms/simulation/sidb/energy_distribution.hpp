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

#include <cstdint>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void energy_distribution(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("calculate_energy_distribution", &fiction::calculate_energy_distribution<Lyt>, py::arg("input_vec"),
          DOC(fiction_energy_distribution));
}

}  // namespace detail

inline void energy_distribution(pybind11::module& m)
{
    namespace py = pybind11;

    py::class_<fiction::energy_state>(m, "energy_state")
        .def(py::init<double, uint64_t>(), py::arg("electrostatic_potential_energy"), py::arg("degeneracy"),
             DOC(fiction_energy_state))
        .def_readwrite("electrostatic_potential_energy", &fiction::energy_state::electrostatic_potential_energy,
                       DOC(fiction_energy_state_electrostatic_potential_energy))
        .def_readwrite("degeneracy", &fiction::energy_state::degeneracy, DOC(fiction_energy_state_degeneracy));

    py::class_<fiction::energy_distribution>(m, "energy_distribution")
        .def(py::init<>())
        .def("get_nth_state", &fiction::energy_distribution::get_nth_state, py::arg("state_index"),
             DOC(fiction_energy_distribution_get_nth_state))
        .def("degeneracy_of_given_energy", &fiction::energy_distribution::degeneracy_of_given_energy, py::arg("energy"),
             DOC(fiction_energy_distribution_degeneracy_of_given_energy))
        .def("add_state", &fiction::energy_distribution::add_state, py::arg("energy"), py::arg("degeneracy"),
             DOC(fiction_energy_distribution_add_state))
        .def("size", &fiction::energy_distribution::size, DOC(fiction_energy_distribution_size))
        .def("empty", &fiction::energy_distribution::empty, DOC(fiction_energy_distribution_empty))
        .def("max_energy", &fiction::energy_distribution::max_energy, DOC(fiction_energy_distribution_max_energy))
        .def("min_energy", &fiction::energy_distribution::min_energy, DOC(fiction_energy_distribution_min_energy),
             DOC(fiction_energy_distribution_min_energy))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::energy_distribution<py_sidb_100_lattice>(m);
    detail::energy_distribution<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ENERGY_DISTRIBUTION_HPP
