#include <nanobind/nanobind.h>

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
