#include <nanobind/nanobind.h>

namespace pyfiction
{

// Forward declarations
void apply_gate_library(nanobind::module_& m);
void color_routing(nanobind::module_& m);
void design_sidb_gates(nanobind::module_& m);
void exact(nanobind::module_& m);
void graph_oriented_layout_design(nanobind::module_& m);
void hexagonalization(nanobind::module_& m);
void orthogonal(nanobind::module_& m);
void post_layout_optimization(nanobind::module_& m);
void wiring_reduction(nanobind::module_& m);

void register_physical_design(nanobind::module_& m)
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
