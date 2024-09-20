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
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void assess_physical_population_stability(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::population_stability_information<Lyt>>(
        m, fmt::format("population_stability_information{}", lattice).c_str(),
        DOC(fiction_population_stability_information))
        .def(py::init<>())
        .def_readwrite("critical_cell", &fiction::population_stability_information<Lyt>::critical_cell,
                       DOC(fiction_population_stability_information_critical_cell))
        .def_readwrite("transition_from_to_with_cell_and_required_pot",
                       &fiction::population_stability_information<Lyt>::transition_from_to_with_cell_and_required_pot,
                       DOC(fiction_population_stability_information_transition_from_to_with_cell_and_required_pot))
        .def_readwrite("distance_corresponding_to_potential",
                       &fiction::population_stability_information<Lyt>::distance_corresponding_to_potential,
                       DOC(fiction_population_stability_information_distance_corresponding_to_potential))
        .def_readwrite("system_energy", &fiction::population_stability_information<Lyt>::system_energy,
                       DOC(fiction_population_stability_information_system_energy));

    // TODO add docu.
    m.def(fmt::format("assess_physical_population_stability{}", lattice).c_str(),
          &fiction::assess_physical_population_stability<Lyt>, "lyt"_a,
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

    detail::assess_physical_population_stability<py_sidb_100_lattice>(m, "_100");
    detail::assess_physical_population_stability<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_ASSESS_PHYSICAL_POPULATION_STABILITY_HPP
