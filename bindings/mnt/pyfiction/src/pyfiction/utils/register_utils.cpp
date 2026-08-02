#include <nanobind/nanobind.h>

namespace pyfiction
{

void layout_utils(nanobind::module_& m);
void name_utils(nanobind::module_& m);
void network_utils(nanobind::module_& m);
void placement_utils(nanobind::module_& m);
void routing_utils(nanobind::module_& m);
void truth_table_utils(nanobind::module_& m);
void version_info(nanobind::module_& m);

void register_utils(nanobind::module_& m)
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
