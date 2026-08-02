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

// Forward declarations
void a_star(nanobind::module_& m);
void distance(nanobind::module_& m);
void enumerate_all_paths(nanobind::module_& m);
void yen_k_shortest_paths(nanobind::module_& m);

void register_path_finding(nanobind::module_& m)
{
    distance(m);
    a_star(m);
    yen_k_shortest_paths(m);
    enumerate_all_paths(m);
}

}  // namespace pyfiction
