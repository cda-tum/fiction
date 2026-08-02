#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

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
