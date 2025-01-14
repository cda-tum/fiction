//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_PHYSICAL_POPULATION_STABILITY_HPP
#define PYFICTION_PHYSICAL_POPULATION_STABILITY_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/physical_population_stability.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void physical_population_stability(pybind11::module& m, const std::string& lattice)
{
    namespace py = pybind11;

    py::class_<fiction::population_stability_information<Lyt>>(
        m, fmt::format("population_stability_information_{}", lattice).c_str(),
        DOC(fiction_population_stability_information))
        .def(py::init<>())
        .def_readwrite("critical_cell", &fiction::population_stability_information<Lyt>::critical_cell,
                       DOC(fiction_population_stability_information_critical_cell))
        .def_readwrite("transition_potentials", &fiction::population_stability_information<Lyt>::transition_potentials,
                       DOC(fiction_population_stability_information_transition_potentials))
        .def_readwrite("distance_corresponding_to_potential",
                       &fiction::population_stability_information<Lyt>::distance_corresponding_to_potential,
                       DOC(fiction_population_stability_information_distance_corresponding_to_potential))
        .def_readwrite("system_energy", &fiction::population_stability_information<Lyt>::system_energy,
                       DOC(fiction_population_stability_information_system_energy));

    m.def(fmt::format("physical_population_stability_{}", lattice).c_str(),
          &fiction::physical_population_stability<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::physical_population_stability_params{},
          DOC(fiction_physical_population_stability));
}

}  // namespace detail

inline void physical_population_stability(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::transition_type>(m, "transition_type", DOC(fiction_transition_type))
        .value("NEUTRAL_TO_NEGATIVE", fiction::transition_type::NEUTRAL_TO_NEGATIVE,
               DOC(fiction_transition_type_NEUTRAL_TO_NEGATIVE))
        .value("NEGATIVE_TO_NEUTRAL", fiction::transition_type::NEGATIVE_TO_NEUTRAL,
               DOC(fiction_transition_type_NEGATIVE_TO_NEUTRAL))
        .value("NEUTRAL_TO_POSITIVE", fiction::transition_type::NEUTRAL_TO_POSITIVE,
               DOC(fiction_transition_type_NEUTRAL_TO_POSITIVE))
        .value("POSITIVE_TO_NEUTRAL", fiction::transition_type::POSITIVE_TO_NEUTRAL,
               DOC(fiction_transition_type_POSITIVE_TO_NEUTRAL));

    /**
     * Parameters.
     */
    py::class_<fiction::physical_population_stability_params>(m, "physical_population_stability_params",
                                                              DOC(fiction_physical_population_stability_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::physical_population_stability_params::simulation_parameters,
                       DOC(fiction_physical_population_stability_params))
        .def_readwrite(
            "precision_for_distance_corresponding_to_potential",
            &fiction::physical_population_stability_params::precision_for_distance_corresponding_to_potential,
            DOC(fiction_physical_population_stability_params_precision_for_distance_corresponding_to_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::physical_population_stability<py_sidb_100_lattice>(m, "100");
    detail::physical_population_stability<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction

#endif  // PYFICTION_PHYSICAL_POPULATION_STABILITY_HPP
