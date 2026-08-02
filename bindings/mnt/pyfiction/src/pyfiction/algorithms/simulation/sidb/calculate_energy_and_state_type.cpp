#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp>

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
void calculate_energy_and_state_type_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("calculate_energy_and_state_type_with_kinks_accepted",
          &fiction::calculate_energy_and_state_type_with_kinks_accepted<Lyt, py_tt>, py::arg("energy_distribution"),
          py::arg("valid_charge_distributions"), py::arg("output_bdl_pairs"), py::arg("spec"), py::arg("input_index"),
          DOC(fiction_calculate_energy_and_state_type_with_kinks_accepted));

    m.def("calculate_energy_and_state_type_with_kinks_rejected",
          &fiction::calculate_energy_and_state_type_with_kinks_rejected<Lyt, py_tt>, py::arg("energy_distribution"),
          py::arg("valid_charge_distributions"), py::arg("spec"), py::arg("input_index"), py::arg("input_bdl_wires"),
          py::arg("output_bdl_wires"), DOC(fiction_calculate_energy_and_state_type_with_kinks_rejected));
}

}  // namespace detail

void calculate_energy_and_state_type(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::calculate_energy_and_state_type_impl<py_sidb_100_lattice>(m);
    detail::calculate_energy_and_state_type_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
