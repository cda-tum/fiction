#include <pybind11/pybind11.h>

namespace pyfiction
{

void logic_simulation(pybind11::module& m);
void register_sidb_simulation(pybind11::module& m);

void register_simulation(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    logic_simulation(m);
    register_sidb_simulation(m);
}

}  // namespace pyfiction
