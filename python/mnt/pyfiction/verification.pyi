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
        Prints gate counts. @param out Output stream. @param detailed Whether
        to print every gate type.
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
    Gives a detailed listing of all gate types present in the provided
    network (or layout). This function can distinguish most gate types
    available as atomic building blocks. Primary terminals and network
    constants do not enter the counts. Placed wires count as fanout
    objects when they drive multiple inputs, or as buffers otherwise. The
    given network (or layout) has to implement a function to test whether
    a node is of the respective gate type.

    Args:
        ntk: The network (or layout).
        pst: Statistics.

    Template Args:
        Ntk: Logic network (or layout) type.
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
    Computes the critical path length (CP) length and the throughput (TP)
    of a gate-level layout.

    The critical path length counts every placed object on the longest
    path to any PO, including wires and terminals. Traversal follows
    declared input ports, independent of physical adjacency and clocking
    legality. Only output dependencies enter the analysis. Explicit placed
    zero-input functions act as path sources.

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

    Raises:
        std::invalid_argument: If an output dependency has a disconnected
                               input or a cycle.
    """

class gate_level_drv_params:
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
    def outside_extent(self) -> bool:
        """Checks that every live object lies within the layout extent."""

    @outside_extent.setter
    def outside_extent(self, arg: bool, /) -> None: ...
    @property
    def non_adjacent_connections(self) -> bool:
        """Check for nodes that are connected to non-adjacent ones."""

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
    def border_io(self) -> bool:
        """Check if the I/Os are located at the layout's border."""

    @border_io.setter
    def border_io(self, arg: bool, /) -> None: ...

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
    params: gate_level_drv_params = ...,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    params: gate_level_drv_params = ...,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
) -> tuple[int, int]: ...
@overload
def gate_level_drvs(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    params: gate_level_drv_params = ...,
    print_report: bool = False,
    statistics: gate_level_drv_stats | None = None,
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

    The checker inspects every live object through public ordered ports,
    including placements outside the extent. Unplaced objects, placed dead
    objects, empty terminals, and gate terminals cannot occur in the
    placed-object API and have no corresponding checks.

    Args:
        lyt: The gate-level layout that is to be examined for DRVs and
             warnings.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Gate-level layout type.
    """

class eq_type(enum.Enum):
    """The different equivalence types possible."""

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
    def spec_drv_stats(self) -> gate_level_drv_stats:
        """Stores DRVs."""

    @property
    def impl_drv_stats(self) -> gate_level_drv_stats: ...

@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.networks.technology_network,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.networks.technology_network,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type: ...
@overload
def equivalence_checking(
    specification: mnt.pyfiction.layouts.hexagonal_gate_layout,
    implementation: mnt.pyfiction.layouts.hexagonal_gate_layout,
    statistics: equivalence_checking_stats | None = None,
) -> eq_type:
    """
    Performs SAT-based equivalence checking between a specification of
    type `Spec` and an implementation of type `Impl`. Each operand is a
    logic network or a placed gate-level layout. Layout logic is extracted
    before SAT checking. Interfaces match by names unique on both sides,
    then by remaining declared positions. Unequal interface sizes, missing
    required inputs, and required dependency cycles return `NO`.

    This implementation enables the comparison of two logic networks, a
    logic network and a gate-level layout or two gate-level layouts. Since
    gate-level layouts have a notion of timing that logic networks do not,
    this function does not simply prove logical equivalence but,
    additionally, takes timing aspects into account as well.

    Thereby, three different types of equivalences arise:

    - `NO` equivalence: Spec and Impl are not logically equivalent or one
      of them is a gate-level layout that contains
    DRVs and, thus, cannot be checked for equivalence.
    - `WEAK` equivalence: Spec and Impl are logically equivalent and have
      different throughput denominators. - `STRONG` equivalence: Spec and
      Impl are logically equivalent and have equal throughput
      denominators.
    Logic networks have throughput denominator one.

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
