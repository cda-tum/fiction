# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""
Design rule, equivalence, and performance checks of gate-level layouts.
"""

import datetime as dt
import enum
from collections.abc import Callable, Sequence
from typing import overload

import mnt.pyfiction.layouts
import mnt.pyfiction.networks

class count_gate_types_stats:
    """
    Counts logic gates and placed wires, excluding primary terminals and network constants.
    """

    def __init__(self) -> None:
        """Default constructor."""

    def report(self, detailed: bool = False) -> str:
        """
        Return the gate-count report as a string.

        Args:
            detailed: List every gate type when True; otherwise combine less-common types under other. Defaults to False.

        Returns:
            The formatted report.
        """

    @property
    def num_fanout(self) -> int: ...
    @property
    def num_buf(self) -> int: ...
    @property
    def num_inv(self) -> int: ...
    @property
    def num_and2(self) -> int: ...
    @property
    def num_or2(self) -> int: ...
    @property
    def num_nand2(self) -> int: ...
    @property
    def num_nor2(self) -> int: ...
    @property
    def num_xor2(self) -> int: ...
    @property
    def num_xnor2(self) -> int: ...
    @property
    def num_lt2(self) -> int: ...
    @property
    def num_gt2(self) -> int: ...
    @property
    def num_le2(self) -> int: ...
    @property
    def num_ge2(self) -> int: ...
    @property
    def num_and3(self) -> int: ...
    @property
    def num_xor_and(self) -> int: ...
    @property
    def num_or_and(self) -> int: ...
    @property
    def num_onehot(self) -> int: ...
    @property
    def num_maj3(self) -> int: ...
    @property
    def num_gamble(self) -> int: ...
    @property
    def num_dot(self) -> int: ...
    @property
    def num_mux(self) -> int: ...
    @property
    def num_and_xor(self) -> int: ...
    @property
    def num_other(self) -> int: ...

@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.networks.technology_network) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.networks.aig_network) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.networks.xag_network) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.networks.mig_network) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.layouts.cartesian_gate_layout) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout) -> count_gate_types_stats: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction.layouts.hexagonal_gate_layout) -> count_gate_types_stats:
    """
    Return gate-type counts for a logic network or gate-level layout.
    Primary terminals and network constants do not enter the counts. Placed wires
    count as fanouts when they drive multiple inputs, or as buffers otherwise.

    Args:
        ntk_or_lyt: Network or gate-level layout to inspect.

    Returns:
        A count_gate_types_stats object containing the counts.
    """

@overload
def critical_path_length_and_throughput(layout: mnt.pyfiction.layouts.cartesian_gate_layout) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(layout: mnt.pyfiction.layouts.hexagonal_gate_layout) -> tuple[int, int]:
    """
    Return the critical path length and throughput denominator of a gate-level layout.
    The critical path counts every placed object on the longest path to a primary
    output, including wires and terminals. Only output dependencies enter the
    analysis. Traversal follows declared input ports, independent of physical
    adjacency and clocking legality; zero-input functions act as path sources.
    The throughput is one result per returned denominator of full clock cycles.

    Args:
        layout: Gate-level layout to analyze.

    Returns:
        A tuple (critical_path_length, throughput_denominator).

    Raises:
        ValueError: An output dependency has a disconnected input or a cycle.
    """

class gate_level_drv_stats:
    """Design rule report and issue counts."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def drvs(self) -> int:
        """Number of design rule violations."""

    @property
    def warnings(self) -> int:
        """Number of warnings."""

    @property
    def report(self) -> str:
        """The full design rule check report as a JSON string."""

@overload
def gate_level_drvs(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    outside_extent: bool = True,
    non_adjacent_connections: bool = True,
    missing_connections: bool = True,
    crossing_gates: bool = True,
    clocked_data_flow: bool = True,
    has_io: bool = True,
    border_io: bool = True,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    outside_extent: bool = True,
    non_adjacent_connections: bool = True,
    missing_connections: bool = True,
    crossing_gates: bool = True,
    clocked_data_flow: bool = True,
    has_io: bool = True,
    border_io: bool = True,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    outside_extent: bool = True,
    non_adjacent_connections: bool = True,
    missing_connections: bool = True,
    crossing_gates: bool = True,
    clocked_data_flow: bool = True,
    has_io: bool = True,
    border_io: bool = True,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
) -> tuple[int, int]:
    """
    Count design-rule violations in the gate-level layout.
    Returns (warnings, drvs); statistics receives their counts.
    The keyword options select which checks run.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        non_adjacent_connections: Check for nodes that are connected to non-adjacent ones.
        missing_connections: Check for nodes without connections.
        crossing_gates: Check for wires that are crossing gates.
        clocked_data_flow: Check if all node connections obey the clocking scheme data flow.
        has_io: Check if the layout has I/Os.
        border_io: Check whether primary inputs and outputs lie on the layout border.
        print_report: Print the design-rule report to Python stdout.
        statistics: Statistics object filled by the algorithm; None discards statistics.
        outside_extent: Check that every live object lies within the layout extent.
    """

class eq_type(enum.Enum):
    """Equivalence classification for logic and layout throughput."""

    NO = 0
    """
    `Spec` and `Impl` differ logically, contain required topology defects,
    or either layout has DRVs.
    """

    WEAK = 1
    """
    `Spec` and `Impl` are logically equivalent and have different
    throughput denominators.
    """

    STRONG = 2
    """
    `Spec` and `Impl` are logically equivalent and have equal throughput
    denominators.
    """

class equivalence_checking_stats:
    """Physical equivalence result, throughput, and diagnostics."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def eq(self) -> eq_type:
        """Stores the equivalence type."""

    @property
    def tp_spec(self) -> int:
        """Specification throughput denominator. Defaults to `1`."""

    @property
    def tp_impl(self) -> int:
        """Implementation throughput denominator. Defaults to `1`."""

    @property
    def tp_diff(self) -> int:
        """
        Absolute difference between the throughput denominators. Defaults to
        `0`.
        """

    @property
    def counter_example(self) -> list[bool]:
        """Stores a possible counter example."""

    @property
    def runtime(self) -> dt.timedelta:
        """Stores the runtime."""

    @property
    def spec_drv_stats(self) -> gate_level_drv_stats:
        """Design rule violations in the specification layout."""

    @property
    def impl_drv_stats(self) -> gate_level_drv_stats:
        """Design rule violations in the implementation layout."""

@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
    *,
    input_mapping: Sequence[int] = [],
) -> eq_type:
    """
    Check a specification against an implementation by SAT, taking layout timing into account.
    Each operand is a logic network or a gate-level layout; layout logic is extracted first. The inputs match by names
    unique on both sides, then by remaining declared positions, unless `input_mapping` names the specification input of
    every implementation input. The outputs match by names and positions either way. The approach was first proposed in
    "Verification for Field-coupled Nanocomputing Circuits" by M. Walter, R. Wille, F. Sill Torres, D. Große, and
    R. Drechsler in DAC 2020.

    Args:
        specification: The specification.
        implementation: The implementation.
        statistics: Statistics object filled by the check; None discards statistics.
        input_mapping: For every implementation input in declared order, the index of the specification input it reads.
            Empty matches the inputs by names and positions, which requires equal input counts. With a mapping, several
            implementation inputs may read the same specification input, as the duplicated inputs of a planarized layout
            do.

    Returns:
        `eq_type.NO` when the functions differ, the interfaces do not match, or a layout has design rule violations;
        `eq_type.STRONG` when the functions agree at equal throughput; `eq_type.WEAK` when they agree with different
        throughput.

    Raises:
        ValueError: If `input_mapping` is non-empty and does not name one specification input for every implementation
            input.
    """
