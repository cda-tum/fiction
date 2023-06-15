from fiction.pyfiction import *
import unittest
import os

import datetime


class SIDBSimulationResultUnitTest(unittest.TestCase):

    def test_sidb_simulation_result(self):
        # Use standard constructor.
        result = sidb_simulation_result()

        params = sidb_simulation_parameters(
            2, -0.4, 7.1, 10.0, 6.84, 7.68, 2.8)

        # Assign values
        result.algorithm_name = "exgs"
        result.simulation_runtime = datetime.timedelta(seconds=100)
        result.physical_parameters = params
        # result.additional_simulation_parameters = [("paramA", 2), ("paramB", 3)]

        # Perform assertions on the properties
        self.assertEqual(result.algorithm_name, "exgs")
        self.assertEqual(result.physical_parameters.lat_a, params.lat_a)
        self.assertEqual(result.physical_parameters.lat_b, params.lat_b)
        self.assertEqual(result.physical_parameters.lat_c, params.lat_c)
        # self.assertEqual(result.additional_simulation_parameters, [("paramA", "abc"), ("paramB", "def")])


if __name__ == '__main__':
    unittest.main()
