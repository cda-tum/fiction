# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Influence of atomic defects on SiDB layouts."""

import datetime as dt
import enum
from collections.abc import Callable, Sequence
from collections.abc import Set as AbstractSet

import mnt.pyfiction.sidb
import mnt.pyfiction.sidb.simulation
import mnt.pyfiction.sidb.simulation.logic
import mnt.pyfiction.synthesis

class dimer_displacement_policy(enum.Enum):
    """Whether a displaced SiDB may leave its dimer."""

    STAY_ON_ORIGINAL_DIMER = 0
    """
    The SiDB stays on its dimer: only the two sites of the dimer are
    possible positions.
    """

    ALLOW_OTHER_DIMER = 1
    """The SiDB may be displaced across dimers."""

class displacement_analysis_mode(enum.Enum):
    """Whether every displaced layout is analyzed or only a random share."""

    EXHAUSTIVE = 0
    """All displaced layouts are analyzed."""

    RANDOM = 1
    """A random share of the displaced layouts is analyzed."""

class displacement_robustness_domain_stats:
    """Statistics of the displacement robustness analysis."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
        """Total runtime. Defaults to zero."""

    @time_total.setter
    def time_total(self, arg: dt.timedelta, /) -> None: ...
    @property
    def num_operational_sidb_displacements(self) -> int:
        """Number of operational displaced layouts. Defaults to 0."""

    @num_operational_sidb_displacements.setter
    def num_operational_sidb_displacements(self, arg: int, /) -> None: ...
    @property
    def num_non_operational_sidb_displacements(self) -> int:
        """Number of non-operational displaced layouts. Defaults to 0."""

    @num_non_operational_sidb_displacements.setter
    def num_non_operational_sidb_displacements(self, arg: int, /) -> None: ...

class displacement_robustness_domain:
    """
    The displacement robustness domain of an SiDB gate: every analyzed
    displaced layout together with its operational status.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def influence_information(
        self,
    ) -> list[tuple[mnt.pyfiction.sidb.sidb_layout, mnt.pyfiction.sidb.simulation.logic.operational_status]]:
        """The displaced layouts and their operational status. Empty by default."""

    @influence_information.setter
    def influence_information(
        self,
        arg: Sequence[tuple[mnt.pyfiction.sidb.sidb_layout, mnt.pyfiction.sidb.simulation.logic.operational_status]],
        /,
    ) -> None: ...

def determine_displacement_robustness_domain(
    layout: mnt.pyfiction.sidb.sidb_layout,
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    *,
    analysis_mode: displacement_analysis_mode = ...,
    displacement_variations: tuple[int, int] = (1, 0),
    timeout: int = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    sim_engine: mnt.pyfiction.sidb.simulation.sidb_simulation_engine = ...,
    threshold_bdl_interdistance: float = 2.0,
    minimum_distance: float = 0.75,
    maximum_distance: float = 1.5,
    input_bdl_config: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration = ...,
    op_condition: mnt.pyfiction.sidb.simulation.logic.operational_condition = ...,
    strategy_to_analyze_operational_status: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy = ...,
    fixed_sidbs: AbstractSet[mnt.pyfiction.sidb.lattice_site] | None = None,
    dimer_policy: dimer_displacement_policy = ...,
    number_of_threads: int | None = None,
    on_progress: Callable[[str, int, int], None] | None = None,
    stats: displacement_robustness_domain_stats | None = None,
    percentage_of_analyzed_displaced_layouts: float = 1.0,
) -> displacement_robustness_domain:
    """
    Determine robustness against displacement of SiDBs in a gate implementing the truth tables.
    The keyword options select displacement, sampling, fixed SiDBs, and operational analysis.

    Args:
        analysis_mode: The analysis mode.
        displacement_variations: Maximum displacement in columns and rows; the displaced sites must remain representable.
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        fixed_sidbs: SiDBs that are not displaced.
        dimer_policy: The dimer policy.
        number_of_threads: Number of workers; None uses the native default.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        stats: Statistics object filled by the algorithm; None discards statistics.
        percentage_of_analyzed_displaced_layouts: Fraction of displaced layouts analyzed in RANDOM mode, between 0 and 1.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
    """
