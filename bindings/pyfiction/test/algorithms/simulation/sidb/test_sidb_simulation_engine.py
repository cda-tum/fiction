import unittest

from mnt.pyfiction import *


class TestSiDBSimulationEngine(unittest.TestCase):

    def test_sidb_simulator_engine_names(self):
        self.assertEqual(sidb_simulation_engine_name(sidb_simulator_engine.QUICKEXACT), "QuickExact")
        self.assertEqual(sidb_simulation_engine_name(sidb_simulator_engine.QUICKSIM), "QuickSim")
        self.assertEqual(sidb_simulation_engine_name(sidb_simulator_engine.EXGS), "ExGS")
        self.assertEqual(sidb_simulation_engine_name(sidb_simulator_engine.CLUSTERCOMPLETE), "ClusterComplete")

        self.assertEqual(sidb_simulation_engine_name(exhaustive_sidb_simulator_engine.QUICKEXACT), "QuickExact")
        self.assertEqual(sidb_simulation_engine_name(exhaustive_sidb_simulator_engine.EXGS), "ExGS")
        self.assertEqual(
            sidb_simulation_engine_name(exhaustive_sidb_simulator_engine.CLUSTERCOMPLETE), "ClusterComplete")

        self.assertEqual(sidb_simulation_engine_name(heuristic_sidb_simulator_engine.QUICKSIM), "QuickSim")

    if __name__ == '__main__':
        unittest.main()
