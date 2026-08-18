import tempfile
import unittest
from pathlib import Path

from mnt.pyfiction import mol_qca_layout, mol_qca_technology, write_qll_layout


class TestWriteQllLayout(unittest.TestCase):
    def test_write_mol_qca_layout(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir:
            filename = Path(tmp_dir) / "mol_qca.qll"

            layout = mol_qca_layout((1, 0), "OPEN", "molQCA")
            layout.assign_cell_type((0, 0), mol_qca_technology.cell_type.NORMAL1)
            layout.assign_cell_type((1, 0), mol_qca_technology.cell_type.NORMAL2)

            write_qll_layout(layout, str(filename))

            qll = filename.read_text(encoding="utf-8")

            assert '<settings tech="MolFCN">' in qll
            assert '<item tech="MolFCN" name="Bisferrocene"/>' in qll
            assert '<property name="phase" value="0"/>' in qll
            assert '<property name="phase" value="1"/>' in qll


if __name__ == "__main__":
    unittest.main()
