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
import mnt.pyfiction.sidb.model
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

class quickexact_params:
    """This struct stores the parameters for the *QuickExact* algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """All parameters for physical SiDB simulations."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def base_number_detection(self) -> automatic_base_number_detection:
        """
        If `ON`, *QuickExact* checks which base number is required for the
        simulation, i.e., whether positively charged SiDBs can occur. If
        `OFF`, the base number from the physical parameters is used.
        """

    @base_number_detection.setter
    def base_number_detection(self, arg: automatic_base_number_detection, /) -> None: ...
    @property
    def local_external_potential(self) -> dict[mnt.pyfiction.sidb.lattice_site, float]:
        """
        Local external electrostatic potentials (e.g., locally applied
        electrodes), per site (unit: V).
        """

    @local_external_potential.setter
    def local_external_potential(self, arg: Mapping[mnt.pyfiction.sidb.lattice_site, float], /) -> None: ...
    @property
    def global_potential(self) -> float:
        """
        Global external electrostatic potential (unit: V). Value is applied on
        each SiDB.
        """

    @global_potential.setter
    def global_potential(self, arg: float, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the number of enumerated charge configurations."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

def quickexact(
    lyt: mnt.pyfiction.sidb.sidb_layout, params: quickexact_params = ...
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
        params: Parameter required for the simulation.

    Returns:
        Simulation result: every physically valid charge distribution of
        `lyt`.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
        utils::timeout_error: if the shared caller deadline expires. No
                              partial result is returned.
    """

class quicksim_params:
    """This struct stores the parameters for the *QuickSim* algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """Simulation parameters for the simulation of the physical SiDB system."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def iteration_steps(self) -> int:
        """Number of iterations to run the simulation for."""

    @iteration_steps.setter
    def iteration_steps(self, arg: int, /) -> None: ...
    @property
    def alpha(self) -> float:
        """`alpha` parameter for the *QuickSim* algorithm."""

    @alpha.setter
    def alpha(self, arg: float, /) -> None: ...
    @property
    def number_threads(self) -> int:
        """
        Number of threads to spawn. By default the number of threads is set to
        the number of available hardware threads.
        """

    @number_threads.setter
    def number_threads(self, arg: int, /) -> None: ...
    @property
    def timeout(self) -> int:
        """Timeout limit (in ms)."""

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the number of completed iterations across all
        threads.
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def on_worker_progress(self, /) -> Callable[[int, int, str, int, int, bool], None] | None:
        """
        Reports logical worker activity with a fixed worker count for each
        invocation.
        """

    @on_worker_progress.setter
    def on_worker_progress(self, value: Callable[[int, int, str, int, int, bool], None] | None) -> None: ...

def quicksim(
    lyt: mnt.pyfiction.sidb.sidb_layout, params: quicksim_params = ...
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
        ps: *QuickSim* parameters.

    Returns:
        The physically valid charge distributions found, or `std::nullopt`
        if the layout is empty, holds charged defects, the iteration count
        is zero, the timeout was hit, or no valid distribution was found.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
        utils::timeout_error: if the shared caller deadline expires. No
                              partial result is returned.
    """

def exhaustive_ground_state_simulation(
    lyt: mnt.pyfiction.sidb.sidb_layout,
    params: mnt.pyfiction.sidb.model.sidb_simulation_parameters = ...,
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
        params: Physical parameters.
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

class clustercomplete_params:
    """
    The struct containing the parameters both passed on to pre-simulator
    Ground State Space, and used during simulation.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """Physical simulation parameters."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def local_external_potential(self) -> dict[mnt.pyfiction.sidb.lattice_site, float]:
        """
        Local external electrostatic potentials (e.g., locally applied
        electrodes).
        """

    @local_external_potential.setter
    def local_external_potential(self, arg: Mapping[mnt.pyfiction.sidb.lattice_site, float], /) -> None: ...
    @property
    def global_potential(self) -> float:
        """
        Global external electrostatic potential. Value is applied to each dot
        in the layout.
        """

    @global_potential.setter
    def global_potential(self, arg: float, /) -> None: ...
    @property
    def validity_witness_partitioning_max_cluster_size_gss(self) -> int:
        """
        This specifies the maximum cluster size for which *Ground State Space*
        will solve an NP-complete sub-problem exhaustively. The sets of SiDBs
        that witness local population stability for each respective charge
        state may be partitioned into disjoint sets such that the number of
        required witnesses for each respective charge state is satisfied. If
        no such partition exists, the multiset charge configuration associated
        with the requirements may be rejected.
        """

    @validity_witness_partitioning_max_cluster_size_gss.setter
    def validity_witness_partitioning_max_cluster_size_gss(self, arg: int, /) -> None: ...
    @property
    def num_overlapping_witnesses_limit_gss(self) -> int:
        """
        The complexity is of validity witness partitioning bounded by a
        factorial in the number of overlapping witnesses. This parameter thus
        allows the validity witness partitioning procedure to perform the
        reduction to overlapping witnesses for larger cluster sizes that could
        be runtime-impairing, then limiting specifically the length of the
        input to the factorial call.
        """

    @num_overlapping_witnesses_limit_gss.setter
    def num_overlapping_witnesses_limit_gss(self, arg: int, /) -> None: ...
    @property
    def available_threads(self) -> int:
        """
        Number of threads to make available to *ClusterComplete* for the
        unfolding stage.
        """

    @available_threads.setter
    def available_threads(self, arg: int, /) -> None: ...
    @property
    def report_gss_stats(self) -> ground_state_space_reporting:
        """
        Option to decide if the *Ground State Space* statistics are reported
        to the standard output. By default, this option is disabled.
        """

    @report_gss_stats.setter
    def report_gss_stats(self, arg: ground_state_space_reporting, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the number of unfolded charge space
        compositions.
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def on_worker_progress(self, /) -> Callable[[int, int, str, int, int, bool], None] | None:
        """
        Reports logical worker activity with a fixed worker count for each
        invocation.
        """

    @on_worker_progress.setter
    def on_worker_progress(self, value: Callable[[int, int, str, int, int, bool], None] | None) -> None: ...

def clustercomplete(
    lyt: mnt.pyfiction.sidb.sidb_layout, params: clustercomplete_params = ...
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
        params: Parameter required for both the invocation of *Ground
                State Space*, and the simulation following.

    Returns:
        Simulation results.

    Raises:
        std::out_of_range: if a site has an invalid lattice basis index.
    """
