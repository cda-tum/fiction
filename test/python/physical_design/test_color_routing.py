# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import LayoutInputPort, arrangement, cartesian_gate_layout, coordinate, hexagonal_gate_layout
from mnt.pyfiction.physical_design import color_routing, color_routing_params


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_gate_layout((5, 5), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (5, 5), "2DDWave", "Layout"), id="hexagonal_gate_layout"
        ),
    ],
)
def test_routing(make_lyt):
    lyt = make_lyt()
    x1 = lyt.create_pi("x1", coordinate(0, 0))
    x2 = lyt.create_pi("x2", coordinate(0, 1))

    a = lyt.create_and(x1, x2, coordinate(2, 2))

    lyt.create_po(a, "f1", coordinate(4, 4))

    success = color_routing(lyt, [((0, 0), (2, 2), 0), ((0, 1), (2, 2), 1), ((2, 2), (4, 4), 0)])

    assert success


def test_crossings():
    lyt = cartesian_gate_layout((5, 3, 2), "2DDWave", "Layout")

    x1 = lyt.create_pi("x1", (0, 1))
    x2 = lyt.create_pi("x2", (3, 2))
    x3 = lyt.create_pi("x3", (2, 0))

    buf1 = lyt.create_buf(x3, (2, 1))
    lyt.create_buf(buf1, (2, 2))

    gate = lyt.create_and(x1, x2, (4, 2))
    lyt.disconnect(LayoutInputPort(gate.object, 0))
    lyt.disconnect(LayoutInputPort(gate.object, 1))

    params = color_routing_params()
    params.crossings = True
    params.path_limit = 1

    success = color_routing(lyt, [((0, 1), (4, 2), 0), ((3, 2), (4, 2), 1)], params=params)

    assert success
