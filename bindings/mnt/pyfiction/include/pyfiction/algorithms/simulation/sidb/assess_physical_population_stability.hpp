//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
#define PYFICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/assess_physical_population_stability.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void assess_physical_population_stability(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    m.def("assess_physical_population_stability", &fiction::assess_physical_population_stability<Lyt>, "lyt"_a,
          "params"_a = fiction::assess_physical_population_stability_params{},
          DOC(fiction_assess_physical_population_stability));
}

}  // namespace detail

inline void assess_physical_population_stability(pybind11::module& m)
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

    py::class_<fiction::population_stability_information<fiction::offset::ucoord_t>>(
        m, "population_stability_information", DOC(fiction_population_stability_information))
        .def(py::init<>())
        .def_readwrite("critical_cell",
                       &fiction::population_stability_information<fiction::offset::ucoord_t>::critical_cell,
                       DOC(fiction_population_stability_information_critical_cell))
        .def_readwrite("transition_from_to",
                       &fiction::population_stability_information<fiction::offset::ucoord_t>::transition_from_to,
                       DOC(fiction_population_stability_information_transition_from_to))
        .def_readwrite("minimum_potential_difference_to_transition",
                       &fiction::population_stability_information<
                           fiction::offset::ucoord_t>::minimum_potential_difference_to_transition,
                       DOC(fiction_population_stability_information_minimum_potential_difference_to_transition))
        .def_readwrite(
            "distance_corresponding_to_potential",
            &fiction::population_stability_information<fiction::offset::ucoord_t>::distance_corresponding_to_potential,
            DOC(fiction_population_stability_information_distance_corresponding_to_potential))
        .def_readwrite("system_energy",
                       &fiction::population_stability_information<fiction::offset::ucoord_t>::system_energy,
                       DOC(fiction_population_stability_information_system_energy));

    /**
     * Parameters.
     */
    py::class_<fiction::assess_physical_population_stability_params>(
        m, "assess_physical_population_stability_params", DOC(fiction_assess_physical_population_stability_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters",
                       &fiction::assess_physical_population_stability_params::simulation_parameters,
                       DOC(fiction_assess_physical_population_stability_params))
        .def_readwrite(
            "precision_for_distance_corresponding_to_potential",
            &fiction::assess_physical_population_stability_params::precision_for_distance_corresponding_to_potential,
            DOC(fiction_assess_physical_population_stability_params_precision_for_distance_corresponding_to_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::assess_physical_population_stability<py_sidb_100_lattice>(m);
    detail::assess_physical_population_stability<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
