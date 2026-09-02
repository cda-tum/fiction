#include <nanobind/nanobind.h>

namespace pyfiction
{

void lattice(nanobind::module_& m);
void charge_distribution_surface(nanobind::module_& m);

void register_sidb_surfaces(nanobind::module_& m)
{
    lattice(m);
    charge_distribution_surface(m);
}

}  // namespace pyfiction
