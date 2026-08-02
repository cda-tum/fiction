#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/is_ground_state.hpp>

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
void is_ground_state_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("is_ground_state", &fiction::is_ground_state<Lyt>, py::arg("heuristic_results"),
          py::arg("exhaustive_results"), DOC(fiction_is_ground_state));
}

}  // namespace detail

void is_ground_state(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_ground_state_impl<py_sidb_100_lattice>(m);
    detail::is_ground_state_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
