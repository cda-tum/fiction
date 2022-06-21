from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestLogicNetwork(unittest.TestCase):

    def test_read_logic_network(self):
        network = pyfiction.read_logic_network(dir_path + "/../resources/mux21.v")

        with self.assertRaises(RuntimeError):
            network = pyfiction.read_logic_network(dir_path + "/mux41.v")


if __name__ == '__main__':
    unittest.main()
