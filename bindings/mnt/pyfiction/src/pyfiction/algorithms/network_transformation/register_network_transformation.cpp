#include <pybind11/pybind11.h>

namespace pyfiction
{

void fanout_substitution(pybind11::module& m);
void network_balancing(pybind11::module& m);
void technology_mapping(pybind11::module& m);

void register_network_transformation(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    fanout_substitution(m);
    network_balancing(m);
    technology_mapping(m);
}

}  // namespace pyfiction
