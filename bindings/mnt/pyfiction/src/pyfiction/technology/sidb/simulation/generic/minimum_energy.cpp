#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/simulation/generic/minimum_energy.hpp>

#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void minimum_energy_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "minimum_energy", [](const std::vector<Lyt>& layouts) -> double
        { return fiction::sidb::simulation::generic::minimum_energy(layouts.cbegin(), layouts.cend()); },
        py::arg("layouts"), DOC(fiction_sidb_simulation_generic_minimum_energy));
}

}  // namespace detail

void minimum_energy(nanobind::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::minimum_energy_impl<py_charge_distribution_surface_100>(m);
    detail::minimum_energy_impl<py_charge_distribution_surface_111>(m);
}

}  // namespace pyfiction
