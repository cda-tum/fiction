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

import datetime
import enum
from collections.abc import Callable
from typing import overload

import mnt.pyfiction._native.layouts
import mnt.pyfiction._native.networks

class GateCounts:
    """The number of nodes of each gate type in a network or layout."""

    def __init__(self) -> None:
        """Default constructor."""

    def report(self, detailed: bool = False) -> str: ...
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
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.networks.TechnologyNetwork) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.networks.AigNetwork) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.networks.XagNetwork) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.networks.MigNetwork) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.CartesianGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.HexagonalGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.OddRowHexGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.OddColumnHexGateLayout) -> GateCounts: ...
@overload
def count_gate_types(ntk_or_lyt: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout) -> GateCounts:
    """
    Gives a detailed listing of all gate types present in the provided
    network (or layout). This function can distinguish most gate types
    available as atomic building blocks and can easily be extended to
    support more gate types. The given network (or layout) has to
    implement a function to test whether a node is of the respective gate
    type.

    Args:
        ntk: The network (or layout).
        pst: Statistics.

    Template Args:
        Ntk: Logic network (or layout) type.
    """

@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.CartesianGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.HexagonalGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
) -> tuple[int, int]: ...
@overload
def critical_path_length_and_throughput(
    layout: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
) -> tuple[int, int]:
    """
    Computes the critical path length (CP) length and the throughput (TP)
    of a gate-level layout.

    The critical path length is defined as the longest path from any PI to
    any PO in tiles.

    The throughput is defined as :math:`\\frac{1}{x}` where :math:`x` is
    the highest path length difference between any sets of paths that lead
    to the same gate. This function provides only the denominator
    :math:`x`, as the numerator is always :math:`1`. Furthermore,
    :math:`x` is given in clock cycles rather than clock phases because it
    is assumed that a path length difference smaller than
    `lyt.num_clocks()` does not lead to any delay. Contrary, for any
    throughput value :math:`\\frac{1}{x}` with :math:`x > 1`, the layout
    computes its represented Boolean function only every :math:`x` full
    clock cycles after the first inputs have been propagated through the
    design. Thereby, all PIs need to be held constant for :math:`x` clock
    phases to ensure proper computation.

    For more information on the concept of throughput and delay see
    \\"Synchronization of Clocked Field-Coupled Circuits\\" by F. Sill
    Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO
    2018; or \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    The complexity of this function is :math:`\\mathcal{O}(|T|)` where
    :math:`T` is the set of all occupied tiles in `lyt`.

    Args:
        lyt: The gate-level layout whose CP and TP are desired.

    Template Args:
        Lyt: Gate-level layout type.

    Returns:
        A struct containing the CP and TP.
    """

class DesignRuleParams:
    """
    Parameters for design rule violation checking that specify the checks
    that are to be executed.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives completed work and the phase total."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def unplaced_nodes(self) -> bool:
        """Check for nodes without locations."""

    @unplaced_nodes.setter
    def unplaced_nodes(self, arg: bool, /) -> None: ...
    @property
    def placed_dead_nodes(self) -> bool:
        """Check for placed but dead nodes."""

    @placed_dead_nodes.setter
    def placed_dead_nodes(self, arg: bool, /) -> None: ...
    @property
    def non_adjacent_connections(self) -> bool: ...
    @non_adjacent_connections.setter
    def non_adjacent_connections(self, arg: bool, /) -> None: ...
    @property
    def missing_connections(self) -> bool:
        """Check for nodes without connections."""

    @missing_connections.setter
    def missing_connections(self, arg: bool, /) -> None: ...
    @property
    def crossing_gates(self) -> bool:
        """Check for wires that are crossing gates."""

    @crossing_gates.setter
    def crossing_gates(self, arg: bool, /) -> None: ...
    @property
    def clocked_data_flow(self) -> bool:
        """Check if all node connections obey the clocking scheme data flow."""

    @clocked_data_flow.setter
    def clocked_data_flow(self, arg: bool, /) -> None: ...
    @property
    def has_io(self) -> bool:
        """Check if the layout has I/Os."""

    @has_io.setter
    def has_io(self, arg: bool, /) -> None: ...
    @property
    def empty_io(self) -> bool:
        """Check if the I/Os are assigned to empty tiles."""

    @empty_io.setter
    def empty_io(self, arg: bool, /) -> None: ...
    @property
    def io_pins(self) -> bool:
        """Check if the I/Os are assigned to wire segments."""

    @io_pins.setter
    def io_pins(self, arg: bool, /) -> None: ...
    @property
    def border_io(self) -> bool:
        """Check if the I/Os are located at the layout's border."""

    @border_io.setter
    def border_io(self, arg: bool, /) -> None: ...

class DesignRuleResult:
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
    layout: mnt.pyfiction._native.layouts.CartesianGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    params: DesignRuleParams = ...,
    print_report: bool = False,
    statistics: DesignRuleResult | None = None,
) -> tuple[int, int]:
    """
    Performs design rule violation (DRV) checking on the given gate-level
    layout. The implementation of gate_level_layout allows for layouts
    with structural defects like the connection of non-adjacent tiles or
    connections that defy the clocking scheme. This function checks for
    such violations and documents them in the statistics. A brief report
    can be printed and more in-depth information including with error
    sites can be obtained from a generated json object.

    Furthermore, this function does not only find and log DRVs but can
    also warn for instances that are not per se errors but defy best
    practices of layout generation, e.g., I/Os not being placed at the
    layout borders.

    For this function to work, `detail::gate_level_drvs_impl` need to be
    declared as a `friend class` to the layout type that is going to be
    examined.

    Args:
        lyt: The gate-level layout that is to be examined for DRVs and
             warnings.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Gate-level layout type.
    """

class EquivalenceType(enum.Enum):
    """The different equivalence types possible."""

    NO = 0
    """`Spec` and `Impl` are logically not equivalent OR `Impl` has DRVs."""

    WEAK = 1
    """
    `Spec` and `Impl` are logically equivalent BUT `Impl` has a throughput
    of :math:`\\frac{1}{x}` with :math:`x > 1`.
    """

    STRONG = 2
    """
    `Spec` and `Impl` are logically equivalent AND `Impl` has a throughput
    of :math:`\\frac{1}{1}`.
    """

class EquivalenceResult:
    def __init__(self) -> None:
        """Default constructor."""

    @property
    def eq(self) -> EquivalenceType:
        """Stores the equivalence type."""

    @property
    def tp_spec(self) -> int:
        """Throughput values at which weak equivalence manifests."""

    @property
    def tp_impl(self) -> int:
        """Throughput values at which weak equivalence manifests."""

    @property
    def tp_diff(self) -> int:
        """Throughput values at which weak equivalence manifests."""

    @property
    def counter_example(self) -> list[bool]:
        """Stores a possible counter example."""

    @property
    def runtime(self) -> datetime.timedelta:
        """Stores the runtime."""

    @property
    def spec_drv_stats(self) -> DesignRuleResult:
        """Stores DRVs."""

    @property
    def impl_drv_stats(self) -> DesignRuleResult: ...

@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.networks.TechnologyNetwork,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.CartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.networks.TechnologyNetwork,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.CartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.ShiftedCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.HexagonalGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenRowCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnCartesianGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddRowHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.OddColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    implementation: mnt.pyfiction._native.layouts.EvenColumnHexGateLayout,
    statistics: EquivalenceResult | None = None,
) -> EquivalenceType:
    """
    Performs SAT-based equivalence checking between a specification of
    type `Spec` and an implementation of type `Impl`. Both `Spec` and
    `Impl` need to be network types (that is, gate-level layouts can be
    utilized as well).

    This implementation enables the comparison of two logic networks, a
    logic network and a gate-level layout or two gate-level layouts. Since
    gate-level layouts have a notion of timing that logic networks do not,
    this function does not simply prove logical equivalence but,
    additionally, takes timing aspects into account as well.

    Thereby, three different types of equivalences arise:

    - `NO` equivalence: Spec and Impl are not logically equivalent or one
      of them is a gate-level layout that contains
    DRVs and, thus, cannot be checked for equivalence.
    - `WEAK` equivalence: Spec and Impl are logically equivalent but
      either one of them is a gate-level layout with TP of
    :math:`\\frac{1}{x}` with :math:`x > 1` or both of them are gate-level
    layouts with TP of :math:`\\frac{1}{x}` and :math:`\\frac{1}{y}`,
    respectively, where :math:`x \\neq y`.
    - `STRONG` equivalence: Spec and Impl are logically equivalent and all
      involved gate-level layouts have TP of
    :math:`\\frac{1}{1}`.

    This approach was first proposed in \\"Verification for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in DAC 2020.

    Args:
        spec: The specification.
        impl: The implementation.
        pst: Statistics.

    Template Args:
        Spec: Specification type.
        Impl: Implementation type.

    Returns:
        The equivalence type of `spec` and `impl`.
    """
