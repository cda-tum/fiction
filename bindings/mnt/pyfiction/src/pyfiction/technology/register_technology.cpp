#include <nanobind/nanobind.h>

namespace pyfiction
{

void area(nanobind::module_& m);
void charge_distribution_surface(nanobind::module_& m);
void charge_state(nanobind::module_& m);
void defect(nanobind::module_& m);
void lattice(nanobind::module_& m);
void nm_distance(nanobind::module_& m);
void nm_position(nanobind::module_& m);

void register_technology(nanobind::module_& m)
{
    area(m);
    defect(m);
    charge_state(m);
    nm_position(m);
    nm_distance(m);
    lattice(m);
    charge_distribution_surface(m);
}

}  // namespace pyfiction
