#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/minimum_energy.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <vector>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void minimum_energy_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "minimum_energy", [](const std::vector<Lyt>& layouts) -> double
        { return fiction::minimum_energy(layouts.cbegin(), layouts.cend()); }, py::arg("layouts"),
        DOC(fiction_minimum_energy));
}

}  // namespace detail

void minimum_energy(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::minimum_energy_impl<py_charge_distribution_surface_100>(m);
    detail::minimum_energy_impl<py_charge_distribution_surface_111>(m);
}

}  // namespace pyfiction
