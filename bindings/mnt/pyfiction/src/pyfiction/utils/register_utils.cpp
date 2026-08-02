#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

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
