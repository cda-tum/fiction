#include <pybind11/pybind11.h>

namespace pyfiction
{

// Forward declarations
void a_star(pybind11::module& m);
void distance(pybind11::module& m);
void enumerate_all_paths(pybind11::module& m);
void yen_k_shortest_paths(pybind11::module& m);

void register_path_finding(pybind11::module& m)
{
    distance(m);
    a_star(m);
    yen_k_shortest_paths(m);
    enumerate_all_paths(m);
}

}  // namespace pyfiction
