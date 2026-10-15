# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical simulation engines for SiDB layouts."""

import enum
from collections.abc import Callable, Mapping

import mnt.pyfiction.sidb
import mnt.pyfiction.sidb.simulation

class automatic_base_number_detection(enum.Enum):
    """Modes to use for the *QuickExact* algorithm."""

    ON = 0
    """
    Simulation is conducted with the required base number (i.e., if
    positively charged SiDBs can occur, three state simulation is
    conducted).
    """

    OFF = 1
    """
    The base number from the physical parameters is used for the
    simulation.
    """

def quickexact(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    base_number_detection: automatic_base_number_detection = ...,
    local_external_potential: Mapping[mnt.pyfiction.sidb.lattice_site, float] | None = None,
    global_potential: float = 0.0,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> mnt.pyfiction.sidb.simulation.sidb_simulation_result:
    """
    Simulate the SiDB layout with QuickExact.
    The keyword options select physical values, base detection, external potentials, and progress reporting.
    Raises ValueError when base is neither 2 nor 3.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        base_number_detection: ON detects whether positive charges can occur; OFF uses base unchanged.
        local_external_potential: External electrostatic potential per lattice site in V; None uses an empty map.
        global_potential: External electrostatic potential applied to every SiDB in V.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    """

def quicksim(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    iteration_steps: int = 80,
    alpha: float = 0.7,
    number_threads: int | None = None,
    timeout: int = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
) -> mnt.pyfiction.sidb.simulation.sidb_simulation_result | None:
    """
    Search for low-energy charge distributions with QuickSim.
    The keyword options select physical values, iterations, threads, runtime, and progress callbacks.
    Returns None when the heuristic search finds no physically valid charge distribution.
    Raises ValueError when base is neither 2 nor 3.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        iteration_steps: Number of iterations to run the simulation for.
        alpha: `alpha` parameter for the *QuickSim* algorithm.
        number_threads: Number of QuickSim workers; None uses the available hardware threads.
        timeout: Execution budget in milliseconds.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    """

def exhaustive_ground_state_simulation(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> mnt.pyfiction.sidb.simulation.sidb_simulation_result:
    """
    Find all minimum-energy charge distributions by exhaustive simulation.
    The physical model is supplied by the scalar keyword values.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    """

class ground_state_space_reporting(enum.Enum):
    """
    This enum class provides meaningful options for configuring the
    reporting of the *Ground State Space* statistics. These statistic may
    be used especially to configure the validity witness partitioning
    options for *Ground State Space*, that may impair runtimes when set
    too high, but could provide a large benefit to the complexity of the
    unfolding process of large simulation problems by performing more
    involved pruning procedures in the construction stage.
    """

    ON = 0
    """
    Enabling this option will output *Ground State Space* statistics to
    the standard output.
    """

    OFF = 1
    """
    Disabling this option will suppress the output of *Ground State Space*
    statistics.
    """

def clustercomplete(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    *,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    local_external_potential: Mapping[mnt.pyfiction.sidb.lattice_site, float] | None = None,
    global_potential: float = 0.0,
    num_overlapping_witnesses_limit_gss: int = 6,
    available_threads: int | None = None,
    report_gss_stats: ground_state_space_reporting = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    validity_witness_partitioning_max_cluster_size_gss: int = 6,
) -> mnt.pyfiction.sidb.simulation.sidb_simulation_result:
    """
    Simulate the SiDB layout with ClusterComplete.
    The keyword options select physical values, clustering, witness searches, and available threads.
    Raises ValueError when base is neither 2 nor 3.

    Args:
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        local_external_potential: External electrostatic potential per lattice site in V; None uses an empty map.
        global_potential: External electrostatic potential applied to every SiDB in V.
        num_overlapping_witnesses_limit_gss: Maximum overlapping witnesses considered by validity witness partitioning.
        available_threads: Threads available for ClusterComplete unfolding; None uses the native default.
        report_gss_stats: Print Ground State Space statistics when ON.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        validity_witness_partitioning_max_cluster_size_gss: Maximum cluster size for validity witness partitioning.
    """
