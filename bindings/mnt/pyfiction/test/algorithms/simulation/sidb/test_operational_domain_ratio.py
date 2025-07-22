import os
import unittest

from mnt.pyfiction import (
    create_and_tt,
    operational_domain_params,
    operational_domain_ratio,
    operational_domain_ratio_params,
    operational_domain_value_range,
    parameter_point,
    read_sqd_layout_100,
    sidb_simulation_engine,
    sweep_parameter,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestComputeOperationalRatioAtPoint(unittest.TestCase):
    def test_and_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/21_hex_inputsdbp_and_v19.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.00, 6.00, 0.1),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 6.00, 0.1),
        ]

        ratio_params = operational_domain_ratio_params()
        ratio_params.op_domain_params = params

        self.assertEqual(ratio_params.op_domain_params.operational_params.simulation_parameters.base, 2)

        operational_domain_ratio_result = operational_domain_ratio(
            lyt,
            [create_and_tt()],
            parameter_point([5.6, 5.0]),
            ratio_params,
        )

        self.assertAlmostEqual(operational_domain_ratio_result, 23 / 121, delta=10e-6)


if __name__ == "__main__":
    unittest.main()
