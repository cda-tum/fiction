import os
import unittest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestGraphOrientedLayoutDesign(unittest.TestCase):
    def test_graph_oriented_layout_design(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = graph_oriented_layout_design(network)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True

        layout = graph_oriented_layout_design(network, params)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        stats = graph_oriented_layout_design_stats()

        layout = graph_oriented_layout_design(network, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True

        stats = graph_oriented_layout_design_stats()

        layout = graph_oriented_layout_design(network, params, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_different_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True
        params.mode = gold_effort_mode.HIGH_EFFORT
        params.timeout = 10000
        params.verbose = True
        params.num_vertex_expansions = 5
        params.planar = False
        params.cost = gold_cost_objective.WIRES
        params.enable_multithreading = False

        layout = graph_oriented_layout_design(network, params)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_custom_cost_function(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True
        params.mode = gold_effort_mode.HIGH_EFFORT
        params.cost = gold_cost_objective.CUSTOM

        def custom_cost_objective(layout):
            return layout.num_wires() * 2 + layout.num_crossings()

        layout = graph_oriented_layout_design(network, params, custom_cost_objective=custom_cost_objective)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_multithreading(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True
        params.mode = gold_effort_mode.HIGH_EFFORT
        params.enable_multithreading = True

        layout = graph_oriented_layout_design(network, params)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)


if __name__ == "__main__":
    unittest.main()
