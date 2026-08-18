from mnt.pyfiction import cartesian_layout


def test_coordinate_iteration():
    layout = cartesian_layout((9, 9, 1))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]
