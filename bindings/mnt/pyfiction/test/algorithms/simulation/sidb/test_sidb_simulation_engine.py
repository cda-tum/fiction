import unittest

from mnt.pyfiction import (
    exact_sidb_simulation_engine,
    heuristic_sidb_simulation_engine,
    sidb_simulation_engine,
    sidb_simulation_engine_name,
)


class TestSiDBSimulationEngine(unittest.TestCase):
    def test_sidb_simulation_engine_names(self):
        assert sidb_simulation_engine_name(sidb_simulation_engine.QUICKEXACT) == "QuickExact"
        assert sidb_simulation_engine_name(sidb_simulation_engine.QUICKSIM) == "QuickSim"
        assert sidb_simulation_engine_name(sidb_simulation_engine.EXGS) == "ExGS"
        assert sidb_simulation_engine_name(sidb_simulation_engine.CLUSTERCOMPLETE) == "ClusterComplete"

        assert sidb_simulation_engine_name(exact_sidb_simulation_engine.QUICKEXACT) == "QuickExact"
        assert sidb_simulation_engine_name(exact_sidb_simulation_engine.EXGS) == "ExGS"
        assert sidb_simulation_engine_name(exact_sidb_simulation_engine.CLUSTERCOMPLETE) == "ClusterComplete"

        assert sidb_simulation_engine_name(heuristic_sidb_simulation_engine.QUICKSIM) == "QuickSim"

    if __name__ == "__main__":
        unittest.main()
