#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/generic/is_ground_state.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_ground_state_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("is_ground_state", &fiction::sidb::simulation::generic::is_ground_state<Lyt>, py::arg("heuristic_results"),
          py::arg("exhaustive_results"), DOC(fiction_is_ground_state));
}

}  // namespace detail

void sidb::simulation::generic::is_ground_state(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_ground_state_impl<py_sidb_100_lattice>(m);
    detail::is_ground_state_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
