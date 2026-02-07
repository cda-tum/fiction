#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/is_ground_state.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_ground_state_impl(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("is_ground_state", &fiction::is_ground_state<Lyt>, py::arg("heuristic_results"),
          py::arg("exhaustive_results"), DOC(fiction_is_ground_state));
}

}  // namespace detail

void is_ground_state(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_ground_state_impl<py_sidb_100_lattice>(m);
    detail::is_ground_state_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
