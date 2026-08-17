import pytest

from mnt.pyfiction import sidb_100_lattice, sidb_111_lattice, sidb_nm_distance_100, sidb_nm_distance_111


def test_sidb_nm_distance_100_lattice():
    layout_one = sidb_100_lattice((10, 10))

    # Replace the placeholders with actual values or function calls
    assert sidb_nm_distance_100(layout_one, (0, 0), (0, 0)) == 0
    assert sidb_nm_distance_100(layout_one, (1, 0), (1, 0)) == 0
    # (1, 1) is off-axis: pins down the source == target short-circuit that avoids
    # architecture-dependent float noise (exact 0.0 on x86, ~8.88e-19 on ARM) without it.
    assert sidb_nm_distance_100(layout_one, (1, 1), (1, 1)) == 0

    # Replace the placeholders with actual values or function calls
    assert sidb_nm_distance_100(layout_one, (0, 0), (2, 0)) == pytest.approx(2 * 0.384)
    assert sidb_nm_distance_100(layout_one, (0, 0), (0, 1)) == pytest.approx(0.225)


def test_sidb_nm_distance_111_lattice():
    layout_one = sidb_111_lattice((10, 10))

    # Replace the placeholders with actual values or function calls
    assert sidb_nm_distance_111(layout_one, (0, 0), (0, 0)) == 0
    assert sidb_nm_distance_111(layout_one, (1, 0), (1, 0)) == 0
    # (1, 1) is off-axis: pins down the source == target short-circuit that avoids
    # architecture-dependent float noise (exact 0.0 on x86, ~8.88e-19 on ARM) without it.
    assert sidb_nm_distance_111(layout_one, (1, 1), (1, 1)) == 0

    # Replace the placeholders with actual values or function calls
    assert sidb_nm_distance_111(layout_one, (0, 0), (2, 0)) == pytest.approx(1.33)
    assert sidb_nm_distance_111(layout_one, (0, 0), (0, 1)) == pytest.approx(0.3839967)
