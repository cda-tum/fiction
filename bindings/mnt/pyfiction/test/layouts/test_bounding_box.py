from mnt.pyfiction import (
    cartesian_gate_layout,
    shifted_cartesian_gate_layout,
    hexagonal_gate_layout,
    cartesian_obstruction_layout,
    shifted_cartesian_obstruction_layout,
    hexagonal_obstruction_layout,
    read_technology_network,
    orthogonal_params,
    orthogonal,
    qca_layout,
    sidb_layout,
    inml_layout,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestBoundingBox(unittest.TestCase):
    def test_bounding_box_around_an_empty_gate_level_layout(self):
        for layout in [
            cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            cartesian_obstruction_layout(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")),
        ]:
            min_coord, max_coord = layout.bounding_box_2d()
            self.assertEqual(min_coord, layout.coord(0, 0))
            self.assertEqual(max_coord, layout.coord(0, 0))
            self.assertEqual(max_coord.x - min_coord.x, 0)
            self.assertEqual(max_coord.y - min_coord.y, 0)

    def test_initialize_gate_level_with_ortho_bounding_box(self):
        network = read_technology_network(os.path.join(dir_path + "/../resources/mux21.v"))
        params = orthogonal_params()
        layout = orthogonal(network, params)
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(5, 7))
        self.assertEqual(max_coord.x - min_coord.x, 5)
        self.assertEqual(max_coord.y - min_coord.y, 7)

        network = read_technology_network(os.path.join(dir_path + "/../resources/xnor2.v"))
        layout = orthogonal(network, params)
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(5, 8))
        self.assertEqual(max_coord.x - min_coord.x, 5)
        self.assertEqual(max_coord.y - min_coord.y, 8)

        network = read_technology_network(os.path.join(dir_path + "/../resources/xor2.v"))
        layout = orthogonal(network, params)
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(4, 7))
        self.assertEqual(max_coord.x - min_coord.x, 4)
        self.assertEqual(max_coord.y - min_coord.y, 7)

        network = read_technology_network(os.path.join(dir_path + "/../resources/FA.v"))
        layout = orthogonal(network, params)
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(7, 11))
        self.assertEqual(max_coord.x - min_coord.x, 7)
        self.assertEqual(max_coord.y - min_coord.y, 11)

    def test_update_gate_level_bounding_box(self):
        network = read_technology_network(os.path.join(dir_path + "/../resources/mux21.v"))
        params = orthogonal_params()
        layout = orthogonal(network, params)
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(5, 7))
        self.assertEqual(max_coord.x - min_coord.x, 5)
        self.assertEqual(max_coord.y - min_coord.y, 7)

        layout.clear_tile((4, 7))
        layout.move_node(layout.get_node((5, 7)), (5, 6), [layout.make_signal(layout.get_node((4, 6)))])
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(5, 6))
        self.assertEqual(max_coord.x - min_coord.x, 5)
        self.assertEqual(max_coord.y - min_coord.y, 6)

        layout.move_node(layout.get_node((5, 6)), (4, 7), [layout.make_signal(layout.get_node((4, 6)))])
        min_coord, max_coord = layout.bounding_box_2d()

        self.assertEqual(min_coord, layout.coord(0, 0))
        self.assertEqual(max_coord, layout.coord(4, 7))
        self.assertEqual(max_coord.x - min_coord.x, 4)
        self.assertEqual(max_coord.y - min_coord.y, 7)

    def test_bounding_box_around_cell_level_layout(self):
        for layout in [
            qca_layout((2, 2, 0), "2DDWave", "Layout"),
            inml_layout((2, 2, 0), "2DDWave", "Layout"),
            sidb_layout((2, 2, 0), "2DDWave", "Layout"),
        ]:
            min_coord, max_coord = layout.bounding_box_2d()
            self.assertEqual(min_coord, layout.coord(0, 0))
            self.assertEqual(max_coord, layout.coord(0, 0))
            self.assertEqual(max_coord.x - min_coord.x, 0)
            self.assertEqual(max_coord.y - min_coord.y, 0)


if __name__ == "__main__":
    unittest.main()
