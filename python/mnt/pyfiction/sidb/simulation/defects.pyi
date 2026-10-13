# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Influence of atomic defects on SiDB layouts."""

import datetime
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
    def time_total(self) -> datetime.timedelta:
        """Total runtime."""

    @time_total.setter
    def time_total(self, arg: datetime.timedelta, /) -> None: ...
    @property
    def num_operational_sidb_displacements(self) -> int:
        """Number of operational displaced layouts."""

    @num_operational_sidb_displacements.setter
    def num_operational_sidb_displacements(self, arg: int, /) -> None: ...
    @property
    def num_non_operational_sidb_displacements(self) -> int:
        """Number of non-operational displaced layouts."""

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
        """The displaced layouts and their operational status."""

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
    During fabrication, SiDBs may not align precisely with their intended
    atomic positions, resulting in displacement. This means that an SiDB
    is fabricated close to the desired one, typically one or a few H-Si
    positions away. Consequently, depending on the fabrication speed, a
    certain number of SiDBs may experience displacement.

    This function determines the operational status of all possible
    displacements of the SiDBs of the given SiDB layout, based on the
    provided truth table specification and displacement robustness
    computation parameters. The number of displacements grows
    exponentially with the number of SiDBs. For small layouts, all
    displacements can be analyzed. For larger layouts, random sampling can
    be applied, controllable by the `analysis_mode` and
    `percentage_of_analyzed_displaced_layouts` in the keyword options.

    Displaced layouts retain the layout name and the defects at their
    original positions.

    Args:
        lyt: The SiDB layout which is analyzed.
        spec: Vector of truth table specifications.
        stats: Statistics related to the displacement robustness
               computation.

    Returns:
        The displacement robustness domain of the SiDB layout.

    Raises:
        std::out_of_range: if a displacement exceeds the lattice-site
                           range.

    Algorithm options are keyword-only; the signature lists their defaults.
    """
