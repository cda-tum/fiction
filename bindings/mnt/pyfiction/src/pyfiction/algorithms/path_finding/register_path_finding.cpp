#include <nanobind/nanobind.h>

namespace pyfiction
{

// Forward declarations
void physical_design::path_finding::a_star(nanobind::module_& m);
void distance(nanobind::module_& m);
void physical_design::path_finding::enumerate_all_paths(nanobind::module_& m);
void physical_design::path_finding::yen_k_shortest_paths(nanobind::module_& m);

void register_path_finding(nanobind::module_& m)
{
    distance(m);
    physical_design::path_finding::a_star(m);
    physical_design::path_finding::yen_k_shortest_paths(m);
    physical_design::path_finding::enumerate_all_paths(m);
}

}  // namespace pyfiction
