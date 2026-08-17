import pytest

from mnt.pyfiction import area, inml_layout, qca_layout, sidb_layout


def test_qca_area():
    lyt = qca_layout((4, 4))
    assert area(lyt) == pytest.approx(9604.0)


def test_inml_area():
    lyt = inml_layout((4, 4))
    assert area(lyt) == pytest.approx(174000.0)


def test_sidb_area():
    lyt = sidb_layout((4, 4))
    assert area(lyt) == pytest.approx(2.359296)
