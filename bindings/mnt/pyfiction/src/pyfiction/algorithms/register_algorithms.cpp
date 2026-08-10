#include <nanobind/nanobind.h>

namespace pyfiction
{

// Forward declarations of registration functions for algorithm sub-categories
void register_path_finding(nanobind::module_& m);
void register_network_transformation(nanobind::module_& m);
void register_iter(nanobind::module_& m);
void register_properties(nanobind::module_& m);
void register_verification(nanobind::module_& m);
void register_simulation(nanobind::module_& m);
void register_physical_design(nanobind::module_& m);
// ... add others as we migrate them

void register_algorithms(nanobind::module_& m)
{
    register_path_finding(m);
    register_network_transformation(m);
    register_iter(m);
    register_properties(m);
    register_verification(m);
    register_simulation(m);
    register_physical_design(m);
    // ... call others
}

}  // namespace pyfiction
