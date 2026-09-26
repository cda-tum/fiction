# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Placement and routing of logic networks into gate-level layouts."""

import datetime
import enum
from collections.abc import Callable, Sequence
from typing import overload

import mnt.pyfiction.inml
import mnt.pyfiction.layouts
import mnt.pyfiction.layouts.coords
import mnt.pyfiction.mol_qca
import mnt.pyfiction.networks
import mnt.pyfiction.qca
import mnt.pyfiction.sidb
from mnt.pyfiction.physical_design import path_finding as path_finding

class technology_constraints(enum.Enum):
    """Target technologies."""

    NONE = 0
    """No technology-specific constraints."""

    TOPOLINANO = 1
    """ToPoLiNano technology-specific constraints."""

class exact_params:
    """Parameters for the exact physical design algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def scheme(self) -> str:
        """Clocking scheme to be used."""

    @scheme.setter
    def scheme(self, arg: str, /) -> None: ...
    @property
    def upper_bound_area(self) -> int:
        """
        Number of total tiles to use as an upper bound.

        Note:
            If `upper_bound_area` and (either) `upper_bound_x` or
            `upper_bound_y` are set, the imposed search space restrictions are
            cumulative. E.g., if `upper_bound_area == 20` and `upper_bound_x
            == 4`, all aspect ratios with an x-dimension of more than 4 *and*
            a total area of more than 20 will be skipped.
        """

    @upper_bound_area.setter
    def upper_bound_area(self, arg: int, /) -> None: ...
    @property
    def upper_bound_x(self) -> int:
        """Number of tiles to use as an upper bound in x direction."""

    @upper_bound_x.setter
    def upper_bound_x(self, arg: int, /) -> None: ...
    @property
    def upper_bound_y(self) -> int:
        """Number of tiles to use as an upper bound in y direction."""

    @upper_bound_y.setter
    def upper_bound_y(self, arg: int, /) -> None: ...
    @property
    def fixed_size(self) -> bool:
        """
        Exclusively investigate aspect ratios that conform with the
        restrictions imposed by the upper bound options. E.g., if `fixed_size
        == true` *and* `upper_bound_area == 20`, only aspect ratios with
        exactly 20 tiles will be examined. Restricted imposed by the
        `upper_bound_x` and `upper_bound_y` flags additionally apply.
        """

    @fixed_size.setter
    def fixed_size(self, arg: bool, /) -> None: ...
    @property
    def num_threads(self) -> int:
        """
        Number of threads to use for exploring the possible aspect ratios.

        Note:
            This is an unstable beta feature.
        """

    @num_threads.setter
    def num_threads(self, arg: int, /) -> None: ...
    @property
    def synchronization_elements(self) -> bool: ...
    @synchronization_elements.setter
    def synchronization_elements(self, arg: bool, /) -> None: ...
    @property
    def crossings(self) -> bool:
        """Flag to indicate that crossings may be used."""

    @crossings.setter
    def crossings(self, arg: bool, /) -> None: ...
    @property
    def border_io(self) -> bool:
        """Flag to indicate that I/Os should be placed at the layout's border."""

    @border_io.setter
    def border_io(self, arg: bool, /) -> None: ...
    @property
    def straight_inverters(self) -> bool:
        """
        Flag to indicate that straight inverters should be used over bend
        ones.
        """

    @straight_inverters.setter
    def straight_inverters(self, arg: bool, /) -> None: ...
    @property
    def desynchronize(self) -> bool:
        """
        Flag to indicate that a discrepancy in fan-in path lengths is allowed
        (reduces runtime!).
        """

    @desynchronize.setter
    def desynchronize(self, arg: bool, /) -> None: ...
    @property
    def minimize_wires(self) -> bool:
        """Minimize the number of wire tiles."""

    @minimize_wires.setter
    def minimize_wires(self, arg: bool, /) -> None: ...
    @property
    def minimize_crossings(self) -> bool:
        """
        Flag to indicate that the number of used crossing tiles should be
        minimized.
        """

    @minimize_crossings.setter
    def minimize_crossings(self, arg: bool, /) -> None: ...
    @property
    def timeout(self) -> int:
        """Timeout budget for the solving process, in milliseconds."""

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def technology_specifics(self) -> technology_constraints:
        """
        Technology-specific constraints that are only to be added for a
        certain target technology.
        """

    @technology_specifics.setter
    def technology_specifics(self, arg: technology_constraints, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the number of examined aspect ratios."""

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

class exact_stats:
    """Statistics."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta: ...
    @property
    def x_size(self) -> int: ...
    @property
    def y_size(self) -> int: ...
    @property
    def num_gates(self) -> int: ...
    @property
    def num_wires(self) -> int: ...
    @property
    def num_crossings(self) -> int: ...
    @property
    def num_aspect_ratios(self) -> int: ...

def exact_cartesian(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.cartesian_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_shifted_cartesian(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.shifted_cartesian_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_hexagonal(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_odd_row_cartesian(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.odd_row_cartesian_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_even_row_cartesian(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.even_row_cartesian_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_even_column_cartesian(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.even_column_cartesian_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_odd_row_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.odd_row_hex_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_odd_column_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.odd_column_hex_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

def exact_even_column_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: exact_params = ...,
    statistics: exact_stats | None = None,
) -> mnt.pyfiction.layouts.even_column_hex_gate_layout | None:
    """
    An exact placement & routing approach using SMT solving as originally
    proposed in \\"An Exact Method for Design Exploration of Quantum-dot
    Cellular Automata\\" by M. Walter, R. Wille, D. Große, F. Sill Torres,
    and R. Drechsler in DATE 2018. A more extensive description can be
    found in \\"Design Automation for Field-coupled Nanotechnologies\\" by
    M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by
    Springer Nature in 2022.

    Via incremental SMT calls, an optimal gate-level layout for a given
    logic network will be found under constraints. Starting with :math:`n`
    tiles, where :math:`n` is the number of logic network nodes, each
    possible layout aspect ratio will be examined by factorization and
    tested for routability with the SMT solver Z3. When no upper bound is
    given, this approach will run until it finds a solution to the
    placement & routing problem instance.

    Note that there a combinations of constraints for which no valid
    solution under the given parameters exist for the given logic network.
    Such combinations cannot be detected automatically. It is, thus,
    recommended to always set a timeout. Recommended settings include the
    use of I/O pins located at the layout borders for better integration.
    Most networks are not realizable without crossings enabled. Specifying
    a regular clocking scheme SIGNIFICANTLY speeds up the process. 2DDWave
    allows for the strictest constraints and, thereby, finds a solution
    the quickest. However, for high input degree networks, no valid
    solution exists when border I/Os are to be used unless global
    synchronization is disabled. Generally, solutions are found the
    fastest with the following settings: Crossings enabled, de-
    synchronization enabled, and 2DDWave clocking given. Multi-threading
    can sometimes speed up the process, especially for large networks.
    Note that the more threads are being used, the less information can be
    shared across the individual solver runs which destroys the benefits
    of incremental solving and thereby, comparatively, slows down each
    run.

    The SMT instance works with a single layer of variables even though it
    is possible to allow crossings in the solution. The reduced number of
    variables saves a considerable amount of runtime. That's why
    `layout.foreach_ground_tile()` is used even though the model will be
    mapped to a 3-dimensional layout afterwards. Generally, the algorithm
    incorporates quite a few encoding optimizations to be as performant as
    possible on various layout topologies and clocking schemes.

    The approach applies to any data structures that implement the
    necessary functions to comply with `is_network_type` and
    `is_gate_level_layout`, respectively. It is, thereby, mostly
    technology-independent but can make certain assumptions if needed, for
    instance for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.
    """

class num_clks(enum.Enum):
    """Number of clock phases of a clocking scheme."""

    THREE = 0
    """Three clocks."""

    FOUR = 1
    """Four clocks."""

class orthogonal_params:
    """Parameters for the orthogonal physical design algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def number_of_clock_phases(self) -> num_clks:
        """Number of clock phases to use. 3 and 4 are supported."""

    @number_of_clock_phases.setter
    def number_of_clock_phases(self, arg: num_clks, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the progress of the gate placement."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

class orthogonal_stats:
    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta: ...
    @property
    def x_size(self) -> int: ...
    @property
    def y_size(self) -> int: ...
    @property
    def num_gates(self) -> int: ...
    @property
    def num_wires(self) -> int: ...
    @property
    def num_crossings(self) -> int: ...

def orthogonal(
    network: mnt.pyfiction.networks.technology_network,
    parameters: orthogonal_params = ...,
    statistics: orthogonal_stats | None = None,
) -> mnt.pyfiction.layouts.cartesian_gate_layout:
    """
    A scalable placement & routing approach based on orthogonal graph
    drawing as originally proposed in \\"Scalable Design for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
    can be found in \\"Design Automation for Field-coupled
    Nanotechnologies\\" by M. Walter, R. Wille, F. Sill Torres, and R.
    Drechsler published by Springer Nature in 2022.

    Via certain restrictions to the degrees of freedom in FCN physical
    design, this algorithm achieves a polynomial time complexity. However,
    these restrictions lead to an overall approximation of optimal layout
    quality within several factors. Therefore, this algorithm produces
    valid layouts within a short amount of time, its results are far from
    being optimal in terms of area.

    The imposed restrictions are that the input logic network has to be a
    3-graph, i.e., cannot have any node exceeding degree 3 (combined input
    and output), and that the resulting layout is always 2DDWave-clocked.

    This algorithm is based on a modification of \\"Improved orthogonal
    drawings of 3-graphs\\" by Therese C. Biedl in Canadian Conference on
    Computational Geometry 1996. Biedl's original algorithm works for
    undirected graphs only while this modification respects information
    flow of directed logic networks. To this end, the edge directions of
    the logic network directly used instead of relabeling the edges
    according to its DFS tree, ordering the vertices using topological
    sorting instead of DFS, and adding an extra placement rule for nodes
    without predecessors.

    The algorithm works in polynomial time :math:`\\mathcal{O}(3|N| + |L|)`
    where :math:`|N|` is the number of nodes the given network and
    :math:`|L|` is the resulting layout size given by :math:`x \\cdot y`,
    which approaches :math:`(\\frac{|N|}{2})^2` asymptotically.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in larger than 2.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.
    """

def orthogonal_hexagonal(
    network: mnt.pyfiction.networks.technology_network,
    parameters: orthogonal_params = ...,
    statistics: orthogonal_stats | None = None,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout:
    """
    A scalable placement & routing approach based on orthogonal graph
    drawing as originally proposed in \\"Scalable Design for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
    can be found in \\"Design Automation for Field-coupled
    Nanotechnologies\\" by M. Walter, R. Wille, F. Sill Torres, and R.
    Drechsler published by Springer Nature in 2022.

    Via certain restrictions to the degrees of freedom in FCN physical
    design, this algorithm achieves a polynomial time complexity. However,
    these restrictions lead to an overall approximation of optimal layout
    quality within several factors. Therefore, this algorithm produces
    valid layouts within a short amount of time, its results are far from
    being optimal in terms of area.

    The imposed restrictions are that the input logic network has to be a
    3-graph, i.e., cannot have any node exceeding degree 3 (combined input
    and output), and that the resulting layout is always 2DDWave-clocked.

    This algorithm is based on a modification of \\"Improved orthogonal
    drawings of 3-graphs\\" by Therese C. Biedl in Canadian Conference on
    Computational Geometry 1996. Biedl's original algorithm works for
    undirected graphs only while this modification respects information
    flow of directed logic networks. To this end, the edge directions of
    the logic network directly used instead of relabeling the edges
    according to its DFS tree, ordering the vertices using topological
    sorting instead of DFS, and adding an extra placement rule for nodes
    without predecessors.

    The algorithm works in polynomial time :math:`\\mathcal{O}(3|N| + |L|)`
    where :math:`|N|` is the number of nodes the given network and
    :math:`|L|` is the resulting layout size given by :math:`x \\cdot y`,
    which approaches :math:`(\\frac{|N|}{2})^2` asymptotically.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in larger than 2.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.
    """

def orthogonal_odd_row_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: orthogonal_params = ...,
    statistics: orthogonal_stats | None = None,
) -> mnt.pyfiction.layouts.odd_row_hex_gate_layout:
    """
    A scalable placement & routing approach based on orthogonal graph
    drawing as originally proposed in \\"Scalable Design for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
    can be found in \\"Design Automation for Field-coupled
    Nanotechnologies\\" by M. Walter, R. Wille, F. Sill Torres, and R.
    Drechsler published by Springer Nature in 2022.

    Via certain restrictions to the degrees of freedom in FCN physical
    design, this algorithm achieves a polynomial time complexity. However,
    these restrictions lead to an overall approximation of optimal layout
    quality within several factors. Therefore, this algorithm produces
    valid layouts within a short amount of time, its results are far from
    being optimal in terms of area.

    The imposed restrictions are that the input logic network has to be a
    3-graph, i.e., cannot have any node exceeding degree 3 (combined input
    and output), and that the resulting layout is always 2DDWave-clocked.

    This algorithm is based on a modification of \\"Improved orthogonal
    drawings of 3-graphs\\" by Therese C. Biedl in Canadian Conference on
    Computational Geometry 1996. Biedl's original algorithm works for
    undirected graphs only while this modification respects information
    flow of directed logic networks. To this end, the edge directions of
    the logic network directly used instead of relabeling the edges
    according to its DFS tree, ordering the vertices using topological
    sorting instead of DFS, and adding an extra placement rule for nodes
    without predecessors.

    The algorithm works in polynomial time :math:`\\mathcal{O}(3|N| + |L|)`
    where :math:`|N|` is the number of nodes the given network and
    :math:`|L|` is the resulting layout size given by :math:`x \\cdot y`,
    which approaches :math:`(\\frac{|N|}{2})^2` asymptotically.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in larger than 2.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.
    """

def orthogonal_odd_column_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: orthogonal_params = ...,
    statistics: orthogonal_stats | None = None,
) -> mnt.pyfiction.layouts.odd_column_hex_gate_layout:
    """
    A scalable placement & routing approach based on orthogonal graph
    drawing as originally proposed in \\"Scalable Design for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
    can be found in \\"Design Automation for Field-coupled
    Nanotechnologies\\" by M. Walter, R. Wille, F. Sill Torres, and R.
    Drechsler published by Springer Nature in 2022.

    Via certain restrictions to the degrees of freedom in FCN physical
    design, this algorithm achieves a polynomial time complexity. However,
    these restrictions lead to an overall approximation of optimal layout
    quality within several factors. Therefore, this algorithm produces
    valid layouts within a short amount of time, its results are far from
    being optimal in terms of area.

    The imposed restrictions are that the input logic network has to be a
    3-graph, i.e., cannot have any node exceeding degree 3 (combined input
    and output), and that the resulting layout is always 2DDWave-clocked.

    This algorithm is based on a modification of \\"Improved orthogonal
    drawings of 3-graphs\\" by Therese C. Biedl in Canadian Conference on
    Computational Geometry 1996. Biedl's original algorithm works for
    undirected graphs only while this modification respects information
    flow of directed logic networks. To this end, the edge directions of
    the logic network directly used instead of relabeling the edges
    according to its DFS tree, ordering the vertices using topological
    sorting instead of DFS, and adding an extra placement rule for nodes
    without predecessors.

    The algorithm works in polynomial time :math:`\\mathcal{O}(3|N| + |L|)`
    where :math:`|N|` is the number of nodes the given network and
    :math:`|L|` is the resulting layout size given by :math:`x \\cdot y`,
    which approaches :math:`(\\frac{|N|}{2})^2` asymptotically.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in larger than 2.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.
    """

def orthogonal_even_column_hex(
    network: mnt.pyfiction.networks.technology_network,
    parameters: orthogonal_params = ...,
    statistics: orthogonal_stats | None = None,
) -> mnt.pyfiction.layouts.even_column_hex_gate_layout:
    """
    A scalable placement & routing approach based on orthogonal graph
    drawing as originally proposed in \\"Scalable Design for Field-coupled
    Nanocomputing Circuits\\" by M. Walter, R. Wille, F. Sill Torres, D.
    Große, and R. Drechsler in ASP-DAC 2019. A more extensive description
    can be found in \\"Design Automation for Field-coupled
    Nanotechnologies\\" by M. Walter, R. Wille, F. Sill Torres, and R.
    Drechsler published by Springer Nature in 2022.

    Via certain restrictions to the degrees of freedom in FCN physical
    design, this algorithm achieves a polynomial time complexity. However,
    these restrictions lead to an overall approximation of optimal layout
    quality within several factors. Therefore, this algorithm produces
    valid layouts within a short amount of time, its results are far from
    being optimal in terms of area.

    The imposed restrictions are that the input logic network has to be a
    3-graph, i.e., cannot have any node exceeding degree 3 (combined input
    and output), and that the resulting layout is always 2DDWave-clocked.

    This algorithm is based on a modification of \\"Improved orthogonal
    drawings of 3-graphs\\" by Therese C. Biedl in Canadian Conference on
    Computational Geometry 1996. Biedl's original algorithm works for
    undirected graphs only while this modification respects information
    flow of directed logic networks. To this end, the edge directions of
    the logic network directly used instead of relabeling the edges
    according to its DFS tree, ordering the vertices using topological
    sorting instead of DFS, and adding an extra placement rule for nodes
    without predecessors.

    The algorithm works in polynomial time :math:`\\mathcal{O}(3|N| + |L|)`
    where :math:`|N|` is the number of nodes the given network and
    :math:`|L|` is the resulting layout size given by :math:`x \\cdot y`,
    which approaches :math:`(\\frac{|N|}{2})^2` asymptotically.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in larger than 2.

    Args:
        ntk: The network that is to place and route.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Desired gate-level layout type.
        Ntk: Network type that acts as specification.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.
    """

class gold_effort_mode(enum.Enum):
    """
    The `effort_mode` enum defines different levels of computational
    effort for generating and exploring search space graphs for during the
    graph-oriented layout design process. Each mode varies in the number
    of search space graphs generated and the strategies employed,
    balancing between runtime efficiency and the likelihood of finding
    optimal solutions.
    """

    HIGH_EFFICIENCY = 0
    """
    HIGH_EFFICIENCY mode generates 2 search space graphs. This option
    minimizes runtime but may not always yield the optimal results.
    """

    HIGH_EFFORT = 1
    """
    HIGH_EFFORT mode generates 12 search space graphs using various fanout
    substitution strategies, PI placements, and other parameters. This
    wider exploration increases the chance of finding optimal layouts but
    also extends runtime. When a solution is found in any graph, its cost
    is used to prune the remaining graphs.
    """

    HIGHEST_EFFORT = 2
    """
    HIGHEST_EFFORT mode builds upon HIGH_EFFORT by duplicating the 12
    search space graphs for different cost objectives. If the cost
    objective involves layout area, number of crossings, number of wire
    segments, or a combination of area and crossings, a total of 48 search
    space graphs are generated. For a custom cost objective, an additional
    12 graphs are created, resulting in 60 graphs in total.
    """

    MAXIMUM_EFFORT = 3
    """
    MAXIMUM_EFFORT mode builds upon HIGHEST_EFFORT by duplicating the 48
    (60) search space graphs using randomized fanout substitution
    strategies and topological orderings. If the cost objective involves
    layout area, number of crossings, number of wire segments, or a
    combination of area and crossings, a total of 96 search space graphs
    are generated. For a custom cost objective, an additional 12 graphs
    are created, resulting in 120 graphs in total. This mode has a higher
    chance of finding optimal solutions but significantly increases
    runtime.
    """

class gold_cost_objective(enum.Enum):
    """
    The `cost_objective` enum defines various cost objectives that can be
    used in the graph-oriented layout design process. Each cost objective
    represents a different metric used to expand a vertex in the search
    space graph.
    """

    AREA = 0
    """
    AREA: Optimizes for the total area of the layout, aiming to minimize
          the space required for the design.
    """

    WIRES = 1
    """
    WIRES: Optimizes for the number of wire segments in the layout,
           reducing the delay and increasing throughput.
    """

    CROSSINGS = 2
    """CROSSINGS: Optimizes for the number of wire crossings in the layout."""

    ACP = 3
    """
    ACP (Area-Crossings Product): Optimizes for a combination of layout
    area and the number of crossings.
    """

    CUSTOM = 4
    """
    CUSTOM: Allows for a user-defined cost objective, enabling
            optimization based on specific criteria outside
    the predefined options.
    """

class graph_oriented_layout_design_params:
    """Parameters for the graph-oriented layout design algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def timeout(self) -> int:
        """Timeout limit (in ms)."""

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def num_vertex_expansions(self) -> int:
        """
        Number of expansions for each vertex that should be explored. For each
        partial layout, `num_vertex_expansions` positions will be checked for
        the next node/gate to be placed. A lower value requires less runtime,
        but the layout might have a larger area or it could also lead to no
        solution being found. A higher value might lead to better solutions,
        but also requires more runtime. Defaults to 4 expansions for each
        vertex.
        """

    @num_vertex_expansions.setter
    def num_vertex_expansions(self, arg: int, /) -> None: ...
    @property
    def verbose(self) -> bool:
        """Verbosity."""

    @verbose.setter
    def verbose(self, arg: bool, /) -> None: ...
    @property
    def mode(self) -> gold_effort_mode:
        """The effort mode used. Defaults to HIGH_EFFORT."""

    @mode.setter
    def mode(self, arg: gold_effort_mode, /) -> None: ...
    @property
    def cost(self) -> gold_cost_objective:
        """The cost objective used. Defaults to AREA"""

    @cost.setter
    def cost(self, arg: gold_cost_objective, /) -> None: ...
    @property
    def return_first(self) -> bool:
        """
        Return the first found layout, which might still have a high cost but
        can be found fast.
        """

    @return_first.setter
    def return_first(self, arg: bool, /) -> None: ...
    @property
    def planar(self) -> bool:
        """
        Disable the creation of crossings during layout generation. If set to
        true, gates will only be placed if a crossing-free wiring is found.
        Defaults to false.
        """

    @planar.setter
    def planar(self, arg: bool, /) -> None: ...
    @property
    def enable_multithreading(self) -> bool:
        """
        BETA feature: Flag to enable or disable multithreading during the
        execution of the layout design algorithm.

        When set to `true`, the algorithm will utilize multiple threads to
        process different search space graphs in parallel, improving
        performance by distributing the workload across available CPU cores.
        If set to `false`, the algorithm will run sequentially on a single
        thread.

        Only recommended for `HIGH_EFFORT` and `HIGHEST_EFFORT` modes and
        complex networks (> 100 nodes).

        Enabling multithreading can significantly speed up the algorithm,
        especially when using multiple search space graphs and dealing with
        complex networks, by concurrently expanding them. However, it may
        introduce additional overhead for thread synchronization and can
        increase memory usage. It is therefore not recommended for small input
        networks.

        Default value: `false`
        """

    @enable_multithreading.setter
    def enable_multithreading(self, arg: bool, /) -> None: ...
    @property
    def seed(self) -> int | None:
        """
        Random seed for PI spacing, fanout substitution, and topological
        ordering. Generated randomly if not specified.
        """

    @seed.setter
    def seed(self, arg: int | None, /) -> None: ...
    @property
    def straight_inverters(self) -> bool:
        """Enforce NOT gates to be routed non-bending only."""

    @straight_inverters.setter
    def straight_inverters(self, arg: bool, /) -> None: ...
    @property
    def tiles_to_skip_between_pis(self) -> int:
        """
        For each primary input (PI) considered during placement, reserve this
        many empty tiles *after* the current frontier:
         - Top edge (row 0): leave `tiles_to_skip_between_pis` empty tiles to
           the right of the rightmost occupied tile before proposing a new PI
           position.
         - Left edge (column 0): leave `tiles_to_skip_between_pis` empty tiles
           below the bottommost occupied tile before proposing a new PI
           position.

        This soft margin can reduce local congestion and increase the
        probability of finding a routable layout at the expense of a
        temporarily larger footprint, which post-layout optimization may later
        shrink. Defaults to `0`.
        """

    @tiles_to_skip_between_pis.setter
    def tiles_to_skip_between_pis(self, arg: int, /) -> None: ...
    @property
    def randomize_tiles_to_skip_between_pis(self) -> bool:
        """
        When enabled, randomizes the tiles_to_skip_between_pis value for each
        PI placement. The random value is chosen from
        `tiles_to_skip_between_pis - 1` to `tiles_to_skip_between_pis`
        (inclusive). A zero limit always selects zero. This can help explore
        different placement strategies and potentially find better layouts.
        Uses the same random seed as other randomization features for
        reproducibility. Defaults to `false`.
        """

    @randomize_tiles_to_skip_between_pis.setter
    def randomize_tiles_to_skip_between_pis(self, arg: bool, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the number of search space graph expansions
        performed so far.
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

class graph_oriented_layout_design_stats:
    """
    This struct stores statistics about the graph-oriented layout design
    process.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta:
        """Runtime of the graph-oriented layout design process."""

    @property
    def x_size(self) -> int:
        """Layout width."""

    @property
    def y_size(self) -> int:
        """Layout height."""

    @property
    def num_gates(self) -> int:
        """Number of gates."""

    @property
    def num_wires(self) -> int:
        """Number of wires."""

    @property
    def num_crossings(self) -> int:
        """Number of crossings."""

def graph_oriented_layout_design(
    network: mnt.pyfiction.networks.technology_network,
    parameters: graph_oriented_layout_design_params = ...,
    statistics: graph_oriented_layout_design_stats | None = None,
    custom_cost_objective: Callable[[mnt.pyfiction.layouts.cartesian_gate_layout], int] | None = None,
) -> mnt.pyfiction.layouts.cartesian_gate_layout | None:
    """
    A scalable and efficient placement & routing approach based on
    spanning a search space graph of partial layouts and finding a path to
    one of its leaves, i.e., a complete layout.

    The search space graph starts with an empty layout and then expands it
    based on where the first node in a topological sort of the logic
    network can be placed. Based on the position of this first node, a
    cost is assigned to each expansion based on the position of the placed
    node. The vertex with the lowest cost, which is the smallest layout
    w.r.t. the cost objective (e.g. area), is then chosen for the next
    expansion. This iterative process continues until a leaf node is
    found, which is a layout with all nodes placed. The algorithm then
    continues to backtrack through the search space graph to find other
    complete layouts with lower cost.

    Exclusively generates 2DDWave-clocked layouts.

    This algorithm was proposed in \\"A* is Born: Efficient and Scalable
    Physical Design for Field-coupled Nanocomputing\\" by S. Hofmann, M.
    Walter, and R. Wille in IEEE NANO 2024
    (https://ieeexplore.ieee.org/document/10628808) and extended in
    \\"Physical Design for Field-coupled Nanocomputing with Discretionary
    Cost Objectives\\" by S. Hofmann, M. Walter, and R. Wille in LASCAS
    2025 (https://ieeexplore.ieee.org/document/10966234).

    Args:
        ntk: The network to be placed and routed.
        ps: The parameters for the A* priority routing algorithm. Defaults
            to an empty parameter set.
        pst: A pointer to a statistics object to record execution details.
             Defaults to nullptr.
        custom_cost_objective: A custom cost objective that is evaluated
                               at every expansion of the search space
                               graph. Should be a function that can be
                               calculated based on the current partial
                               layout and returns an uint64_t that should
                               be minimized.

    Template Args:
        Lyt: Cartesian gate-level layout type.
        Ntk: Network type.

    Returns:
        The smallest layout yielded by the graph-oriented layout design
        algorithm under the given parameters.
    """

def apply_qca_one_library(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.qca.qca_layout:
    """
    Applies a gate library to a given gate-level layout and, thereby,
    creates and returns the layout type the library produces, e.g.,
    `qca::layout` for `qca::qca_one_library` or `sidb::layout` for
    `sidb::bestagon_library`. The gate library type should provide all
    functions specified in `fcn::gate_library`. It is, thus, easiest to
    extend `fcn::gate_library` to implement a new gate library.

    A cell grid spans the gate-level layout; if it has tile-based
    clocking, its clock zones are the library's tiles and follow the gate-
    level clocking, and, if it has synchronization elements, each clock
    zone receives the synchronization delay of its gate tile. The delay
    therefore also covers cells that are added to the zone later, e.g.,
    via cells. Input and output cells carry the names of their nodes. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed nonconstant
                     gate mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        GateLyt: Type of the gate-level layout to apply the library to.

    Returns:
        A layout that implements `lyt`'s gate types with building blocks
        defined in `GateLibrary`.
    """

def apply_sim7_mol_library(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.mol_qca.mol_qca_layout:
    """
    Applies a gate library to a given gate-level layout and, thereby,
    creates and returns the layout type the library produces, e.g.,
    `qca::layout` for `qca::qca_one_library` or `sidb::layout` for
    `sidb::bestagon_library`. The gate library type should provide all
    functions specified in `fcn::gate_library`. It is, thus, easiest to
    extend `fcn::gate_library` to implement a new gate library.

    A cell grid spans the gate-level layout; if it has tile-based
    clocking, its clock zones are the library's tiles and follow the gate-
    level clocking, and, if it has synchronization elements, each clock
    zone receives the synchronization delay of its gate tile. The delay
    therefore also covers cells that are added to the zone later, e.g.,
    via cells. Input and output cells carry the names of their nodes. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed nonconstant
                     gate mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        GateLyt: Type of the gate-level layout to apply the library to.

    Returns:
        A layout that implements `lyt`'s gate types with building blocks
        defined in `GateLibrary`.
    """

def apply_topolinano_library(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> mnt.pyfiction.inml.inml_layout:
    """
    Applies a gate library to a given gate-level layout and, thereby,
    creates and returns the layout type the library produces, e.g.,
    `qca::layout` for `qca::qca_one_library` or `sidb::layout` for
    `sidb::bestagon_library`. The gate library type should provide all
    functions specified in `fcn::gate_library`. It is, thus, easiest to
    extend `fcn::gate_library` to implement a new gate library.

    A cell grid spans the gate-level layout; if it has tile-based
    clocking, its clock zones are the library's tiles and follow the gate-
    level clocking, and, if it has synchronization elements, each clock
    zone receives the synchronization delay of its gate tile. The delay
    therefore also covers cells that are added to the zone later, e.g.,
    via cells. Input and output cells carry the names of their nodes. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed nonconstant
                     gate mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        GateLyt: Type of the gate-level layout to apply the library to.

    Returns:
        A layout that implements `lyt`'s gate types with building blocks
        defined in `GateLibrary`.
    """

def apply_bestagon_library(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.sidb.sidb_layout:
    """
    Applies a gate library to a given gate-level layout and, thereby,
    creates and returns the layout type the library produces, e.g.,
    `qca::layout` for `qca::qca_one_library` or `sidb::layout` for
    `sidb::bestagon_library`. The gate library type should provide all
    functions specified in `fcn::gate_library`. It is, thus, easiest to
    extend `fcn::gate_library` to implement a new gate library.

    A cell grid spans the gate-level layout; if it has tile-based
    clocking, its clock zones are the library's tiles and follow the gate-
    level clocking, and, if it has synchronization elements, each clock
    zone receives the synchronization delay of its gate tile. The delay
    therefore also covers cells that are added to the zone later, e.g.,
    via cells. Input and output cells carry the names of their nodes. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed nonconstant
                     gate mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        GateLyt: Type of the gate-level layout to apply the library to.

    Returns:
        A layout that implements `lyt`'s gate types with building blocks
        defined in `GateLibrary`.
    """

class graph_coloring_engine(enum.Enum):
    """
    An enumeration of coloring engines to use for the graph coloring. All
    but SAT are using the graph-coloring library by Brian Crites.
    """

    MCS = 0
    """
    Optimal coloring for chordal graphs proposed in \\"Register Allocation
    via Coloring of Chordal Graphs\\" by Jens Palsberg in CATS 2007.
    """

    DSATUR = 1
    """
    Saturation degree algorithm proposed in \\"New Methods to Color the
    Vertices of a Graph\\" by Daniel Brélaz in Communications of the ACM,
    1979. This algorithm is a heuristic but is exact for bipartite graphs.
    """

    LMXRLF = 2
    """
    A randomized heuristic algorithm that combines various paradigms like
    divide-and-conquer, objective functions, reuse of intermediate
    solutions etc. It was proposed in \\"Efficient Coloring of a Large
    Spectrum of Graphs\\" by Darko Kirovski and Miodrag Potkonjak in DAC
    1998. While this algorithm is really performant, it tends to find non-
    optimal solutions even for small instances.
    """

    TABUCOL = 3
    """
    A :math:`k`-coloring algorithm using tabu search proposed in \\"Using
    Tabu Search Techniques for Graph Coloring\\" by A. Hertz and D. de
    Werra in Computing 1987. The authors claim that it significantly
    outperforms simulated annealing. However, since it is a
    :math:`k`-coloring algorithm, it is required to set `k_color_value` in
    `determine_vertex_coloring_params` to the chromatic number that is to
    be checked for.
    """

    SAT = 4
    """Custom iterative SAT-based encoding that finds optimal colorings."""

class color_routing_params:
    """Parameters for the color routing algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def conduct_partial_routing(self) -> bool:
        """
        Do not abort if some objectives cannot be fulfilled, but partially
        route the layout as much as possible.
        """

    @conduct_partial_routing.setter
    def conduct_partial_routing(self, arg: bool, /) -> None: ...
    @property
    def crossings(self) -> bool:
        """Enable crossings."""

    @crossings.setter
    def crossings(self, arg: bool, /) -> None: ...
    @property
    def path_limit(self) -> int | None:
        """
        If a value is given, for each objective, only up to the `path_limit`
        shortest paths will be enumerated (using Yen's algorithm) instead of
        all paths.
        """

    @path_limit.setter
    def path_limit(self, arg: int | None, /) -> None: ...
    @property
    def engine(self) -> graph_coloring_engine:
        """The engine to use."""

    @engine.setter
    def engine(self, arg: graph_coloring_engine, /) -> None: ...
    @property
    def partial_sat(self) -> bool:
        """Allow partial solutions when the SAT engine is used."""

    @partial_sat.setter
    def partial_sat(self, arg: bool, /) -> None: ...

@overload
def color_routing(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        ]
    ],
    params: color_routing_params = ...,
) -> bool: ...
@overload
def color_routing(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        ]
    ],
    params: color_routing_params = ...,
) -> bool: ...
@overload
def color_routing(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        ]
    ],
    params: color_routing_params = ...,
) -> bool:
    """
    A multi-path signal routing approach based on coloring of edge
    intersection graphs as originally proposed in \\"Efficient Multi-Path
    Signal Routing for Field-coupled Nanotechnologies\\" by M. Walter and
    R. Wille in NANOARCH 2022.

    Given a gate-level layout and a set of routing objectives, this
    algorithm tries to fulfill all objectives by routing several conflict-
    free wire paths. To this end, a plethora of possible paths are
    enumerated in the given layout and an edge-intersection graph of paths
    on a grid (EPG) created from them. In an EPG, each vertex represents a
    path and each edge conflicts between them. When two vertices are
    connected by an edge, they cannot be routed together conflict-free in
    the layout. To determine a (maximum) set of routable paths, a vertex
    coloring is computed on the EPG. Finally, all vertices that are
    colored identically can be routed together. The biggest such set is
    applied to the layout.

    Multiple parameters can be set to specify the behavior of the
    algorithm. For instance, whether crossings should be enabled and
    whether a partial routing should be conducted if not all objectives
    could be fulfilled. Furthermore, the path enumeration and the coloring
    can be parameterized in the first place. By default, all paths are
    enumerated for each objective. While this guarantees completeness on
    small layouts, it quickly becomes intractable. Therefore, a path limit
    can be set that restricts the number of paths to the :math:`k`
    shortest. Additionally, for the coloring process, SAT solving is used
    by default, which, again, guarantees completeness, but becomes
    infeasible rather quickly. However, powerful symmetry breaking is
    applied that assists the solving process, e.g., it is known that all
    vertices that are belonging to the same objective are forming a
    clique, which can be pre-colored. Additionally, lexicographical
    orderings are enforced. For a more scalable (yet incomplete) approach,
    several coloring heuristics are available, from which can be chosen
    (see determine_vertex_coloring).

    This function will return `true` if all objectives could be satisfied
    or if the partial routing parameter was set. In the case of `true`
    being returned, all determined paths have been routed in the given
    layout.

    Args:
        lyt: A gate-level layout to route.
        objectives: The routing objectives as source-target pairs to
                    fulfill.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: The gate-level layout type to route.

    Returns:
        `true` iff routing was successful, i.e., iff all objectives could
        be satisfied.
    """

class hexagonalization_io_pin_routing_error(RuntimeError): ...

class hexagonalization_io_pin_extension_mode(enum.Enum):
    """
    Specifies how primary inputs/outputs should be handled in the
    hexagonalization process.
    """

    NONE = 0
    """Do not extend primary inputs/outputs to the top/bottom row (default)."""

    EXTEND = 1
    """Extend primary inputs/outputs to the top/bottom row."""

    EXTEND_PLANAR = 2
    """
    Extend primary inputs/outputs to the top/bottom row with planar
    rerouting (i.e., without crossings).
    """

class hexagonalization_params:
    """
    This structure encapsulates settings that determine how primary inputs
    (PIs) and primary outputs (POs) are handled during the conversion from
    a Cartesian to a hexagonal layout.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def input_pin_extension(self) -> hexagonalization_io_pin_extension_mode:
        """Input extension mode. Defaults to none"""

    @input_pin_extension.setter
    def input_pin_extension(self, arg: hexagonalization_io_pin_extension_mode, /) -> None: ...
    @property
    def output_pin_extension(self) -> hexagonalization_io_pin_extension_mode:
        """Output extension mode. Defaults to none"""

    @output_pin_extension.setter
    def output_pin_extension(self, arg: hexagonalization_io_pin_extension_mode, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the progress of the gate mapping and the pin
        extension.
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

class hexagonalization_stats:
    """This struct stores statistics about the hexagonalization process."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta:
        """Runtime of the hexagonalization process."""

    @property
    def x_size(self) -> int:
        """Layout width."""

    @property
    def y_size(self) -> int:
        """Layout height."""

    @property
    def num_gates(self) -> int:
        """Number of gates."""

    @property
    def num_wires(self) -> int:
        """Number of wires."""

    @property
    def num_crossings(self) -> int:
        """Number of crossings."""

def hexagonalization(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    parameters: hexagonalization_params = ...,
    statistics: hexagonalization_stats | None = None,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout:
    """
    Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even
    row clocked layout suitable for SiDBs by remapping all gates and wires
    as originally proposed in \\"Scalable Physical Design for Silicon
    Dangling Bond Logic: How a 45° Turn Prevents the Reinvention of the
    Wheel\\" by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2023
    (https://ieeexplore.ieee.org/document/10231278).

    Args:
        lyt: 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
        params: Parameters.
        stats: Statistics.

    Template Args:
        HexLyt: Even-row hexagonal gate-level layout return type.
        CartLyt: Input Cartesian gate-level layout type.

    Returns:
        Hexagonal representation of the Cartesian layout.
    """

class post_layout_optimization_params:
    """Parameters for the post-layout optimization algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def max_gate_relocations(self) -> int | None:
        """
        Maximum number of relocations to try for each gate. Defaults to the
        number of tiles in the given layout if not specified.
        """

    @max_gate_relocations.setter
    def max_gate_relocations(self, arg: int | None, /) -> None: ...
    @property
    def optimize_pos_only(self) -> bool:
        """Only optimize PO positions."""

    @optimize_pos_only.setter
    def optimize_pos_only(self, arg: bool, /) -> None: ...
    @property
    def planar_optimization(self) -> bool:
        """
        Disable the creation of crossings during optimization. If set to true,
        gates will only be relocated if a crossing-free wiring is found.
        Defaults to false.
        """

    @planar_optimization.setter
    def planar_optimization(self, arg: bool, /) -> None: ...
    @property
    def timeout(self) -> int:
        """
        Timeout limit (in ms). Specifies the maximum allowed time in
        milliseconds for the optimization process. For large layouts, the
        actual execution time may slightly exceed this limit because it's
        impractical to check the timeout at every algorithm step and the
        functional correctness has to be ensured by completing essential
        algorithm steps.
        """

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the progress of the gate relocations and of the
        nested wiring reduction.
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

class post_layout_optimization_stats:
    """
    This struct stores statistics about the post-layout optimization
    process.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta:
        """Runtime of the post-layout optimization process."""

    @property
    def x_size_before(self) -> int:
        """Layout width before the post-layout optimization process."""

    @property
    def y_size_before(self) -> int:
        """Layout height before the post-layout optimization process."""

    @property
    def x_size_after(self) -> int:
        """Layout width after the post-layout optimization process."""

    @property
    def y_size_after(self) -> int:
        """Layout height after the post-layout optimization process."""

    @property
    def area_improvement(self) -> float:
        """Area reduction (in %) after the post-layout optimization process."""

    @property
    def num_wires_before(self) -> int:
        """Number of wire segments before the post-layout optimization process."""

    @property
    def num_wires_after(self) -> int:
        """Number of wire segments after the post-layout optimization process."""

    @property
    def num_crossings_before(self) -> int:
        """Number of crossings before the post-layout optimization process."""

    @property
    def num_crossings_after(self) -> int:
        """Number of crossings after the post-layout optimization process."""

def post_layout_optimization(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    parameters: post_layout_optimization_params = ...,
    statistics: post_layout_optimization_stats | None = None,
) -> None:
    """
    A post-layout optimization algorithm as originally proposed in \\"Post-
    Layout Optimization for Field-coupled Nanotechnologies\\" by S.
    Hofmann, M. Walter, and R. Wille in NANOARCH 2023
    (https://dl.acm.org/doi/10.1145/3611315.3633247) and extended in
    \\"Efficient and Scalable Post-Layout Optimization for Field-coupled
    Nanotechnologies\\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
    (https://ieeexplore.ieee.org/document/10916761). It can be used to
    reduce the area of a given sub-optimal Cartesian gate-level layout
    created by heuristics or machine learning. This optimization utilizes
    the distinct characteristics of the 2DDWave clocking scheme, which
    only allows information flow from top to bottom and left to right,
    therefore only aforementioned clocking scheme is supported.

    To reduce the layout area, first, gates are moved up and to the left
    as far as possible, including rerouting. This creates more compact
    layouts by freeing up space to the right and bottom, as all gates were
    moved to the top left corner.

    After moving all gates, this algorithm also checks if excess wiring
    exists on the layout using the `wiring_reduction` algorithm (cf.
    `wiring_reduction.hpp`)

    As outputs have to lay on the border of a layout for better
    accessibility, they are also moved to new borders determined based on
    the location of all other gates.

    Args:
        lyt: 2DDWave-clocked Cartesian gate-level layout to optimize.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Cartesian gate-level layout type.

    Note:
        This function requires the gate-level layout to be 2DDWave-
        clocked!
    """

class wiring_reduction_params:
    """Parameters for the wiring reduction algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def timeout(self) -> int:
        """
        Timeout limit (in ms). Specifies the maximum allowed time in
        milliseconds for the optimization process. For large layouts, the
        actual execution time may slightly exceed this limit because it's
        impractical to check the timeout at every algorithm step and the
        functional correctness has to be ensured by completing essential
        algorithm steps.
        """

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the number of wire paths processed so far."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

class wiring_reduction_stats:
    """This struct stores statistics about the wiring reduction process."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> datetime.timedelta:
        """Runtime of the wiring reduction process."""

    @property
    def x_size_before(self) -> int:
        """Layout width before the wiring reduction process."""

    @property
    def y_size_before(self) -> int:
        """Layout height before the wiring reduction process."""

    @property
    def x_size_after(self) -> int:
        """Layout width after the wiring reduction process."""

    @property
    def y_size_after(self) -> int:
        """Layout height before the wiring reduction process."""

    @property
    def num_wires_before(self) -> int:
        """Number of wire segments before the wiring reduction process."""

    @property
    def num_wires_after(self) -> int:
        """Number of wire segments after the wiring reduction process."""

    @property
    def wiring_improvement(self) -> float:
        """Improvement in the number wire segments."""

    @property
    def area_improvement(self) -> float:
        """Improvement in layout area."""

def wiring_reduction(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    parameters: wiring_reduction_params = ...,
    statistics: wiring_reduction_stats | None = None,
) -> None:
    """
    A scalable wiring reduction algorithm for 2DDWave-clocked layouts
    based on A* path finding as originally proposed in \\"Late Breaking
    Results: Wiring Reduction for Field-coupled Nanotechnologies\\" by S.
    Hofmann, M. Walter, and R. Wille in DAC 2024
    (https://dl.acm.org/doi/10.1145/3649329.3663491) and extended in
    \\"Efficient and Scalable Post-Layout Optimization for Field-coupled
    Nanotechnologies\\" by S. Hofmann, M. Walter, and R. Wille in TCAD 2025
    (https://ieeexplore.ieee.org/document/10916761).

    The core concept revolves around the selective removal of excess
    wiring by cutting them from a layout, contingent upon the ability to
    restore functional correctness by realigning the remaining layout
    fragments. Given the complexity of identifying these cuts,
    obstructions are strategically inserted into the layout to safeguard
    against the inadvertent deletion of standard gates or wire segments
    essential for the layout's integrity. Leveraging the obstructed layout
    as a basis, A* Search is employed to systematically identify feasible
    cuts either from left to right or top to bottom. Subsequently, these
    identified cuts are removed from the layout to minimize not only the
    number of wire segments, but also the area and critical path length.

    Args:
        lyt: The 2DDWave-clocked layout whose wiring is to be reduced.
        ps: Parameters.
        pst: Statistics.

    Template Args:
        Lyt: Cartesian gate-level layout type.
    """

@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    src: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
) -> bool: ...
@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    src: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
) -> bool: ...
@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    src: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
) -> bool:
    """
    Checks whether a given coordinate `successor` hosts a crossable wire
    when coming from coordinate `src` in a given layout. A wire is said to
    be crossable if a potential cross-over would not result in running
    along the same information flow direction. For example, a wire segment
    hosted by `successor` that is horizontal and runs from west to east is
    crossable by a wire segment coming from `src` that is vertical and
    runs from north to south. However, if the wire segment coming from
    `src` were also horizontal and ran from west to east, the cross-over
    would be prohibited.

    Args:
        lyt: The layout.
        src: Source coordinate in `lyt`.
        successor: Successor coordinate in lyt reachable from `src`.

    Template Args:
        Lyt: Layout type.

    Returns:
        `true` iff `successor` hosts a wire that is crossable from `src`.

    Note:
        This function can be called on layout types other than gate-level
        layouts, but will then always return `false`. This is helpful for
        general routing in, e.g., clocked layouts.
    """

@overload
def route_path(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    path: Sequence[mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]],
) -> None: ...
@overload
def route_path(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    path: Sequence[mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]],
) -> None: ...
@overload
def route_path(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    path: Sequence[mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]],
) -> None:
    """
    Establishes a wire routing along the given path in the given layout.
    To this end, the given path's source and target coordinates are
    assumed to be populated by other gates or wires that the new path
    shall connect to.

    If `path` contains a tile that is allocated already, it will instead
    switch to the crossing layer. If path contains exactly source and
    target, no wires are created, but the source and target are connected.

    Args:
        lyt: Gate-level layout in which a wire path is to be established.
        path: Path to route wires along.

    Template Args:
        Lyt: Gate-level layout type.
        Path: Path type.
    """

@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coords.offset_coordinate, mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coords.offset_coordinate, mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coords.offset_coordinate, mnt.pyfiction.layouts.coords.offset_coordinate]]:
    """
    Extracts all routing objectives from the given layout. To this end,
    all routing paths in the layout are traversed, starting at each PI.
    Whenever the next regular node (non-IO, non-constant, non-wire) is
    encountered, this connection is added to the list of all objectives.

    For example, let a layout have connections from `(0,0)` to `(2,3)` via
    a cascade of wires and a direct connection from `(2,2)` to `(2,3)`.
    The list of routing objectives extracted from that layout would
    contain `{(0,0), (2,3)}` and `{(2,2), (2,3)}`.

    In other words, if all wires were removed from the layout and all
    connections ripped-up, an equivalent layout could be recreated from
    the list of routing objectives.

    Args:
        lyt: Layout whose routing objectives are to be extracted.

    Template Args:
        Lyt: Gate-level layout type.

    Returns:
        List of all routing objectives in the given layout.
    """

@overload
def clear_routing(lyt: mnt.pyfiction.layouts.cartesian_gate_layout) -> None: ...
@overload
def clear_routing(lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout) -> None: ...
@overload
def clear_routing(lyt: mnt.pyfiction.layouts.hexagonal_gate_layout) -> None:
    """
    Removes the entire wire routing from the passed layout. This involves
    deleting all wire segments that have been placed on any tile as well
    as removing stored connections (children pointers) from all gates.

    Args:
        lyt: The layout whose routing is to be deleted.

    Template Args:
        Lyt: Gate-level Layout type.
    """

@overload
def reserve_input_nodes(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout, ntk: mnt.pyfiction.networks.technology_network
) -> dict[int, int]: ...
@overload
def reserve_input_nodes(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout, ntk: mnt.pyfiction.networks.technology_network
) -> dict[int, int]: ...
@overload
def reserve_input_nodes(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout, ntk: mnt.pyfiction.networks.technology_network
) -> dict[int, int]:
    """
    Reserves input nodes and returns their source-node to layout-node mapping.
    """

@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: bool | None,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: bool | None,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: bool | None,
) -> int: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: int,
    b: int,
    c: int,
) -> int: ...
