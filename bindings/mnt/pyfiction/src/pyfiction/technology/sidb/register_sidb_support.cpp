#include <nanobind/nanobind.h>

namespace pyfiction
{

void sidb_simulation_engine(nanobind::module_& m);
void sidb_simulation_parameters(nanobind::module_& m);
void sidb_simulation_result(nanobind::module_& m);

void register_sidb_support(nanobind::module_& m)
{
    sidb_simulation_engine(m);
    sidb_simulation_parameters(m);
    sidb_simulation_result(m);
}

}  // namespace pyfiction
