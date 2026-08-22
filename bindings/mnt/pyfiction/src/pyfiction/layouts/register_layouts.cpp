#include <nanobind/nanobind.h>

namespace pyfiction
{

void layouts::cartesian_layout(nanobind::module_& m);
void cell_level_layouts(nanobind::module_& m);
void clocked_layouts(nanobind::module_& m);
void coordinate_utility(nanobind::module_& m);
void cube_coordinate(nanobind::module_& m);
void gate_level_layouts(nanobind::module_& m);
void layouts::hexagonal_layout(nanobind::module_& m);
void obstruction_layouts(nanobind::module_& m);
void offset_coordinate(nanobind::module_& m);
void layouts::shifted_cartesian_layout(nanobind::module_& m);
void siqad_coordinate(nanobind::module_& m);

void register_layouts(nanobind::module_& m)
{
    offset_coordinate(m);
    cube_coordinate(m);
    siqad_coordinate(m);
    coordinate_utility(m);

    layouts::cartesian_layout(m);
    layouts::shifted_cartesian_layout(m);
    layouts::hexagonal_layout(m);
    clocked_layouts(m);
    gate_level_layouts(m);
    cell_level_layouts(m);
    obstruction_layouts(m);
}

}  // namespace pyfiction
