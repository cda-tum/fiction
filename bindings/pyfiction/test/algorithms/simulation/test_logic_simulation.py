from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
z3 = os.environ.get("z3", "OFF")


class TestLogicSimulation(unittest.TestCase):

    @unittest.skipIf(z3 == "OFF", "Z3 not enabled")
    def test_logic_simulation(self):
        xor2_net = read_technology_network(dir_path + "/../../resources/xor2.v")
        xnor2_net = read_technology_network(dir_path + "/../../resources/xnor2.v")

        xor_sim = simulate(xor2_net)
        self.assertEqual(xor_sim["out"], [False, True, True, False])

        xnor_sim = simulate(xnor2_net)
        self.assertEqual(xnor_sim["out"], [True, False, False, True])

        params = exact_params()
        params.crossings = True
        xor_lyt = exact_cartesian(xor2_net, params)
        xor_lyt_sim = simulate(xor_lyt)
        self.assertEqual(xor_lyt_sim["out"], [False, True, True, False])


if __name__ == '__main__':
    unittest.main()
