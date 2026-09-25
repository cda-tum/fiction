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

class displacement_robustness_domain_params:
    """Parameters of the displacement robustness analysis."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def analysis_mode(self) -> displacement_analysis_mode:
        """The analysis mode."""

    @analysis_mode.setter
    def analysis_mode(self, arg: displacement_analysis_mode, /) -> None: ...
    @property
    def percentage_of_analyzed_displaced_layouts(self) -> float:
        """Share of the displaced layouts to analyze in `RANDOM` mode."""

    @percentage_of_analyzed_displaced_layouts.setter
    def percentage_of_analyzed_displaced_layouts(self, arg: float, /) -> None: ...
    @property
    def displacement_variations(self) -> tuple[int, int]:
        """
        Maximum displacement in columns and rows; the displaced sites must
        remain representable.
        """

    @displacement_variations.setter
    def displacement_variations(self, arg: tuple[int, int], /) -> None: ...
    @property
    def operational_params(self) -> mnt.pyfiction.sidb.simulation.logic.is_operational_params:
        """
        Operational parameters. Their timeout bounds the entire displacement
        analysis across all layouts and workers. Finite budgets reject
        ClusterComplete and throw `utils::timeout_error` on expiration.
        """

    @operational_params.setter
    def operational_params(self, arg: mnt.pyfiction.sidb.simulation.logic.is_operational_params, /) -> None: ...
    @property
    def fixed_sidbs(self) -> set[mnt.pyfiction.sidb.lattice_site]:
        """SiDBs that are not displaced."""

    @fixed_sidbs.setter
    def fixed_sidbs(self, arg: AbstractSet[mnt.pyfiction.sidb.lattice_site], /) -> None: ...
    @property
    def dimer_policy(self) -> dimer_displacement_policy:
        """The dimer policy."""

    @dimer_policy.setter
    def dimer_policy(self, arg: dimer_displacement_policy, /) -> None: ...
    @property
    def number_of_threads(self) -> int:
        """Number of threads to use."""

    @number_of_threads.setter
    def number_of_threads(self, arg: int, /) -> None: ...
    @property
    def on_progress(self) -> Callable[[str, int, int], None] | None:
        """Callback that receives the number of analyzed displaced layouts."""

    @on_progress.setter
    def on_progress(self, arg: Callable[[str, int, int], None], /) -> None: ...

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
    params: displacement_robustness_domain_params = ...,
    stats: displacement_robustness_domain_stats | None = None,
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
    `percentage_of_analyzed_displaced_layouts` in `params`.

    Displaced layouts retain the layout name and the defects at their
    original positions.

    Args:
        lyt: The SiDB layout which is analyzed.
        spec: Vector of truth table specifications.
        params: Parameters for the displacement robustness computation.
        stats: Statistics related to the displacement robustness
               computation.

    Returns:
        The displacement robustness domain of the SiDB layout.

    Raises:
        std::out_of_range: if a displacement exceeds the lattice-site
                           range.
    """
