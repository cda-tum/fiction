from __future__ import annotations

from mnt.pyfiction import hexagonal_layout


def test_coordinate_iteration():
    layout = hexagonal_layout((9, 9, 1))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    # comparing the whole set rather than each element also catches a missing neighbor
    neighbors = {(t.x, t.y, t.z) for t in layout.adjacent_coordinates((2, 2))}
    assert neighbors == {(2, 1, 0), (3, 1, 0), (3, 2, 0), (3, 3, 0), (2, 3, 0), (1, 2, 0)}
