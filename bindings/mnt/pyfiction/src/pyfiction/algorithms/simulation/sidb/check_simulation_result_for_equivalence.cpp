#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void check_for_equivalence(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("check_simulation_results_for_equivalence", &fiction::check_simulation_results_for_equivalence<Lyt>,
          py::arg("result1"), py::arg("result2"), DOC(fiction_check_simulation_results_for_equivalence));
}

}  // namespace detail

void check_simulation_results_for_equivalence(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::check_for_equivalence<py_sidb_100_lattice>(m);
    detail::check_for_equivalence<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
