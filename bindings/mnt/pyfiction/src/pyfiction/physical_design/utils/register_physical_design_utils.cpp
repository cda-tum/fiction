#include <nanobind/nanobind.h>

namespace pyfiction
{

void placement_utils(nanobind::module_& m);
void routing_utils(nanobind::module_& m);

void register_physical_design_utils(nanobind::module_& m)
{
    routing_utils(m);
    placement_utils(m);
}

}  // namespace pyfiction
