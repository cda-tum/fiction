import unittest
from io import StringIO

from mnt.pyfiction import (
    operational_domain,
    operational_status,
    parameter_point,
    sample_writing_mode,
    sweep_parameter,
    write_operational_domain,
    write_operational_domain_params,
)


class TestWriteOperationalDomain(unittest.TestCase):
    def test_write_empty_operational_domain(self):
        os = StringIO()

        # Default sweep dimensions
        opdom = operational_domain()
        opdom.dimensions.extend([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

        expected = "epsilon_r,lambda_tf,operational status\n"

        write_operational_domain(opdom, os)
        self.assertEqual(os.getvalue(), expected)

        # Custom sweep dimensions
        opdom = operational_domain()
        opdom.dimensions.extend([sweep_parameter.LAMBDA_TF, sweep_parameter.MU_MINUS])

        expected = "lambda_tf,mu_minus,operational status\n"

        os = StringIO()
        write_operational_domain(opdom, os)
        self.assertEqual(os.getvalue(), expected)

    def test_write_simple_operational_domain(self):
        opdom = operational_domain()
        opdom.dimensions.extend([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

        opdom.operational_values = {
            parameter_point([0, 0]): operational_status.OPERATIONAL,
            parameter_point([0, 1]): operational_status.NON_OPERATIONAL,
        }

        # Default operational tags
        os = StringIO()
        expected = {"epsilon_r,lambda_tf,operational status", "0,0,1", "0,1,0"}

        write_operational_domain(opdom, os)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Custom operational tags
        os = StringIO()
        params = write_operational_domain_params(operational_tag="True", non_operational_tag="False")
        expected = {"epsilon_r,lambda_tf,operational status", "0,0,True", "0,1,False"}

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Skip non-operational samples
        os = StringIO()
        params.writing_mode = sample_writing_mode.OPERATIONAL_ONLY
        expected = {"epsilon_r,lambda_tf,operational status", "0,0,1"}

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

    def test_write_operational_domain_with_floating_point_values(self):
        opdom = operational_domain()
        opdom.dimensions.extend([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

        opdom.operational_values = {
            parameter_point([0.1, 0.2]): operational_status.OPERATIONAL,
            parameter_point([0.3, 0.4]): operational_status.NON_OPERATIONAL,
            parameter_point([1.2, 1.4]): operational_status.OPERATIONAL,
            parameter_point([2.4, 5.75]): operational_status.NON_OPERATIONAL,
        }

        # Default operational tags
        os = StringIO()
        expected = {
            "epsilon_r,lambda_tf,operational status",
            "0.1,0.2,1",
            "0.3,0.4,0",
            "1.2,1.4,1",
            "2.4,5.75,0",
        }

        write_operational_domain(opdom, os)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Custom operational tags
        os = StringIO()
        params = write_operational_domain_params(operational_tag="operational", non_operational_tag="non-operational")
        expected = {
            "epsilon_r,lambda_tf,operational status",
            "0.1,0.2,operational",
            "0.3,0.4,non-operational",
            "1.2,1.4,operational",
            "2.4,5.75,non-operational",
        }

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Skip non-operational samples
        os = StringIO()
        params.writing_mode = sample_writing_mode.OPERATIONAL_ONLY
        expected = {"epsilon_r,lambda_tf,operational status", "0.1,0.2,1", "1.2,1.4,1"}

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

    def test_write_operational_domain_with_floating_point_and_temperature_values(self):
        opdom = operational_domain()
        opdom.dimensions.extend([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

        opdom.operational_values = {
            parameter_point([0.1, 0.2]): operational_status.OPERATIONAL,
            parameter_point([0.3, 0.4]): operational_status.NON_OPERATIONAL,
            parameter_point([1.2, 1.4]): operational_status.OPERATIONAL,
            parameter_point([2.4, 5.75]): operational_status.NON_OPERATIONAL,
        }

        opdom.metric_values = {
            parameter_point([0.1, 0.2]): 50.3,
            parameter_point([0.3, 0.4]): 0.0,
            parameter_point([1.2, 1.4]): 400.0,
            parameter_point([2.4, 5.75]): 0.0,
        }

        # Default operational tags
        os = StringIO()
        expected = {
            "epsilon_r,lambda_tf,operational status,ct",
            "0.1,0.2,1,50.3",
            "0.3,0.4,0,0",
            "1.2,1.4,1,400",
            "2.4,5.75,0,0",
        }

        write_operational_domain(opdom, os)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Custom operational tags
        os = StringIO()
        params = write_operational_domain_params(operational_tag="operational", non_operational_tag="non-operational")
        expected = {
            "epsilon_r,lambda_tf,operational status,ct",
            "0.1,0.2,operational,50.3",
            "0.3,0.4,non-operational,0",
            "1.2,1.4,operational,400",
            "2.4,5.75,non-operational,0",
        }

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)

        # Skip non-operational samples
        os = StringIO()
        params.writing_mode = sample_writing_mode.OPERATIONAL_ONLY
        expected = {
            "epsilon_r,lambda_tf,operational status,ct",
            "0.1,0.2,1,50.3",
            "1.2,1.4,1,400",
        }

        write_operational_domain(opdom, os, params)
        lines = set(os.getvalue().strip().split("\n"))
        self.assertEqual(lines, expected)


if __name__ == "__main__":
    unittest.main()
