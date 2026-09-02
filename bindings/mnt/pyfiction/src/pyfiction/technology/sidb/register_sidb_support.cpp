#include <nanobind/nanobind.h>

namespace pyfiction
{

void engine(nanobind::module_& m);
void simulation_parameters(nanobind::module_& m);
void result(nanobind::module_& m);

void register_sidb_support(nanobind::module_& m)
{
    engine(m);
    simulation_parameters(m);
    result(m);
}

}  // namespace pyfiction
