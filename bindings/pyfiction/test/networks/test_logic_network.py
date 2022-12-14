from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestLogicNetwork(unittest.TestCase):

    def test_read_logic_network(self):
        network = pyfiction.read_logic_network(dir_path + "/../resources/mux21.v")

        self.assertEqual(network.gates(), [5, 6, 7, 8, 9])
        self.assertTrue(network.is_inv(5))
        self.assertTrue(network.is_and(6))
        self.assertTrue(network.is_and(7))
        self.assertTrue(network.is_or(8))

        self.assertEqual(network.pis(), [2, 3, 4])
        self.assertTrue(network.is_pi(2))
        self.assertTrue(network.is_pi(3))
        self.assertTrue(network.is_pi(4))

        self.assertEqual(network.pos(), [9])
        self.assertTrue(network.is_po(9))

        self.assertEqual(network.fanins(0), [])
        self.assertEqual(network.fanins(1), [])
        self.assertEqual(network.fanins(2), [])
        self.assertEqual(network.fanins(3), [])
        self.assertEqual(network.fanins(4), [])
        self.assertEqual(network.fanins(5), [4])
        self.assertEqual(network.fanins(6), [2, 5])
        self.assertEqual(network.fanins(7), [3, 4])
        self.assertEqual(network.fanins(8), [6, 7])
        self.assertEqual(network.fanins(9), [8])

        with self.assertRaises(RuntimeError):
            network = pyfiction.read_logic_network(dir_path + "/mux41.v")


if __name__ == '__main__':
    unittest.main()
