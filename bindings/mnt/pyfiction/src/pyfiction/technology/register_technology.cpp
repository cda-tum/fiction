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

void area(nanobind::module_& m);
void charge_distribution_surfaces(nanobind::module_& m);
void sidb_charge_state(nanobind::module_& m);
void sidb_defects(nanobind::module_& m);
void sidb_lattices(nanobind::module_& m);
void sidb_nm_distance(nanobind::module_& m);
void sidb_nm_position(nanobind::module_& m);

void register_technology(nanobind::module_& m)
{
    area(m);
    sidb_defects(m);
    sidb_charge_state(m);
    sidb_nm_position(m);
    sidb_nm_distance(m);
    sidb_lattices(m);
    charge_distribution_surfaces(m);
}

}  // namespace pyfiction
