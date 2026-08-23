#include <nanobind/nanobind.h>

namespace pyfiction
{

// Forward declarations
void physical_design::apply_gate_library(nanobind::module_& m);
void physical_design::color_routing(nanobind::module_& m);
void design_sidb_gates(nanobind::module_& m);
void physical_design::exact(nanobind::module_& m);
void physical_design::graph_oriented_layout_design(nanobind::module_& m);
void physical_design::hexagonalization(nanobind::module_& m);
void physical_design::orthogonal(nanobind::module_& m);
void physical_design::post_layout_optimization(nanobind::module_& m);
void physical_design::wiring_reduction(nanobind::module_& m);

void register_physical_design(nanobind::module_& m)
{
    physical_design::exact(m);
    physical_design::orthogonal(m);
    physical_design::graph_oriented_layout_design(m);
    physical_design::apply_gate_library(m);
    physical_design::color_routing(m);
    physical_design::hexagonalization(m);
    physical_design::post_layout_optimization(m);
    physical_design::wiring_reduction(m);
    design_sidb_gates(m);
}

}  // namespace pyfiction
