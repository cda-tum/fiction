#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void exhaustive_ground_state_simulation_impl(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("exhaustive_ground_state_simulation", &fiction::exhaustive_ground_state_simulation<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::sidb_simulation_parameters{}, DOC(fiction_exhaustive_ground_state_simulation));
}

}  // namespace detail

void exhaustive_ground_state_simulation(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::exhaustive_ground_state_simulation_impl<py_sidb_100_lattice>(m);
    detail::exhaustive_ground_state_simulation_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
