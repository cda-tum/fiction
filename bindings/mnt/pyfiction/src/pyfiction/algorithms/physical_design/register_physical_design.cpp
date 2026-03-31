#include <pybind11/pybind11.h>

namespace pyfiction
{

// Forward declarations
void apply_gate_library(pybind11::module& m);
void color_routing(pybind11::module& m);
void design_sidb_gates(pybind11::module& m);
void exact(pybind11::module& m);
void graph_oriented_layout_design(pybind11::module& m);
void hexagonalization(pybind11::module& m);
void orthogonal(pybind11::module& m);
void post_layout_optimization(pybind11::module& m);
void wiring_reduction(pybind11::module& m);

void register_physical_design(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    exact(m);
    orthogonal(m);
    graph_oriented_layout_design(m);
    apply_gate_library(m);
    color_routing(m);
    hexagonalization(m);
    post_layout_optimization(m);
    wiring_reduction(m);
    design_sidb_gates(m);
}

}  // namespace pyfiction
