# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import CartesianGateLayout, HexagonalGateLayout
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.physical_design import color_routing, color_routing_params


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: CartesianGateLayout((4, 4), "2DDWave", "Layout"), id="CartesianGateLayout"),
        pytest.param(lambda: HexagonalGateLayout((4, 4), "2DDWave", "Layout"), id="HexagonalGateLayout"),
    ],
)
def test_routing(make_lyt):
    lyt = make_lyt()
    x1 = lyt.create_pi("x1", OffsetCoordinate(0, 0))
    x2 = lyt.create_pi("x2", OffsetCoordinate(0, 1))

    a = lyt.create_and(x1, x2, OffsetCoordinate(2, 2))

    lyt.create_po(a, "f1", OffsetCoordinate(4, 4))

    success = color_routing(lyt, [((0, 0), (2, 2)), ((0, 1), (2, 2)), ((2, 2), (4, 4))])

    assert success


def test_crossings():
    lyt = CartesianGateLayout((4, 2, 1), "2DDWave", "Layout")

    x1 = lyt.create_pi("x1", (0, 1))
    x2 = lyt.create_pi("x2", (3, 2))
    x3 = lyt.create_pi("x3", (2, 0))

    buf1 = lyt.create_buf(x3, (2, 1))
    lyt.create_buf(buf1, (2, 2))

    lyt.create_and(x1, x2, (4, 2))
    lyt.move_node(lyt.get_node((4, 2)), (4, 2))

    params = color_routing_params()
    params.crossings = True
    params.path_limit = 1

    success = color_routing(lyt, [((0, 1), (4, 2)), ((3, 2), (4, 2))], params=params)

    assert success
