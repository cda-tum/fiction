# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.sidb.simulation import io, logic
from mnt.pyfiction.sidb.simulation.io import (
    sample_writing_mode,
    write_critical_temperature_domain_to_string,
    write_operational_domain_to_string,
)
from mnt.pyfiction.sidb.simulation.logic import (
    critical_temperature_domain,
    operational_domain,
    operational_status,
    parameter_point,
    sweep_parameter,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_write_simple_operational_domain():
    opdom = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    opdom[parameter_point([0, 0])] = operational_status.OPERATIONAL
    opdom[parameter_point([0, 1])] = operational_status.NON_OPERATIONAL

    expected = "epsilon_r,lambda_tf,operational status\n0,0,1\n0,1,0"

    # Get the result from the function that returns a string
    operational_domain_as_string = write_operational_domain_to_string(opdom)

    # Sort both expected and result to handle order variations
    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))

    # Custom operational tags
    expected_custom = "epsilon_r,lambda_tf,operational status\n0,0,True\n0,1,False"

    result_custom = write_operational_domain_to_string(opdom, operational_tag="True", non_operational_tag="False")

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

    operational_domain_custom_as_string = write_operational_domain_to_string(
        opdom, operational_tag="operational", non_operational_tag="non-operational"
    )

    assert sorted(operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_write_operational_domain_with_metric_values():
    opdom = critical_temperature_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    # Adding metric values
    opdom[parameter_point([0.1, 0.2])] = (operational_status.OPERATIONAL, 50.3)
    opdom[parameter_point([0.3, 0.4])] = (operational_status.NON_OPERATIONAL, 0.0)

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

    temperature_operational_domain_custom_as_string = write_critical_temperature_domain_to_string(
        opdom, operational_tag="operational", non_operational_tag="non-operational"
    )

    assert sorted(temperature_operational_domain_custom_as_string.strip().split("\n")) == sorted(
        expected_custom.strip().split("\n")
    )


def test_skip_non_operational_samples():
    opdom = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    opdom[parameter_point([0.1, 0.2])] = operational_status.OPERATIONAL
    opdom[parameter_point([0.3, 0.4])] = operational_status.NON_OPERATIONAL

    # Skip non-operational samples

    expected = "epsilon_r,lambda_tf,operational status\n0.1,0.2,1"

    operational_domain_as_string = write_operational_domain_to_string(
        opdom, writing_mode=sample_writing_mode.OPERATIONAL_ONLY
    )

    assert sorted(operational_domain_as_string.strip().split("\n")) == sorted(expected.strip().split("\n"))


@pytest.mark.parametrize("domain_name", ["operational_domain", "critical_temperature_domain"])
def test_domain_writer_parameters(tmp_path: Path, domain_name: str) -> None:
    """File writers require params; string writers provide the same defaults."""
    domain = getattr(logic, domain_name)([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])
    point = parameter_point([0.1, 0.2])
    domain[point] = (
        operational_status.OPERATIONAL
        if domain_name == "operational_domain"
        else (operational_status.OPERATIONAL, 50.3)
    )
    write_file = getattr(io, f"write_{domain_name}")
    write_string = getattr(io, f"write_{domain_name}_to_string")
    filename = tmp_path / "domain.csv"
    with pytest.raises(TypeError):
        write_file(opdom=domain, filename=str(filename))
    params = write_operational_domain_params()
    write_file(opdom=domain, filename=str(filename), params=params)
    assert filename.read_text(encoding="utf-8") == write_string(opdom=domain)
    params.operational_tag = "working"
    write_file(opdom=domain, filename=str(filename), params=params)
    assert filename.read_text(encoding="utf-8") == write_string(opdom=domain, params=params)
    assert "working" in filename.read_text(encoding="utf-8")
