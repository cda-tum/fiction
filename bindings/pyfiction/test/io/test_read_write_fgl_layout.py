from fiction.pyfiction import *
from dotenv import load_dotenv
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
load_dotenv()
z3 = os.environ.get("z3", "OFF")


class TestReadWriteFglLayout(unittest.TestCase):

    def test_read_write_cartesian(self):
        network = read_logic_network(dir_path + "/../resources/mux21.v")
        layout = orthogonal(network)
        write_fgl_layout(layout, "mux21.fgl")
        read_layout = read_fgl_layout("mux21.fgl")
        self.assertEqual(equivalence_checking(read_layout, layout), eq_type.STRONG)

    @unittest.skipIf(z3 == "OFF", "Z3 not enabled")
    def test_read_write_hexagonal(self):
        network = read_logic_network(dir_path + "/../resources/mux21.v")
        layout = exact_hexagonal(network)
        write_fgl_layout(layout, "mux21_hex.fgl")
        read_layout = read_fgl_layout("mux21_hex.fgl")
        self.assertEqual(equivalence_checking(read_layout, layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
