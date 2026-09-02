#include <nanobind/nanobind.h>

namespace pyfiction
{

void offset_coordinate(nanobind::module_& m);
void cube_coordinate(nanobind::module_& m);
void siqad_coordinate(nanobind::module_& m);
void coordinate_utility(nanobind::module_& m);
void cartesian_layout(nanobind::module_& m);
void shifted_cartesian_layout(nanobind::module_& m);
void hexagonal_layout(nanobind::module_& m);
void clocked_layout(nanobind::module_& m);
void gate_level_layout(nanobind::module_& m);
void cell_level_layout(nanobind::module_& m);
void obstruction_layout(nanobind::module_& m);
void layout_utils(nanobind::module_& m);

void register_layouts(nanobind::module_& m)
{
    offset_coordinate(m);
    cube_coordinate(m);
    siqad_coordinate(m);
    coordinate_utility(m);
    cartesian_layout(m);
    shifted_cartesian_layout(m);
    hexagonal_layout(m);
    clocked_layout(m);
    gate_level_layout(m);
    cell_level_layout(m);
    obstruction_layout(m);
    layout_utils(m);
}

}  // namespace pyfiction
