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

void fanout_substitution(nanobind::module_& m);
void network_balancing(nanobind::module_& m);
void technology_mapping(nanobind::module_& m);

void register_network_transformation(nanobind::module_& m)
{
    fanout_substitution(m);
    network_balancing(m);
    technology_mapping(m);
}

}  // namespace pyfiction
