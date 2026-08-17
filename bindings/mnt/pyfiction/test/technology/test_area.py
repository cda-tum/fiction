import unittest

import pytest

from mnt.pyfiction import area, inml_layout, qca_layout, sidb_layout


class TestArea(unittest.TestCase):
    def test_qca_area(self):
        lyt = qca_layout((4, 4))
        assert area(lyt) == pytest.approx(9604.0)

    def test_inml_area(self):
        lyt = inml_layout((4, 4))
        assert area(lyt) == pytest.approx(174000.0)

    def test_sidb_area(self):
        lyt = sidb_layout((4, 4))
        assert area(lyt) == pytest.approx(2.359296)


if __name__ == "__main__":
    unittest.main()
