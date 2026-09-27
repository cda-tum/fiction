# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb.analysis import (
    CriticalTemperatureDomain,
    OperationalDomain,
    OperationalStatus,
    ParameterPoint,
    SweepParameter,
)
from mnt.pyfiction.sidb.io import (
    SampleWritingMode,
    WriteOperationalDomainParams,
    write_critical_temperature_domain_to_string,
    write_operational_domain_to_string,
)


def test_write_simple_operational_domain():
    opdom = OperationalDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    opdom[ParameterPoint([0, 0])] = OperationalStatus.OPERATIONAL
    opdom[ParameterPoint([0, 1])] = OperationalStatus.NON_OPERATIONAL

    expected = "epsilon_r,lambda_tf,operational status\n0,0,1\n0,1,0"

    # Get the result from the function that returns a string
    operational_domain_as_string = write_operational_domain_to_string(opdom, params=WriteOperationalDomainParams())

    # Sort both expected and result to handle order variations
    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = "epsilon_r,lambda_tf,operational status\n0,0,True\n0,1,False"
    params = WriteOperationalDomainParams()
    params.operational_tag = "True"
    params.non_operational_tag = "False"

    result_custom = write_operational_domain_to_string(opdom, params=params)

    assert sorted(result_custom.strip().split("\n")) == sorted(expected_custom.strip().split("\n"))


def test_write_operational_domain_with_floating_point_values():
    opdom = OperationalDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    # Using floating point values for the parameter points
    opdom[ParameterPoint([0.1, 0.2])] = OperationalStatus.OPERATIONAL
    opdom[ParameterPoint([0.3, 0.4])] = OperationalStatus.NON_OPERATIONAL

    expected = "epsilon_r,lambda_tf,operational status\n0.1,0.2,1\n0.3,0.4,0"

    # Get the result from the function that returns a string
    operational_domain_as_string = write_operational_domain_to_string(opdom)

    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = "epsilon_r,lambda_tf,operational status\n0.1,0.2,operational\n0.3,0.4,non-operational"
    params = WriteOperationalDomainParams()
    params.operational_tag = "operational"
    params.non_operational_tag = "non-operational"

    operational_domain_custom_as_string = write_operational_domain_to_string(opdom, params=params)

    assert sorted(operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_write_operational_domain_with_metric_values():
    opdom = CriticalTemperatureDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    # Adding metric values
    opdom[ParameterPoint([0.1, 0.2])] = (OperationalStatus.OPERATIONAL, 50.3)
    opdom[ParameterPoint([0.3, 0.4])] = (OperationalStatus.NON_OPERATIONAL, 0.0)

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
    params = WriteOperationalDomainParams()
    params.operational_tag = "operational"
    params.non_operational_tag = "non-operational"

    temperature_operational_domain_custom_as_string = write_critical_temperature_domain_to_string(opdom, params=params)

    assert sorted(temperature_operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_skip_non_operational_samples():
    opdom = OperationalDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    opdom[ParameterPoint([0.1, 0.2])] = OperationalStatus.OPERATIONAL
    opdom[ParameterPoint([0.3, 0.4])] = OperationalStatus.NON_OPERATIONAL

    # Skip non-operational samples
    params = WriteOperationalDomainParams()
    params.writing_mode = SampleWritingMode.OPERATIONAL_ONLY

    expected = "epsilon_r,lambda_tf,operational status\n0.1,0.2,1"

    operational_domain_as_string = write_operational_domain_to_string(opdom, params=params)

    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))
