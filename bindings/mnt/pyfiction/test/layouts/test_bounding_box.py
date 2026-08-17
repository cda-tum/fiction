import os

import pytest

from mnt.pyfiction import (
    cartesian_gate_layout,
    cartesian_obstruction_layout,
    hexagonal_gate_layout,
    hexagonal_obstruction_layout,
    inml_layout,
    orthogonal,
    orthogonal_params,
    qca_layout,
    read_technology_network,
    shifted_cartesian_gate_layout,
    shifted_cartesian_obstruction_layout,
    sidb_layout,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
        ),
        pytest.param(lambda: hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
        pytest.param(
            lambda: cartesian_obstruction_layout(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="shifted_cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: hexagonal_obstruction_layout(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="hexagonal_obstruction_layout",
        ),
    ],
)
def test_bounding_box_around_an_empty_gate_level_layout(make_layout):
    layout = make_layout()
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(0, 0)
    assert max_coord.x - min_coord.x == 0
    assert max_coord.y - min_coord.y == 0


def test_initialize_gate_level_with_ortho_bounding_box():
    network = read_technology_network(os.path.join(dir_path + "/../resources/mux21.v"))
    params = orthogonal_params()
    layout = orthogonal(network, params)
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 7)
    assert max_coord.x - min_coord.x == 5
    assert max_coord.y - min_coord.y == 7

    network = read_technology_network(os.path.join(dir_path + "/../resources/xnor2.v"))
    layout = orthogonal(network, params)
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 8)
    assert max_coord.x - min_coord.x == 5
    assert max_coord.y - min_coord.y == 8

    network = read_technology_network(os.path.join(dir_path + "/../resources/xor2.v"))
    layout = orthogonal(network, params)
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(4, 7)
    assert max_coord.x - min_coord.x == 4
    assert max_coord.y - min_coord.y == 7

    network = read_technology_network(os.path.join(dir_path + "/../resources/FA.v"))
    layout = orthogonal(network, params)
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(7, 11)
    assert max_coord.x - min_coord.x == 7
    assert max_coord.y - min_coord.y == 11


def test_update_gate_level_bounding_box():
    network = read_technology_network(os.path.join(dir_path + "/../resources/mux21.v"))
    params = orthogonal_params()
    layout = orthogonal(network, params)
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 7)
    assert max_coord.x - min_coord.x == 5
    assert max_coord.y - min_coord.y == 7

    layout.clear_tile((4, 7))
    layout.move_node(layout.get_node((5, 7)), (5, 6), [layout.make_signal(layout.get_node((4, 6)))])
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 6)
    assert max_coord.x - min_coord.x == 5
    assert max_coord.y - min_coord.y == 6

    layout.move_node(layout.get_node((5, 6)), (4, 7), [layout.make_signal(layout.get_node((4, 6)))])
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(4, 7)
    assert max_coord.x - min_coord.x == 4
    assert max_coord.y - min_coord.y == 7


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: qca_layout((2, 2, 0), "2DDWave", "Layout"), id="qca_layout"),
        pytest.param(lambda: inml_layout((2, 2, 0), "2DDWave", "Layout"), id="inml_layout"),
        pytest.param(lambda: sidb_layout((2, 2, 0), "2DDWave", "Layout"), id="sidb_layout"),
    ],
)
def test_bounding_box_around_cell_level_layout(make_layout):
    layout = make_layout()
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(0, 0)
    assert max_coord.x - min_coord.x == 0
    assert max_coord.y - min_coord.y == 0
