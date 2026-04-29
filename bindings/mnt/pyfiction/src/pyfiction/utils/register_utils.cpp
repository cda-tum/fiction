#include <pybind11/pybind11.h>

namespace pyfiction
{

void layout_utils(pybind11::module& m);
void name_utils(pybind11::module& m);
void network_utils(pybind11::module& m);
void placement_utils(pybind11::module& m);
void routing_utils(pybind11::module& m);
void truth_table_utils(pybind11::module& m);
void version_info(pybind11::module& m);

void register_utils(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    layout_utils(m);
    routing_utils(m);
    name_utils(m);
    network_utils(m);
    placement_utils(m);
    truth_table_utils(m);
    version_info(m);
}

}  // namespace pyfiction
