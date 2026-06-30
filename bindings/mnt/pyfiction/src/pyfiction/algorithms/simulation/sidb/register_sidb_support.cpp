#include <pybind11/pybind11.h>

namespace pyfiction
{

void sidb_simulation_engine(pybind11::module& m);
void sidb_simulation_parameters(pybind11::module& m);
void sidb_simulation_result(pybind11::module& m);

void register_sidb_support(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    sidb_simulation_engine(m);
    sidb_simulation_parameters(m);
    sidb_simulation_result(m);
}

}  // namespace pyfiction
