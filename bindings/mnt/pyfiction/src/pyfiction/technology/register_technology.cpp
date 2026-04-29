#include <pybind11/pybind11.h>

namespace pyfiction
{

void area(pybind11::module& m);
void charge_distribution_surfaces(pybind11::module& m);
void sidb_charge_state(pybind11::module& m);
void sidb_defects(pybind11::module& m);
void sidb_lattices(pybind11::module& m);
void sidb_nm_distance(pybind11::module& m);
void sidb_nm_position(pybind11::module& m);

void register_technology(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
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
