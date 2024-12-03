from mnt.pyfiction import (
    read_technology_network,
    orthogonal,
    write_fgl_layout,
    read_cartesian_fgl_layout,
    equivalence_checking,
    eq_type,
    hexagonalization,
    read_hexagonal_fgl_layout,
    shifted_cartesian_gate_layout,
    read_shifted_cartesian_fgl_layout,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestReadWriteFglLayout(unittest.TestCase):
    def test_read_write(self):
        network = read_technology_network(dir_path + "/../resources/mux21.v")
        cart_layout = orthogonal(network)
        write_fgl_layout(cart_layout, "mux21_cartesian.fgl")
        read_cart_layout = read_cartesian_fgl_layout("mux21_cartesian.fgl")
        self.assertEqual(equivalence_checking(read_cart_layout, cart_layout), eq_type.STRONG)

        hex_layout = hexagonalization(cart_layout)
        write_fgl_layout(hex_layout, "mux21_hexagonal.fgl")
        read_hexagonal_layout = read_hexagonal_fgl_layout("mux21_hexagonal.fgl")
        self.assertEqual(equivalence_checking(read_hexagonal_layout, hex_layout), eq_type.STRONG)

        shifted_cart_layout = shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")
        write_fgl_layout(shifted_cart_layout, "empty_shifted_cartesian.fgl")
        read_shifted_cart_layout = read_shifted_cartesian_fgl_layout("empty_shifted_cartesian.fgl")
        self.assertEqual(equivalence_checking(read_shifted_cart_layout, shifted_cart_layout), eq_type.STRONG)

        try:
            os.remove("mux21_cartesian.fgl")
            os.remove("mux21_hexagonal.fgl")
            os.remove("empty_shifted_cartesian.fgl")
        except OSError as e:
            print("Error: %s - %s." % (e.filename, e.strerror))


if __name__ == "__main__":
    unittest.main()
