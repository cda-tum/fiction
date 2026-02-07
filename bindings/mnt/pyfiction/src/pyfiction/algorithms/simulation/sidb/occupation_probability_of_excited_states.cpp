#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

void occupation_probability_of_excited_states_impl(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("occupation_probability_gate_based", &fiction::occupation_probability_gate_based,
          py::arg("energy_and_state_type"), py::arg("temperature"), DOC(fiction_occupation_probability_gate_based));

    m.def("occupation_probability_non_gate_based", &fiction::occupation_probability_non_gate_based,
          py::arg("calculate_energy_distribution"),
          py::arg("temperature"));  // TODO DOC(fiction_occupation_probability_non_gate_based)
}

}  // namespace detail

void occupation_probability_of_excited_states(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::occupation_probability_of_excited_states_impl(m);
}

}  // namespace pyfiction
