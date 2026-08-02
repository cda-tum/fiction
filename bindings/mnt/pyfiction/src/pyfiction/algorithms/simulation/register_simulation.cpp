#include <nanobind/nanobind.h>

namespace pyfiction
{

void logic_simulation(nanobind::module_& m);
void register_sidb_simulation(nanobind::module_& m);

void register_simulation(nanobind::module_& m)
{
    logic_simulation(m);
    register_sidb_simulation(m);
}

}  // namespace pyfiction
