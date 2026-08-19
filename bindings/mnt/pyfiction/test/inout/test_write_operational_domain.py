from __future__ import annotations

from mnt.pyfiction import (
    critical_temperature_domain,
    operational_domain,
    operational_status,
    parameter_point,
    sample_writing_mode,
    sweep_parameter,
    write_critical_temperature_domain_to_string,
    write_operational_domain_params,
    write_operational_domain_to_string,
)


def test_write_simple_operational_domain():
    opdom = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    opdom[parameter_point([0, 0])] = operational_status.OPERATIONAL
    opdom[parameter_point([0, 1])] = operational_status.NON_OPERATIONAL

    expected = "epsilon_r,lambda_tf,operational status\n0,0,1\n0,1,0"

    # Get the result from the function that returns a string
    operational_domain_as_string = write_operational_domain_to_string(opdom, write_operational_domain_params())

    # Sort both expected and result to handle order variations
    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = "epsilon_r,lambda_tf,operational status\n0,0,True\n0,1,False"
    params = write_operational_domain_params()
    params.operational_tag = "True"
    params.non_operational_tag = "False"

    result_custom = write_operational_domain_to_string(opdom, params)

    assert sorted(result_custom.strip().split("\n")) == sorted(expected_custom.strip().split("\n"))


def test_write_operational_domain_with_floating_point_values():
    opdom = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    # Using floating point values for the parameter points
    opdom[parameter_point([0.1, 0.2])] = operational_status.OPERATIONAL
    opdom[parameter_point([0.3, 0.4])] = operational_status.NON_OPERATIONAL

    expected = "epsilon_r,lambda_tf,operational status\n0.1,0.2,1\n0.3,0.4,0"

    # Get the result from the function that returns a string
    operational_domain_as_string = write_operational_domain_to_string(opdom)

    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = "epsilon_r,lambda_tf,operational status\n0.1,0.2,operational\n0.3,0.4,non-operational"
    params = write_operational_domain_params()
    params.operational_tag = "operational"
    params.non_operational_tag = "non-operational"

    operational_domain_custom_as_string = write_operational_domain_to_string(opdom, params)

    assert sorted(operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_write_operational_domain_with_metric_values():
    opdom = critical_temperature_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    # Adding metric values
    opdom[parameter_point([0.1, 0.2])] = [operational_status.OPERATIONAL, 50.3]
    opdom[parameter_point([0.3, 0.4])] = [operational_status.NON_OPERATIONAL, 0.0]

    expected = "epsilon_r,lambda_tf,operational status,critical temperature\n0.1,0.2,1,50.3\n0.3,0.4,0,0"

    # Get the result from the function that returns a string
    temperature_operational_domain_as_string = write_critical_temperature_domain_to_string(opdom)

    assert sorted(temperature_operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = (
        "epsilon_r,lambda_tf,operational status,critical temperature\n"
        "0.1,0.2,operational,50.3\n"
        "0.3,0.4,non-operational,0"
    )
    params = write_operational_domain_params()
    params.operational_tag = "operational"
    params.non_operational_tag = "non-operational"

    temperature_operational_domain_custom_as_string = write_critical_temperature_domain_to_string(opdom, params)

    assert sorted(temperature_operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_skip_non_operational_samples():
    opdom = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    opdom[parameter_point([0.1, 0.2])] = operational_status.OPERATIONAL
    opdom[parameter_point([0.3, 0.4])] = operational_status.NON_OPERATIONAL

    # Skip non-operational samples
    params = write_operational_domain_params()
    params.writing_mode = sample_writing_mode.OPERATIONAL_ONLY

    expected = "epsilon_r,lambda_tf,operational status\n0.1,0.2,1"

    operational_domain_as_string = write_operational_domain_to_string(opdom, params)

    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))
