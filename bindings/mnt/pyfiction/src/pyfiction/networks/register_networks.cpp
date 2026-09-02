#include <nanobind/nanobind.h>

namespace pyfiction
{

void logic_networks(nanobind::module_& m);
void dynamic_truth_table(nanobind::module_& m);
void name_utils(nanobind::module_& m);
void network_utils(nanobind::module_& m);
void logic_simulation(nanobind::module_& m);

void register_networks(nanobind::module_& m)
{
    logic_networks(m);
    dynamic_truth_table(m);
    name_utils(m);
    network_utils(m);
    logic_simulation(m);
}

}  // namespace pyfiction
