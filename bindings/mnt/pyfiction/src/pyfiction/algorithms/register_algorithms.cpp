#include <pybind11/pybind11.h>

namespace pyfiction
{

// Forward declarations of registration functions for algorithm sub-categories
void register_path_finding(pybind11::module& m);
// void register_physical_design(pybind11::module& m);
// ... add others as we migrate them

void register_algorithms(pybind11::module& m)
{
    register_path_finding(m);
    // ... call others
}

}  // namespace pyfiction
