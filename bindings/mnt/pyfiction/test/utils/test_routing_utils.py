from __future__ import annotations

from mnt.pyfiction import a_star, cartesian_gate_layout, route_path


def test_empty_layout():
    lyt = cartesian_gate_layout((4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)])

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile((x, y))


def test_empty_layout_a_star():
    lyt = cartesian_gate_layout((4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, a_star(lyt, (0, 1), (4, 1)))

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile((x, y))
