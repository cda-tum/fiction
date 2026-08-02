#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp>

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
void check_for_equivalence_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("check_simulation_results_for_equivalence", &fiction::check_simulation_results_for_equivalence<Lyt>,
          py::arg("result1"), py::arg("result2"), DOC(fiction_check_simulation_results_for_equivalence));
}

}  // namespace detail

void check_simulation_results_for_equivalence(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::check_for_equivalence_impl<py_sidb_100_lattice>(m);
    detail::check_for_equivalence_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
