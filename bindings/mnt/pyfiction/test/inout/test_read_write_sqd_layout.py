from mnt.pyfiction import (read_sqd_layout_100, read_sqd_layout_111)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestReadWriteFglLayout(unittest.TestCase):

    def test_read_write(self):
        layout_100 = read_sqd_layout_100(dir_path + "/../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd")
        layout_111 = read_sqd_layout_111(dir_path + "/../resources/AND_mu_032_111_surface.sqd")

if __name__ == '__main__':
    unittest.main()
