from mnt.pyfiction import (read_sqd_layout_100)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestReadWriteFglLayout(unittest.TestCase):

    def test_read_write(self):
        layout = read_sqd_layout_100(dir_path + "/../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd")


if __name__ == '__main__':
    unittest.main()
