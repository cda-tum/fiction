#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void physical_population_stability_impl(nanobind::module_& m, const std::string& lattice)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::analysis::population_stability_information<Lyt>>(
        m, fmt::format("population_stability_information_{}", lattice).c_str(),
        DOC(fiction_population_stability_information))
        .def(py::init<>(), "Default constructor.")
        .def_rw("critical_cell",
                &fiction::sidb::simulation::analysis::population_stability_information<Lyt>::critical_cell,
                DOC(fiction_population_stability_information_critical_cell))
        .def_rw("transition_potentials",
                &fiction::sidb::simulation::analysis::population_stability_information<Lyt>::transition_potentials,
                DOC(fiction_population_stability_information_transition_potentials))
        .def_rw("distance_corresponding_to_potential",
                &fiction::sidb::simulation::analysis::population_stability_information<
                    Lyt>::distance_corresponding_to_potential,
                DOC(fiction_population_stability_information_distance_corresponding_to_potential))
        .def_rw("system_energy",
                &fiction::sidb::simulation::analysis::population_stability_information<Lyt>::system_energy,
                DOC(fiction_population_stability_information_system_energy));

    m.def(fmt::format("physical_population_stability_{}", lattice).c_str(),
          &fiction::sidb::simulation::analysis::physical_population_stability<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::sidb::simulation::analysis::physical_population_stability_params{},
          DOC(fiction_physical_population_stability));
}

}  // namespace detail

void physical_population_stability(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::sidb::simulation::analysis::transition_type>(m, "transition_type", DOC(fiction_transition_type))
        .value("NEUTRAL_TO_NEGATIVE", fiction::sidb::simulation::analysis::transition_type::NEUTRAL_TO_NEGATIVE,
               DOC(fiction_transition_type_NEUTRAL_TO_NEGATIVE))
        .value("NEGATIVE_TO_NEUTRAL", fiction::sidb::simulation::analysis::transition_type::NEGATIVE_TO_NEUTRAL,
               DOC(fiction_transition_type_NEGATIVE_TO_NEUTRAL))
        .value("NEUTRAL_TO_POSITIVE", fiction::sidb::simulation::analysis::transition_type::NEUTRAL_TO_POSITIVE,
               DOC(fiction_transition_type_NEUTRAL_TO_POSITIVE))
        .value("POSITIVE_TO_NEUTRAL", fiction::sidb::simulation::analysis::transition_type::POSITIVE_TO_NEUTRAL,
               DOC(fiction_transition_type_POSITIVE_TO_NEUTRAL));

    /**
     * Parameters.
     */
    py::class_<fiction::sidb::simulation::analysis::physical_population_stability_params>(
        m, "physical_population_stability_params", DOC(fiction_physical_population_stability_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters",
                &fiction::sidb::simulation::analysis::physical_population_stability_params::sim_params,
                DOC(fiction_physical_population_stability_params))
        .def_rw("precision_for_distance_corresponding_to_potential",
                &fiction::sidb::simulation::analysis::physical_population_stability_params::
                    precision_for_distance_corresponding_to_potential,
                DOC(fiction_physical_population_stability_params_precision_for_distance_corresponding_to_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::physical_population_stability_impl<py_sidb_100_lattice>(m, "100");
    detail::physical_population_stability_impl<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction
