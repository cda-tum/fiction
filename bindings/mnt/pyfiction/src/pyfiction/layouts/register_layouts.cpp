#include <pybind11/pybind11.h>

namespace pyfiction
{

void cartesian_layout(pybind11::module& m);
void cell_level_layouts(pybind11::module& m);
void clocked_layouts(pybind11::module& m);
void coordinate_utility(pybind11::module& m);
void cube_coordinate(pybind11::module& m);
void gate_level_layouts(pybind11::module& m);
void hexagonal_layout(pybind11::module& m);
void obstruction_layouts(pybind11::module& m);
void offset_coordinate(pybind11::module& m);
void shifted_cartesian_layout(pybind11::module& m);
void siqad_coordinate(pybind11::module& m);

void register_layouts(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    offset_coordinate(m);
    cube_coordinate(m);
    siqad_coordinate(m);
    coordinate_utility(m);

    cartesian_layout(m);
    shifted_cartesian_layout(m);
    hexagonal_layout(m);
    clocked_layouts(m);
    gate_level_layouts(m);
    cell_level_layouts(m);
    obstruction_layouts(m);
}

}  // namespace pyfiction
