#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void calculate_energy_and_state_type_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

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

void calculate_energy_and_state_type(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::calculate_energy_and_state_type_impl<py_sidb_100_lattice>(m);
    detail::calculate_energy_and_state_type_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction
