# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of SiDB simulation results."""

import enum

import mnt.pyfiction.sidb.simulation
import mnt.pyfiction.sidb.simulation.logic

def write_sqd_sim_result(sim_result: mnt.pyfiction.sidb.simulation.sidb_simulation_result, filename: str) -> None:
    """
    Writes a simulation result as a SiQAD simulation result file.

    Args:
        sim_result: Result to write.
        filename: File to write into.

    Raises:
        RuntimeError: If creating, writing, closing, or replacing the
                      output file fails.
        ValueError: if a distribution has different sites from the result
                    layout.
    """

class sample_writing_mode(enum.Enum):
    """Mode selector for writing samples to file."""

    ALL_SAMPLES = 0
    """
    Write all samples, including non-operational ones. This may lead to
    large file sizes.
    """

    OPERATIONAL_ONLY = 1
    """
    Write operational samples only. This can drastically reduce file size
    and help with visibility in 3D plots.
    """

def write_operational_domain(
    opdom: mnt.pyfiction.sidb.simulation.logic.operational_domain,
    filename: str,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> None:
    """
    Write the operational domain to the supplied filename as CSV.
    CSV columns hold the sweep-parameter values followed by the operational-status tag.
    The keyword options select status tags and which samples to write.

    Args:
        opdom: Domain samples to write.
        filename: Output CSV file path.
        operational_tag: The tag used to represent the operational value of a parameter set.
        non_operational_tag: The tag used to represent the non-operational value of a parameter set.
        writing_mode: Write all samples or only operational samples.

    Raises:
        ValueError: The domain does not have one to three sweep dimensions.
        RuntimeError: The file cannot be created, written, closed, or replaced.
    """

def write_operational_domain_to_string(
    opdom: mnt.pyfiction.sidb.simulation.logic.operational_domain,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> str:
    """
    Return the domain CSV with a header and one row per selected sample.
    Columns hold the sweep-parameter values and operational-status tag; temperature domains add critical temperature.
    The keyword options select status tags and which samples to write.

    Args:
        opdom: Domain samples to write.
        operational_tag: The tag used to represent the operational value of a parameter set.
        non_operational_tag: The tag used to represent the non-operational value of a parameter set.
        writing_mode: Write all samples or only operational samples.

    Returns:
        CSV text for the domain.

    Raises:
        ValueError: The domain does not have one to three sweep dimensions.
    """

def write_critical_temperature_domain(
    opdom: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain,
    filename: str,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> None:
    """
    Write critical-temperature samples to a CSV file.
    CSV columns hold sweep-parameter values, the operational-status tag, and critical temperature.
    The keyword options select status tags and which samples to write.

    Args:
        opdom: Domain samples to write.
        filename: Output CSV file path.
        operational_tag: The tag used to represent the operational value of a parameter set.
        non_operational_tag: The tag used to represent the non-operational value of a parameter set.
        writing_mode: Write all samples or only operational samples.

    Raises:
        ValueError: The domain does not have one to three sweep dimensions.
        RuntimeError: The file cannot be created, written, closed, or replaced.
    """

def write_critical_temperature_domain_to_string(
    opdom: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> str:
    """
    Return the domain CSV with a header and one row per selected sample.
    Columns hold the sweep-parameter values and operational-status tag; temperature domains add critical temperature.
    The keyword options select status tags and which samples to write.

    Args:
        opdom: Domain samples to write.
        operational_tag: The tag used to represent the operational value of a parameter set.
        non_operational_tag: The tag used to represent the non-operational value of a parameter set.
        writing_mode: Write all samples or only operational samples.

    Returns:
        CSV text for the domain.

    Raises:
        ValueError: The domain does not have one to three sweep dimensions.
    """

def write_location_and_ground_state(
    sim_result: mnt.pyfiction.sidb.simulation.sidb_simulation_result, filename: str
) -> None:
    """
    Writes the SiDB positions of a simulation result and the charge states
    of its ground states as CSV.

    Args:
        sim_result: Result to write.
        filename: File to write into.

    Raises:
        RuntimeError: If creating, writing, closing, or replacing the
                      output file fails.
    """
