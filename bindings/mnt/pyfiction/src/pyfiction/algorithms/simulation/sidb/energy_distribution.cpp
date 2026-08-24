#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/analysis/energy_distribution.hpp>

#include <cstdint>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void energy_distribution_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("calculate_energy_distribution", &fiction::sidb::simulation::analysis::calculate_energy_distribution<Lyt>,
          py::arg("charge_distributions"), DOC(fiction_energy_distribution));
}

}  // namespace detail

void sidb::simulation::analysis::energy_distribution(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::sidb::simulation::analysis::energy_state>(m, "energy_state")
        .def(py::init<double, uint64_t>(), py::arg("electrostatic_potential_energy"), py::arg("degeneracy"),
             DOC(fiction_energy_state))
        .def_rw("electrostatic_potential_energy",
                &fiction::sidb::simulation::analysis::energy_state::electrostatic_potential_energy,
                DOC(fiction_energy_state_electrostatic_potential_energy))
        .def_rw("degeneracy", &fiction::sidb::simulation::analysis::energy_state::degeneracy,
                DOC(fiction_energy_state_degeneracy));

    py::class_<fiction::sidb::simulation::analysis::energy_distribution>(m, "energy_distribution")
        .def(py::init<>(), "Default constructor.")
        .def("get_nth_state", &fiction::sidb::simulation::analysis::energy_distribution::get_nth_state,
             py::arg("state_index"), DOC(fiction_energy_distribution_get_nth_state))
        .def("degeneracy", &fiction::sidb::simulation::analysis::energy_distribution::degeneracy, py::arg("energy"),
             DOC(fiction_energy_distribution_degeneracy))
        .def("add_energy_state", &fiction::sidb::simulation::analysis::energy_distribution::add_energy_state,
             py::arg("state"), DOC(fiction_energy_distribution_add_energy_state))
        .def("size", &fiction::sidb::simulation::analysis::energy_distribution::size,
             DOC(fiction_energy_distribution_size))
        .def("empty", &fiction::sidb::simulation::analysis::energy_distribution::empty,
             DOC(fiction_energy_distribution_empty))
        .def("max_energy", &fiction::sidb::simulation::analysis::energy_distribution::max_energy,
             DOC(fiction_energy_distribution_max_energy))
        .def("min_energy", &fiction::sidb::simulation::analysis::energy_distribution::min_energy,
             DOC(fiction_energy_distribution_min_energy))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::energy_distribution_impl<py_sidb_100_lattice>(m);
    detail::energy_distribution_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
