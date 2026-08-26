#include <nanobind/nanobind.h>

namespace pyfiction
{

void logic_simulation(nanobind::module_& m);
void name_utils(nanobind::module_& m);
void network_utils(nanobind::module_& m);
void truth_table_utils(nanobind::module_& m);

void register_networks_utils(nanobind::module_& m)
{
    name_utils(m);
    network_utils(m);
    truth_table_utils(m);
    logic_simulation(m);
}

}  // namespace pyfiction
