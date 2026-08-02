#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void exhaustive_ground_state_simulation_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("exhaustive_ground_state_simulation", &fiction::exhaustive_ground_state_simulation<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::sidb_simulation_parameters{}, DOC(fiction_exhaustive_ground_state_simulation));
}

}  // namespace detail

void exhaustive_ground_state_simulation(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::exhaustive_ground_state_simulation_impl<py_sidb_100_lattice>(m);
    detail::exhaustive_ground_state_simulation_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
