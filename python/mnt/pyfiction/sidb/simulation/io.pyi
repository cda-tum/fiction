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
        std::invalid_argument: if a distribution has different sites from
                               the result layout.
        std::ofstream::failure: if the file cannot be opened.
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
    Writes a CSV representation of an operational domain to the specified
     output stream. The data are written as rows, each corresponding to
     one set of simulation parameters and their corresponding operational
     status.

     The output CSV format is e.g. as follows:
    ```
    embed:rst
    .. code-block:: RST

    epsilon_r, lambda_tf, operational status
    0.0, 0.0, 0
    0.1, 0.0, 1
    ... subsequent rows for each set of simulation parameters
    ```




     The operational status is a binary value represented by specified
     tags in the keyword options indicating whether the simulation parameters are
     within the operational domain or not.

    Args:
        opdom: The operational domain to be written. It represents a
               mapping between sets of simulation parameters (one to three
               sweep parameters, written as the X, Y, and Z columns) and a
               tuple containing detailed information about the SiDB layout
               associated with those simulation parameters.
        os: The output stream where the CSV representation of the
            operational domain is written to.

    Raises:
        std::invalid_argument: if the number of dimensions in the
                               operational domain is 0 or greater than 3.
                               Three is the number of enumerators of
                               `sweep_parameter`, so a fourth dimension
                               could only repeat one of them.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

def write_operational_domain_to_string(
    opdom: mnt.pyfiction.sidb.simulation.logic.operational_domain,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> str: ...
def write_critical_temperature_domain(
    opdom: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain,
    filename: str,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> None:
    """
    The `critical_temperature_domain` class collects the critical
    temperature and the operational status for a range of different
    physical parameters of a given SiDB layout. It allows for the
    evaluation of how the critical temperature depends on variations in
    the underlying parameter points. This enables simulations to explore
    the critical temperature's behavior across different conditions and
    configurations.

    It was proposed in \\"The Operational Domain Explorer: A Comprehensive
    Framework to Unveil the Thermal Landscape of Silicon Dangling Bond
    Logic Beyond Conventional Operability\\" by M. Walter, J. Drewniok, and
    R. Wille in IEEE-NANO 2025
    (https://ieeexplore.ieee.org/abstract/document/11113672).
    """

def write_critical_temperature_domain_to_string(
    opdom: mnt.pyfiction.sidb.simulation.logic.critical_temperature_domain,
    *,
    operational_tag: str = "1",
    non_operational_tag: str = "0",
    writing_mode: sample_writing_mode = ...,
) -> str: ...
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
        std::ofstream::failure: if the file cannot be opened.
    """
