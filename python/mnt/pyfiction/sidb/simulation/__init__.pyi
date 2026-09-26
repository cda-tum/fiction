# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""SiDB simulation engines, results, and their analysis."""

import datetime
import enum
from collections.abc import Mapping, Sequence
from typing import overload

import mnt.pyfiction.sidb
import mnt.pyfiction.sidb.model
from mnt.pyfiction.sidb.simulation import analysis as analysis
from mnt.pyfiction.sidb.simulation import defects as defects
from mnt.pyfiction.sidb.simulation import engines as engines
from mnt.pyfiction.sidb.simulation import io as io
from mnt.pyfiction.sidb.simulation import logic as logic

class sidb_simulation_engine(enum.Enum):
    """Selector for the available SiDB simulation engines."""

    EXGS = 0
    """
    *Exhaustive Ground State Search (EXGS)* is an exact simulation engine
    that always has exponential runtime.
    """

    QUICKSIM = 1
    """
    *QuickSim* is a heuristic simulation engine that only requires
    polynomial runtime.
    """

    QUICKEXACT = 2
    """
    *QuickExact* is also an exact simulation engine that requires
    exponential runtime, but it scales a lot better than *ExGS* due to its
    effective search-space pruning.
    """

    CLUSTERCOMPLETE = 3
    """
    *ClusterComplete* is a novel exact simulation engine that requires
    exponential runtime, though, depending on the simulation problem, it
    effectively reduces the base number by a real number, thus allowing
    problem sizes that were previously considered astronomical in size.
    Inherent to the simulation methodology that does not depend on the
    simulation base, it simulates very effectively for either base number
    (2 or 3).
    """

class exact_sidb_simulation_engine(enum.Enum):
    """Selector exclusively for exact SiDB simulation engines."""

    EXGS = 0
    """
    *Exhaustive Ground State Search* (EXGS) is an exact simulation engine
    that always has exponential runtime.
    """

    QUICKEXACT = 1
    """
    *QuickExact* is also an exact simulation engine that requires
    exponential runtime, but it scales a lot better than ExGS due to its
    effective search-space pruning.
    """

    CLUSTERCOMPLETE = 2
    """
    *ClusterComplete* is a novel exact simulation engine that requires
    exponential runtime, though, depending on the simulation problem, it
    effectively reduces the base number by a real number, thus allowing
    problem sizes that were previously considered astronomical. Inherent
    to the simulation methodology that does not depend on the simulation
    base, it simulates very effectively for either base number (2 or 3).
    """

class heuristic_sidb_simulation_engine(enum.Enum):
    """Selector exclusively for heuristic SiDB simulation engines."""

    QUICKSIM = 0
    """
    *QuickSim* is a heuristic simulation engine that only requires
    polynomial runtime.
    """

@overload
def sidb_simulation_engine_name(engine: sidb_simulation_engine) -> str: ...
@overload
def sidb_simulation_engine_name(engine: exact_sidb_simulation_engine) -> str: ...
@overload
def sidb_simulation_engine_name(engine: heuristic_sidb_simulation_engine) -> str:
    """
    Returns the name of the given simulation engine.

    Args:
        eng: An SiDB simulation engine.

    Template Args:
        EngineType: The type of the SiDB simulation engine
                    (exhaustive/heuristic/generic).

    Returns:
        The name of the simulation engine.
    """

class charge_transition_threshold_bounds(enum.Enum):
    """
    The bounds a local potential is tested against to decide whether a
    charge state is population stable.
    """

    NEGATIVE_UPPER_BOUND = 0
    """The upper bound check against `mu_minus` to validate DB-."""

    POSITIVE_LOWER_BOUND = 1
    """The lower bound check against `mu_plus` to validate DB+."""

    NEUTRAL_LOWER_BOUND = 2
    """The lower bound check against `mu_minus` to validate DB0."""

    NEUTRAL_UPPER_BOUND = 3
    """The upper bound check against `mu_plus` to validate DB0."""

class potential_landscape:
    """
    The static electrostatics of an SiDB layout under one set of physical
    parameters: the screened Coulomb potential every SiDB exerts on every
    other, the potential the charged surface defects and the external
    sources exert on every SiDB, and the resulting charge transition
    thresholds. Nothing in it depends on the charge states, so one
    landscape serves every charge distribution of the layout and can be
    shared read-only by worker threads.

    Distances are stored in nm, potentials in V, energies in eV.
    """

    def __init__(
        self,
        layout: mnt.pyfiction.sidb.sidb_layout,
        params: mnt.pyfiction.sidb.model.sidb_simulation_parameters = ...,
        local_external_potential: Mapping[mnt.pyfiction.sidb.lattice_site, float] = {},
        global_external_potential: float = 0.0,
    ) -> None:
        """
        Builds the landscape of a layout.

        Args:
            lyt: Layout; its charged surface defects enter the landscape.
            params: Physical parameters.
            local_external_potential: External potential per site (unit: V). A
                                      site holding a defect shifts the
                                      potential at that defect; a site holding
                                      an SiDB shifts the potential at that
                                      SiDB.
            global_external_potential: External potential applied to every
                                       SiDB (unit: V).

        Raises:
            std::out_of_range: if a SiDB or defect site has an invalid lattice
                               basis index.
        """

    def get_layout(self) -> mnt.pyfiction.sidb.sidb_layout:
        """
        The layout the landscape describes.

        Returns:
            The layout.
        """

    def params(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """
        The physical parameters.

        Returns:
            The parameters.
        """

    def num_sidbs(self) -> int:
        """
        Number of SiDBs.

        Returns:
            Number of SiDBs.
        """

    def sites(self) -> list[mnt.pyfiction.sidb.lattice_site]:
        """
        The SiDB sites in raster order, shared with the charge distributions
        built over this landscape.

        Returns:
            The shared site list.
        """

    def defects(self) -> list[tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.model.sidb_defect]]:
        """
        The charged surface defects that enter the landscape.

        Returns:
            Site-defect pairs.
        """

    def nm_distance(self, i: int, j: int) -> float:
        """
        Distance between two SiDBs.

        Args:
            i: Index of the first SiDB.
            j: Index of the second SiDB.

        Returns:
            Distance (unit: nm).

        Raises:
            std::out_of_range: if an SiDB index is out of range.
        """

    def chargeless_potential(self, i: int, j: int) -> float:
        """
        The chargeless potential one SiDB exerts on another, i.e., the
        potential of a unit charge at the distance of the two.

        Args:
            i: Index of the SiDB the potential acts on.
            j: Index of the SiDB that exerts it.

        Returns:
            Chargeless potential (unit: V).

        Raises:
            std::out_of_range: if an SiDB index is out of range.
        """

    def local_external_potential(self, i: int) -> float:
        """
        The external potential at an SiDB: its local external potential plus
        the global one.

        Args:
            i: Index of the SiDB.

        Returns:
            External potential (unit: V).

        Raises:
            std::out_of_range: if an SiDB index is out of range.
        """

    def local_potential_caused_by_defects(self, i: int) -> float:
        """
        The potential the charged surface defects exert on an SiDB.

        Args:
            i: Index of the SiDB.

        Returns:
            Defect potential (unit: V).

        Raises:
            std::out_of_range: if an SiDB index is out of range.
        """

    def effective_charge_transition_thresholds(self, i: int) -> list[float]:
        """
        The charge transition thresholds of an SiDB, indexed by
        `charge_transition_threshold_bounds`.

        Args:
            i: Index of the SiDB.

        Returns:
            The four thresholds (unit: V).

        Raises:
            std::out_of_range: if an SiDB index is out of range.
        """

    def chargeless_potential_at_distance(self, distance: float) -> float:
        """
        The chargeless potential of a unit charge at a distance under the
        landscape's parameters.

        Args:
            distance: Distance (unit: nm).

        Returns:
            Chargeless potential (unit: V); 0 at distance 0.
        """

    def local_internal_potentials(self, cd: mnt.pyfiction.sidb.charge_distribution) -> list[float]:
        """
        The local internal potentials of a charge distribution: at every SiDB,
        the potential of the charged SiDBs plus the potential of the charged
        defects. O(N²).

        Args:
            cd: Charge distribution over this landscape's SiDBs.

        Returns:
            Local internal potential per SiDB (unit: V).

        Raises:
            std::invalid_argument: if the distribution sites differ from the
                                   landscape.
        """

    def local_potentials(self, cd: mnt.pyfiction.sidb.charge_distribution) -> list[float]:
        """
        The local potentials of a charge distribution: internal plus external
        potential per SiDB. O(N²).

        Args:
            cd: Charge distribution over this landscape's SiDBs.

        Returns:
            Local potential per SiDB (unit: V).

        Raises:
            std::invalid_argument: if the distribution sites differ from the
                                   landscape.
        """

    def energy(self, cd: mnt.pyfiction.sidb.charge_distribution) -> float:
        """
        The electrostatic potential energy of a charge distribution. O(N²).

        Args:
            cd: Charge distribution over this landscape's SiDBs.

        Returns:
            Energy (unit: eV).

        Raises:
            std::invalid_argument: if the distribution sites differ from the
                                   landscape.
        """

    def is_physically_valid(self, cd: mnt.pyfiction.sidb.charge_distribution) -> bool:
        """
        Whether a charge distribution is physically valid. O(N²).

        Args:
            cd: Charge distribution over this landscape's SiDBs.

        Returns:
            `true` iff both stability criteria hold.

        Raises:
            std::invalid_argument: if the distribution sites differ from the
                                   landscape.
        """

    def evaluate(self, cd: mnt.pyfiction.sidb.charge_distribution) -> mnt.pyfiction.sidb.charge_distribution:
        """
        Evaluates a charge distribution: returns a copy with its energy set.
        O(N²).

        Args:
            cd: Charge distribution over this landscape's SiDBs.

        Returns:
            `cd` with its energy assigned.

        Raises:
            std::invalid_argument: if the distribution sites differ from the
                                   landscape.
        """

class sidb_simulation_result:
    """
    The unified return type of every SiDB simulation algorithm: the name
    of the algorithm, its runtime, the layout it simulated, the physically
    valid charge distributions it found over that layout, the physical
    parameters, and optional algorithm-specific named parameters.

    The layout is stored once; each charge distribution holds one charge
    state per SiDB in the layout's raster order plus its energy.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def algorithm_name(self) -> str:
        """Name of the algorithm used to determine the charge distributions."""

    @algorithm_name.setter
    def algorithm_name(self, arg: str, /) -> None: ...
    @property
    def simulation_runtime(self) -> datetime.timedelta:
        """Total simulation runtime in seconds."""

    @simulation_runtime.setter
    def simulation_runtime(self, arg: datetime.timedelta, /) -> None: ...
    @property
    def layout(self) -> mnt.pyfiction.sidb.sidb_layout:
        """The simulated layout."""

    @layout.setter
    def layout(self, arg: mnt.pyfiction.sidb.sidb_layout, /) -> None: ...
    @property
    def charge_distributions(self) -> list[mnt.pyfiction.sidb.charge_distribution]:
        """Charge distributions determined by the algorithm."""

    @charge_distributions.setter
    def charge_distributions(self, arg: Sequence[mnt.pyfiction.sidb.charge_distribution], /) -> None: ...
    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """Physical parameters used in the simulation."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def additional_simulation_parameters(self) -> dict[str, int | float | bool | str]:
        """
        Additional named simulation parameters. This is used to store
        algorithm-dependent parameters that are not part of the
        `sidb::model::simulation_parameters` struct.

        The key of the map is the name of the parameter, the element is the
        value of the parameter.
        """

    def charge_state(
        self, distribution: int, site: mnt.pyfiction.sidb.lattice_site
    ) -> mnt.pyfiction.sidb.model.sidb_charge_state:
        """
        The charge state of an SiDB in one of the charge distributions.

        Args:
            distribution: Index into `charge_distributions`.
            s: Site of the SiDB.

        Returns:
            The charge state, `charge_state::NONE` if `s` holds no SiDB.

        Raises:
            std::out_of_range: if the distribution index is out of range.
        """

    def groundstates(self) -> list[mnt.pyfiction.sidb.charge_distribution]:
        """
        The charge distributions of minimal energy. Distributions with
        identical charge states count once.

        Returns:
            The charge distributions with the minimal energy.

        Note:
            If degenerate states exist in the simulation result, this function
            returns multiple ground states that all possess the same energy.
        """

def is_ground_state(heuristic_results: sidb_simulation_result, exhaustive_results: sidb_simulation_result) -> bool:
    """
    Whether a heuristic simulation found every ground state an exact
    simulation found: the two results have the same number of ground
    states and every exact ground state appears among the heuristic ones.

    Args:
        heuristic_results: Result of a heuristic simulation.
        exact_results: Result of an exact simulation of the same layout.

    Returns:
        `true` iff the heuristic result contains every ground state of the
        exact one.
    """

def check_simulation_results_for_equivalence(result1: sidb_simulation_result, result2: sidb_simulation_result) -> bool:
    """
    Whether two simulation results of the same layout are equivalent: the
    same number of distinct charge distributions, and, sorted by charge
    states, pairwise the same charge states and energies within
    `ERROR_MARGIN`.

    Args:
        result1: First result.
        result2: Second result.

    Returns:
        `true` iff both results hold the same charge distributions.
    """
