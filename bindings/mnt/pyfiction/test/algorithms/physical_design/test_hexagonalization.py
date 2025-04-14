import os
import unittest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_params,
    hexagonalization_stats,
    orthogonal,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestHexagonalization(unittest.TestCase):
    def test_hexagonalization_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        cart_layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, cart_layout), eq_type.STRONG)
        hex_layout = hexagonalization(cart_layout)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)

    def test_hexagonalization_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        cart_layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, cart_layout), eq_type.STRONG)
        params = hexagonalization_params()
        hex_layout = hexagonalization(cart_layout, params)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)

    def test_hexagonalization_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        cart_layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, cart_layout), eq_type.STRONG)
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, statistics=stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)

    def test_hexagonalization_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        cart_layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, cart_layout), eq_type.STRONG)

        params = hexagonalization_params()
        params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.NONE
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for pi in hex_layout.pis():
            self.assertEqual(pi.y, 0)

        params.input_pin_extension = hexagonalization_io_pin_extension_mode.NONE
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for po in hex_layout.pos():
            self.assertEqual(po.y, hex_layout.y())

        params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for pi in hex_layout.pis():
            self.assertEqual(pi.y, 0)
        for po in hex_layout.pos():
            self.assertEqual(po.y, hex_layout.y())

        params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.NONE
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for pi in hex_layout.pis():
            self.assertEqual(pi.y, 0)

        params.input_pin_extension = hexagonalization_io_pin_extension_mode.NONE
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for po in hex_layout.pos():
            self.assertEqual(po.y, hex_layout.y())

        params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
        params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
        stats = hexagonalization_stats()
        hex_layout = hexagonalization(cart_layout, params, stats)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        for pi in hex_layout.pis():
            self.assertEqual(pi.y, 0)
        for po in hex_layout.pos():
            self.assertEqual(po.y, hex_layout.y())


if __name__ == "__main__":
    unittest.main()
