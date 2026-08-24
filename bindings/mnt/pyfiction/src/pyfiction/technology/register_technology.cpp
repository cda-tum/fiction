#include <nanobind/nanobind.h>

namespace pyfiction
{

void fcn::area(nanobind::module_& m);
void charge_distribution_surfaces(nanobind::module_& m);
void sidb_charge_state(nanobind::module_& m);
void sidb_defects(nanobind::module_& m);
void sidb_lattices(nanobind::module_& m);
void sidb_nm_distance(nanobind::module_& m);
void sidb_nm_position(nanobind::module_& m);

void register_technology(nanobind::module_& m)
{
    fcn::area(m);
    sidb_defects(m);
    sidb_charge_state(m);
    sidb_nm_position(m);
    sidb_nm_distance(m);
    sidb_lattices(m);
    charge_distribution_surfaces(m);
}

}  // namespace pyfiction
