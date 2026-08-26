#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
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
void exhaustive_ground_state_simulation_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("exhaustive_ground_state_simulation",
          &fiction::sidb::simulation::engines::exhaustive_ground_state_simulation<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::sidb::model::simulation_parameters{},
          DOC(fiction_sidb_simulation_engines_exhaustive_ground_state_simulation));
}

}  // namespace detail

void exhaustive_ground_state_simulation(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::exhaustive_ground_state_simulation_impl<py_sidb_100_lattice>(m);
    detail::exhaustive_ground_state_simulation_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
