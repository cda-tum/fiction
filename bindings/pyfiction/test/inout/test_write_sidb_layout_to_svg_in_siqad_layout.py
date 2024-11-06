import unittest
import os
from mnt.pyfiction import *

class TestWriteSidbLayoutToSvgInSiqadLayout(unittest.TestCase):

    def test_write_sidb_cell_level_layout(self):
        # Determine the directory paths
        dir_path = os.path.dirname(os.path.realpath(__file__))
        original_svg_path = os.path.join(dir_path, "../resources/cell_level_layout_dark_mode.svg")


        # Read the original SVG file
        with open(original_svg_path, 'rb') as f:
            original_svg_content = f.read()

        # Create and configure the SIDB layout
        sidb_layout = sidb_100_lattice((4, 4))
        sidb_layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        sidb_layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        sidb_layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        sidb_layout.assign_cell_type((3, 3), sidb_technology.cell_type.NORMAL)

        # todo need to discuss how this can be solved

if __name__ == '__main__':
    unittest.main()
