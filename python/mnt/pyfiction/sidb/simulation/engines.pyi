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
    *QuickExact* is a quick and exact physical simulation algorithm
    designed specifically for SiDB layouts. It was proposed in \\"The Need
    for Speed: Efficient Exact Simulation of Silicon Dangling Bond Logic\\"
    by J. Drewniok, M. Walter, and R. Wille in ASP-DAC 2024
    (https://ieeexplore.ieee.org/document/10473946). It determines all
    physically valid charge configurations of a given SiDB layout,
    providing a significant performance advantage of more than three
    orders of magnitude over *ExGS*.

    The performance improvement stems from the use of physically informed
    search space pruning: SiDBs whose local potential forces them negative
    are removed from the enumeration and modeled as fixed negative
    charges, one SiDB's charge state follows from the others, and
    positively charged SiDBs are enumerated only where they can occur.
    Charged surface defects of the layout and external potentials enter
    the potential landscape.

    Args:
        lyt: Layout to simulate.

    Returns:
        Simulation result: every physically valid charge distribution of
        `lyt`.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
        utils::timeout_error: if the shared caller deadline expires. No
                              partial result is returned.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    *QuickSim* is a heuristic ground-state simulation of SiDB layouts
    proposed in \\"QuickSim: Efficient and Accurate Physical Simulation of
    Silicon Dangling Bond Logic\\" by J. Drewniok, M. Walter, S. S. H. Ng,
    K. Walus, and R. Wille in IEEE NANO 2023
    (https://ieeexplore.ieee.org/document/10231266). It grows candidate
    charge distributions from the SiDBs that have to be negative by
    repeatedly charging the neutral SiDB farthest from every negative one,
    keeping the physically valid intermediate states. Every worker thread
    owns one simulation state and shares the layout's potential landscape.
    Charged surface defects are not supported: a layout that holds any
    yields `std::nullopt`.

    Args:
        lyt: Layout to simulate.

    Returns:
        The physically valid charge distributions found, or `std::nullopt`
        if the layout is empty, holds charged defects, the iteration count
        is zero, the timeout was hit, or no valid distribution was found.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
        utils::timeout_error: if the shared caller deadline expires. No
                              partial result is returned.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    *Exhaustive Ground State Simulation* (*ExGS*) which was proposed in
    \\"Computer-Aided Design of Atomic Silicon Quantum Dots and
    Computational Applications\\" by S. S. H. Ng
    (https://dx.doi.org/10.14288/1.0392909) enumerates every charge
    distribution of the layout in base 2 or 3 and keeps the physically
    valid ones. Charged surface defects of the layout enter the potential
    landscape.

    Args:
        lyt: Layout to simulate.
        on_progress: Callback that receives the number of enumerated
                     charge configurations.
        deadline: Shared caller deadline. `time_point::max()` leaves the
                  simulation unlimited.

    Returns:
        The physically valid charge distributions.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
        utils::timeout_error: if the shared caller deadline expires. No
                              partial result is returned.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    *ClusterComplete* is an instantiation of a general solution to
    exhaustive state assignment searching for which all local predicates
    hold, given respective local evaluations that may be aggregated from
    individual inter-variable interactions. Applied to the problem of
    exact physical simulation of SiDBs, it is able to efficiently consider
    positive charges that are rare to occur, but drastically blow up exact
    simulation runtimes when hierarchical pruning methods are not applied.
    In fact, the exponential growth in problem complexity for added SiDBs
    is tamed by *ClusterComplete*, as SiDB layouts to simulate in practise
    amount to a high pruning efficacy, resulting in a layout-dependent
    reduction of the simulation base. This amounts to an effective
    simulation base in the real number range :math:`[1,b]`, where
    :math:`b\\in\\{2,3\\}` is the given simulation base.

    The part of the *ClusterComplete* algorithm that is implemented in
    this file is the destructive phase of the procedure that employs the
    duality of construction and destruction, folding and unfolding. The
    phase preceding it is the key ingredient to the achieved efficiency:
    the *Ground State Space* algorithm, which constructs a minimized
    hierarchical search space of charge configurations that adhere to the
    critical population stability criterion. In particular, it generalizes
    physically informed space pruning that contributes to the capabilities
    of the *QuickExact* simulator, now applying to all charge states
    equally, and, most importantly, it lifts the associated potential
    equations to higher order, allowing us to reason over potential bounds
    in a cluster hierarchy.

    *ClusterComplete* was proposed in \\"Mastering the Exponential
    Complexity of Exact Physical Simulation of Silicon Dangling Bonds\\" by
    W. Lambooy, J. Drewniok, M. Walter, and R. Wille in ASP-DAC 2026.

    Args:
        lyt: Layout to simulate.

    Returns:
        Simulation results.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.

    Algorithm options are keyword-only; the signature lists their defaults.
    """
