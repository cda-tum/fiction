import os
import unittest

from mnt.pyfiction import read_sqd_layout_100, read_sqd_layout_111

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestReadWriteSqdLayout(unittest.TestCase):
    def test_read_write(self):
        read_sqd_layout_100(dir_path + "/../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd")
        read_sqd_layout_111(dir_path + "/../resources/AND_mu_032_111_surface.sqd")


if __name__ == "__main__":
    unittest.main()
