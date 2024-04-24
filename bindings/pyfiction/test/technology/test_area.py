import unittest
from mnt.pyfiction import *


class TestArea(unittest.TestCase):
    def test_qca_area(self):
        lyt = qca_layout((4, 4))
        self.assertAlmostEqual(area(lyt), 9604.0)

    def test_inml_area(self):
        lyt = inml_layout((4, 4))
        self.assertAlmostEqual(area(lyt), 174000.0)

    def test_sidb_area(self):
        lyt = sidb_layout((4, 4))
        self.assertAlmostEqual(area(lyt), 2.359296)


if __name__ == '__main__':
    unittest.main()
