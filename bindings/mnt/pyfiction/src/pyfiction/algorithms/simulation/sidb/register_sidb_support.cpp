#include <nanobind/nanobind.h>

namespace pyfiction
{

void sidb::simulation::engine(nanobind::module_& m);
void sidb::model::simulation_parameters(nanobind::module_& m);
void sidb::simulation::result(nanobind::module_& m);

void register_sidb_support(nanobind::module_& m)
{
    sidb::simulation::engine(m);
    sidb::model::simulation_parameters(m);
    sidb::simulation::result(m);
}

}  // namespace pyfiction
