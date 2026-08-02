#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp>

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

void occupation_probability_of_excited_states_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("occupation_probability_gate_based", &fiction::occupation_probability_gate_based,
          py::arg("energy_and_state_type"), py::arg("temperature"), DOC(fiction_occupation_probability_gate_based));

    m.def("occupation_probability_non_gate_based", &fiction::occupation_probability_non_gate_based,
          py::arg("calculate_energy_distribution"),
          py::arg("temperature"));  // TODO DOC(fiction_occupation_probability_non_gate_based)
}

}  // namespace detail

void occupation_probability_of_excited_states(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::occupation_probability_of_excited_states_impl(m);
}

}  // namespace pyfiction
