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

from mnt.pyfiction.layouts import CartesianGateLayout
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design.routing import a_star, place, route_path

if TYPE_CHECKING:
    from pathlib import Path


def test_empty_layout():
    lyt = CartesianGateLayout((4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)])

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile((x, y))


def test_empty_layout_a_star():
    lyt = CartesianGateLayout((4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, a_star(lyt, (0, 1), (4, 1)))

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile((x, y))


@pytest.mark.parametrize("path", [[], [(0, 0)], [(0, 0), (1, 0)], [(0, 0), (4, 0), (2, 0)]])
def test_route_path_rejects_invalid_paths(path: list[tuple[int, int]]) -> None:
    layout = CartesianGateLayout((2, 0))
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "y", (2, 0))
    before = (layout.num_gates(), layout.num_wires())
    with pytest.raises(ValueError, match=r"at least two|endpoints|within layout bounds"):
        route_path(layout, path)
    assert (layout.num_gates(), layout.num_wires()) == before


def test_place_uses_coordinates_and_validates_inputs(tmp_path: Path) -> None:
    path = tmp_path / "and.v"
    path.write_text("module top(a, b, y);\ninput a, b;\noutput y;\nassign y = a & b;\nendmodule\n")
    network = read_network(str(path))
    layout = CartesianGateLayout((2, 1))
    a, b = network.pis()
    left = place(layout, (0, 0), network, a)
    right = place(layout, (0, 1), network, b)
    gate = next(node for node in network.gates() if network.is_and(node))
    result = place(layout, (1, 1), network, gate, left, right)
    assert result == OffsetCoordinate(1, 1)
    assert layout.is_and(layout.get_node(result))
    with pytest.raises(IndexError, match="out of range"):
        place(layout, (2, 1), network, len(network))
    with pytest.raises(ValueError, match="wrong number"):
        place(layout, (2, 1), network, gate, left)
    with pytest.raises(ValueError, match="empty tile"):
        place(layout, (1, 1), network, gate, left, right)
    with pytest.raises(ValueError, match="existing gates"):
        place(layout, (2, 1), network, gate, left, (2, 0))
    with pytest.raises(ValueError, match="three-input function"):
        place(layout, (2, 1), network, gate, left, right, constant=False)
    assert layout.is_empty_tile((2, 1))


def test_place_inverter(tmp_path: Path) -> None:
    path = tmp_path / "not.v"
    path.write_text("module top(a, y);\ninput a;\noutput y;\nassign y = ~a;\nendmodule\n", encoding="utf-8")
    network = read_technology_network(str(path))
    layout = CartesianGateLayout((1, 0), "2DDWave")
    source = place(layout, (0, 0), network, network.pis()[0])
    gate = next(node for node in network.gates() if network.is_inv(node))
    tile = place(layout, (1, 0), network, gate, source)
    assert tile == OffsetCoordinate(1, 0)
    assert layout.is_inv(layout.get_node(tile))
    assert layout.fanins(tile) == [source]


@pytest.mark.parametrize("constant", [None, False, True])
def test_place_majority_inputs(tmp_path: Path, *, constant: bool | None) -> None:
    path = tmp_path / "maj.v"
    path.write_text(
        "module top(a, b, c, y);\ninput a, b, c;\noutput y;\nassign y = (a & b) | (a & c) | (b & c);\nendmodule\n"
    )
    network = read_network(str(path))
    layout = CartesianGateLayout((2, 2))
    inputs = [place(layout, (0, i), network, node) for i, node in enumerate(network.pis())]
    gate = next(node for node in network.gates() if network.is_maj(node))
    if constant is not None:
        with pytest.raises(ValueError, match="two coordinate inputs"):
            place(layout, (1, 1), network, gate, *inputs, constant=constant)
        assert layout.is_empty_tile((1, 1))
    selected = inputs if constant is None else inputs[:2]
    tile = place(layout, (1, 1), network, gate, *selected, constant=constant)
    node = layout.get_node(tile)
    if constant is None:
        assert layout.is_maj(node)
    elif constant:
        assert layout.is_or(node)
    else:
        assert layout.is_and(node)
