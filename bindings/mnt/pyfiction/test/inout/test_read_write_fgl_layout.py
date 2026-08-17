import os

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    hexagonalization,
    orthogonal,
    read_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    shifted_cartesian_gate_layout,
    write_fgl_layout,
)


def test_read_write(mux21):
    cart_layout = orthogonal(mux21)
    write_fgl_layout(cart_layout, "mux21_cartesian.fgl")
    read_cart_layout = read_cartesian_fgl_layout("mux21_cartesian.fgl")
    assert equivalence_checking(read_cart_layout, cart_layout) == eq_type.STRONG

    hex_layout = hexagonalization(cart_layout)
    write_fgl_layout(hex_layout, "mux21_hexagonal.fgl")
    read_hexagonal_layout = read_hexagonal_fgl_layout("mux21_hexagonal.fgl")
    assert equivalence_checking(read_hexagonal_layout, hex_layout) == eq_type.STRONG

    shifted_cart_layout = shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")
    write_fgl_layout(shifted_cart_layout, "empty_shifted_cartesian.fgl")
    read_shifted_cart_layout = read_shifted_cartesian_fgl_layout("empty_shifted_cartesian.fgl")
    assert equivalence_checking(read_shifted_cart_layout, shifted_cart_layout) == eq_type.STRONG

    try:
        os.remove("mux21_cartesian.fgl")
        os.remove("mux21_hexagonal.fgl")
        os.remove("empty_shifted_cartesian.fgl")
    except OSError as e:
        print(f"Error: {e.filename} - {e.strerror}.")
