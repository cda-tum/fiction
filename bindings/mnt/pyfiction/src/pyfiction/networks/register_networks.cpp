#include <pybind11/pybind11.h>

namespace pyfiction
{

void logic_networks(pybind11::module& m);
void truth_tables(pybind11::module& m);

void register_networks(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    logic_networks(m);
    truth_tables(m);
}

}  // namespace pyfiction
