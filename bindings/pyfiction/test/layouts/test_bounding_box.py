from fiction.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestBoundingBox(unittest.TestCase):

    def test_bounding_box_around_an_empty_gate_level_layout(self):
        for bb in [cartesian_gate_layout_bounding_box_2d(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
                   shifted_cartesian_gate_layout_bounding_box_2d(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
                   hexagonal_gate_layout_bounding_box_2d(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")),
                   cartesian_obstruction_layout_bounding_box_2d(cartesian_obstruction_layout(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"))),
                   shifted_cartesian_obstruction_layout_bounding_box_2d(shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"))),
                   hexagonal_obstruction_layout_bounding_box_2d(hexagonal_obstruction_layout(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")))]:
            self.assertEqual(bb.get_min(), offset_coordinate(0, 0))
            self.assertEqual(bb.get_max(), offset_coordinate(0, 0))
            self.assertEqual(bb.get_x_size(), 0)
            self.assertEqual(bb.get_y_size(), 0)

    def test_initialize_gate_level_bounding_box(self):
        network = read_logic_network(os.path.join(dir_path + "/../resources/mux21.v"))
        params = orthogonal_params()
        layout = orthogonal(network, params)
        bb_mux = bounding_box_2d(layout)

        self.assertEqual(bb_mux.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_mux.get_max(), layout.coord(5, 7))
        self.assertEqual(bb_mux.get_x_size(), 5)
        self.assertEqual(bb_mux.get_y_size(), 7)

        network = read_logic_network(os.path.join(dir_path + "/../resources/xnor2.v"))
        layout = orthogonal(network, params)
        bb_xnor = bounding_box_2d(layout)

        self.assertEqual(bb_xnor.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_xnor.get_max(), layout.coord(5, 8))
        self.assertEqual(bb_xnor.get_x_size(), 5)
        self.assertEqual(bb_xnor.get_y_size(), 8)

        network = read_logic_network(os.path.join(dir_path + "/../resources/xor2.v"))
        layout = orthogonal(network, params)
        bb_xor = bounding_box_2d(layout)

        self.assertEqual(bb_xor.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_xor.get_max(), layout.coord(4, 7))
        self.assertEqual(bb_xor.get_x_size(), 4)
        self.assertEqual(bb_xor.get_y_size(), 7)

        network = read_logic_network(os.path.join(dir_path + "/../resources/FA.v"))
        layout = orthogonal(network, params)
        bb_fa = bounding_box_2d(layout)

        self.assertEqual(bb_fa.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_fa.get_max(), layout.coord(7, 11))
        self.assertEqual(bb_fa.get_x_size(), 7)
        self.assertEqual(bb_fa.get_y_size(), 11)

    def test_update_gate_level_bounding_box(self):
        network = read_logic_network(os.path.join(dir_path + "/../resources/mux21.v"))
        params = orthogonal_params()
        layout = orthogonal(network, params)
        bb_mux = bounding_box_2d(layout)

        self.assertEqual(bb_mux.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_mux.get_max(), layout.coord(5, 7))
        self.assertEqual(bb_mux.get_x_size(), 5)
        self.assertEqual(bb_mux.get_y_size(), 7)

        layout.clear_tile((4, 7))
        layout.move_node(layout.get_node((5, 7)), (5, 6), [layout.make_signal(layout.get_node((4, 6)))])
        bb_mux.update_bounding_box()

        self.assertEqual(bb_mux.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_mux.get_max(), layout.coord(5, 6))
        self.assertEqual(bb_mux.get_x_size(), 5)
        self.assertEqual(bb_mux.get_y_size(), 6)

        layout.move_node(layout.get_node((5, 6)), (4, 7), [layout.make_signal(layout.get_node((4, 6)))])
        bb_mux.update_bounding_box()

        self.assertEqual(bb_mux.get_min(), layout.coord(0, 0))
        self.assertEqual(bb_mux.get_max(), layout.coord(4, 7))
        self.assertEqual(bb_mux.get_x_size(), 4)
        self.assertEqual(bb_mux.get_y_size(), 7)
