# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.inml import inml_layout
from mnt.pyfiction.layouts import cartesian_gate_layout, hexagonal_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.layouts.coords import offset_coordinate
from mnt.pyfiction.networks.io import read_technology_network
from mnt.pyfiction.physical_design import orthogonal, orthogonal_params
from mnt.pyfiction.qca import qca_layout

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: cartesian_gate_layout(offset_coordinate(2, 2, 0), "2DDWave", "Layout"), id="cartesian_gate_layout"
        ),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(offset_coordinate(2, 2, 0), "2DDWave", "Layout"),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(offset_coordinate(2, 2, 0), "2DDWave", "Layout"), id="hexagonal_gate_layout"
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


@pytest.mark.parametrize("verilog", ["mux21.v", "xnor2.v", "xor2.v", "FA.v"])
def test_initialize_gate_level_with_ortho_bounding_box(resources_dir: Path, verilog: str) -> None:
    """Orthogonal placement fills the layout's declared extent."""
    network = read_technology_network(str(resources_dir / verilog))
    layout = orthogonal(network, orthogonal_params())
    min_coord, max_coord = layout.bounding_box_2d()

    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(layout.x(), layout.y())


def test_update_gate_level_bounding_box() -> None:
    """Clearing and moving occupied boundary tiles updates both bounding-box axes."""
    layout = cartesian_gate_layout(offset_coordinate(8, 8), "2DDWave")
    source = layout.create_pi("a", offset_coordinate(0, 0))
    wire = layout.create_buf(source, offset_coordinate(4, 6))
    edge = layout.create_buf(wire, offset_coordinate(4, 7))
    layout.create_po(edge, "out", offset_coordinate(5, 7))
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 7)

    layout.clear_tile(offset_coordinate(4, 7))
    layout.move_node(layout.get_node(offset_coordinate(5, 7)), offset_coordinate(5, 6), [wire])
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(5, 6)

    layout.move_node(layout.get_node(offset_coordinate(5, 6)), offset_coordinate(4, 7), [wire])
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(4, 7)


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: qca_layout(offset_coordinate(2, 2, 0), "2DDWave", "Layout"), id="qca_layout"),
        pytest.param(lambda: inml_layout(offset_coordinate(2, 2, 0), "2DDWave", "Layout"), id="inml_layout"),
    ],
)
def test_bounding_box_around_cell_level_layout(make_layout):
    layout = make_layout()
    min_coord, max_coord = layout.bounding_box_2d()
    assert min_coord == layout.coord(0, 0)
    assert max_coord == layout.coord(0, 0)
    assert max_coord.x - min_coord.x == 0
    assert max_coord.y - min_coord.y == 0
