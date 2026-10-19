# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Placement and routing of logic networks into gate-level layouts."""

import datetime as dt
import enum
from collections.abc import Callable, Sequence
from typing import overload

import mnt.pyfiction.inml
import mnt.pyfiction.layouts
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

class exact_stats:
    """Statistics."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
        """Total algorithm runtime."""

    @property
    def x_size(self) -> int:
        """Layout extent along the x axis."""

    @property
    def y_size(self) -> int:
        """Layout extent along the y axis."""

    @property
    def num_gates(self) -> int:
        """Number of logic gates, excluding primary inputs and outputs."""

    @property
    def num_wires(self) -> int:
        """Number of physical wires, excluding primary inputs and outputs."""

    @property
    def num_crossings(self) -> int:
        """Number of crossing-layer wires above occupied ground-layer tiles."""

    @property
    def num_aspect_ratios(self) -> int:
        """Number of examined aspect ratios."""

@overload
def exact(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.cartesian_gate_layout] = ...,
    statistics: exact_stats | None = None,
    scheme: str = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    upper_bound_area: int = ...,
    upper_bound_x: int = ...,
    upper_bound_y: int = ...,
    fixed_size: bool = ...,
    num_threads: int = ...,
    synchronization_elements: bool = ...,
    crossings: bool = ...,
    border_io: bool = ...,
    straight_inverters: bool = ...,
    desynchronize: bool = ...,
    minimize_wires: bool = ...,
    minimize_crossings: bool = ...,
    timeout: int = ...,
    technology_specifics: technology_constraints = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = ...,
) -> mnt.pyfiction.layouts.cartesian_gate_layout | None:
    """
    Place and route a technology network with SMT solving.
    The layout_type keyword selects a registered gate-layout class.
    Returns a layout or None when the search finds no solution within its bounds.
    Raises ValueError when a shifted Cartesian or hexagonal layout requires layout_arrangement and none is supplied.

    Args:
        layout_type: Registered gate-layout class for the returned layout.
        statistics: Statistics object filled by the algorithm; None discards statistics.
        scheme: Clocking scheme to be used.
        layout_arrangement: Row or column arrangement; required for shifted Cartesian and hexagonal layouts, ignored for Cartesian layouts.
        upper_bound_area: Maximum total tile count. Area and axis bounds apply together.
        upper_bound_x: Maximum tile count in the x direction, combined with the area and y bounds.
        upper_bound_y: Maximum tile count in the y direction, combined with the area and x bounds.
        fixed_size: Search only aspect ratios with area min(upper_bound_area, upper_bound_x * upper_bound_y); axis bounds still apply.
        num_threads: Number of workers exploring aspect ratios.
        synchronization_elements: Allow synchronization elements to delay clock phases.
        crossings: Allow wires to cross on separate layers.
        border_io: Place primary inputs and outputs at the layout border.
        straight_inverters: Require straight wires through inverters.
        desynchronize: Allow unequal fan-in path lengths.
        minimize_wires: Minimize the number of wire tiles.
        minimize_crossings: Minimize the number of crossing tiles.
        timeout: Execution budget in milliseconds.
        technology_specifics: Constraints for the selected target technology.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
    """

@overload
def exact(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.shifted_cartesian_gate_layout],
    statistics: exact_stats | None = None,
    scheme: str = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    upper_bound_area: int = ...,
    upper_bound_x: int = ...,
    upper_bound_y: int = ...,
    fixed_size: bool = ...,
    num_threads: int = ...,
    synchronization_elements: bool = ...,
    crossings: bool = ...,
    border_io: bool = ...,
    straight_inverters: bool = ...,
    desynchronize: bool = ...,
    minimize_wires: bool = ...,
    minimize_crossings: bool = ...,
    timeout: int = ...,
    technology_specifics: technology_constraints = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = ...,
) -> mnt.pyfiction.layouts.shifted_cartesian_gate_layout | None: ...
@overload
def exact(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.hexagonal_gate_layout],
    statistics: exact_stats | None = None,
    scheme: str = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    upper_bound_area: int = ...,
    upper_bound_x: int = ...,
    upper_bound_y: int = ...,
    fixed_size: bool = ...,
    num_threads: int = ...,
    synchronization_elements: bool = ...,
    crossings: bool = ...,
    border_io: bool = ...,
    straight_inverters: bool = ...,
    desynchronize: bool = ...,
    minimize_wires: bool = ...,
    minimize_crossings: bool = ...,
    timeout: int = ...,
    technology_specifics: technology_constraints = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = ...,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout | None: ...
@overload
def exact(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[
        mnt.pyfiction.layouts.cartesian_gate_layout
        | mnt.pyfiction.layouts.shifted_cartesian_gate_layout
        | mnt.pyfiction.layouts.hexagonal_gate_layout
    ],
    statistics: exact_stats | None = None,
    scheme: str = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    upper_bound_area: int = ...,
    upper_bound_x: int = ...,
    upper_bound_y: int = ...,
    fixed_size: bool = ...,
    num_threads: int = ...,
    synchronization_elements: bool = ...,
    crossings: bool = ...,
    border_io: bool = ...,
    straight_inverters: bool = ...,
    desynchronize: bool = ...,
    minimize_wires: bool = ...,
    minimize_crossings: bool = ...,
    timeout: int = ...,
    technology_specifics: technology_constraints = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = ...,
) -> (
    mnt.pyfiction.layouts.cartesian_gate_layout
    | mnt.pyfiction.layouts.shifted_cartesian_gate_layout
    | mnt.pyfiction.layouts.hexagonal_gate_layout
    | None
): ...

class num_clks(enum.Enum):
    """Number of clock phases of a clocking scheme."""

    THREE = 0
    """Three clocks."""

    FOUR = 1
    """Four clocks."""

class orthogonal_stats:
    """Statistics for orthogonal physical design."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
        """Total algorithm runtime."""

    @property
    def x_size(self) -> int:
        """Layout extent along the x axis."""

    @property
    def y_size(self) -> int:
        """Layout extent along the y axis."""

    @property
    def num_gates(self) -> int:
        """Number of logic gates, excluding primary inputs and outputs."""

    @property
    def num_wires(self) -> int:
        """Number of physical wires, excluding primary inputs and outputs."""

    @property
    def num_crossings(self) -> int:
        """Number of crossing-layer wires above occupied ground-layer tiles."""

@overload
def orthogonal(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.cartesian_gate_layout] = ...,
    statistics: orthogonal_stats | None = None,
    number_of_clock_phases: num_clks = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
) -> mnt.pyfiction.layouts.cartesian_gate_layout:
    """
    Place and route a technology network with orthogonal graph drawing.
    The layout_type keyword selects Cartesian or hexagonal gate layouts.
    The keyword options select clock phases, arrangement, and progress reporting.
    Raises ValueError when a hexagonal layout requires layout_arrangement and none is supplied.
    Raises high_degree_fanin_exception for network nodes with more than two inputs.

    Args:
        layout_type: Registered gate-layout class for the returned layout.
        statistics: Statistics object filled by the algorithm; None discards statistics.
        number_of_clock_phases: Number of clock phases to use. 3 and 4 are supported.
        layout_arrangement: Row or column arrangement; required for hexagonal layouts, ignored for Cartesian layouts.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
    """

@overload
def orthogonal(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.hexagonal_gate_layout],
    statistics: orthogonal_stats | None = None,
    number_of_clock_phases: num_clks = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout: ...
@overload
def orthogonal(
    network: mnt.pyfiction.networks.technology_network,
    *,
    layout_type: type[mnt.pyfiction.layouts.cartesian_gate_layout | mnt.pyfiction.layouts.hexagonal_gate_layout],
    statistics: orthogonal_stats | None = None,
    number_of_clock_phases: num_clks = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = ...,
    on_progress: Callable[[str, int, int], None] | None = ...,
) -> mnt.pyfiction.layouts.cartesian_gate_layout | mnt.pyfiction.layouts.hexagonal_gate_layout: ...

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

class graph_oriented_layout_design_stats:
    """
    This struct stores statistics about the graph-oriented layout design
    process.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
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
        """Number of physical wires, excluding primary inputs and outputs."""

    @property
    def num_crossings(self) -> int:
        """Number of crossings."""

def graph_oriented_layout_design(
    network: mnt.pyfiction.networks.technology_network,
    *,
    timeout: int = ...,
    num_vertex_expansions: int = 4,
    verbose: bool = False,
    mode: gold_effort_mode = ...,
    cost: gold_cost_objective = ...,
    return_first: bool = False,
    planar: bool = False,
    enable_multithreading: bool = False,
    seed: int | None = None,
    straight_inverters: bool = False,
    tiles_to_skip_between_pis: int = 0,
    randomize_tiles_to_skip_between_pis: bool = False,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
    statistics: graph_oriented_layout_design_stats | None = None,
    custom_cost_objective: Callable[[mnt.pyfiction.layouts.cartesian_gate_layout], int] | None = None,
) -> mnt.pyfiction.layouts.cartesian_gate_layout | None:
    """
    Place and route a technology network by searching partial gate layouts.
    The keyword options control search effort, clocking, and progress reporting.
    tiles_to_skip_between_pis must lie in [0, 2**20]; values outside this range raise ValueError.

    Args:
        timeout: Execution budget in milliseconds.
        num_vertex_expansions: Candidate positions explored per gate; larger values increase search effort.
        verbose: Print search details.
        mode: Search effort mode.
        cost: Layout cost objective.
        return_first: Return the first valid layout instead of continuing cost optimization.
        planar: Require crossing-free wiring during layout generation.
        enable_multithreading: Explore search-space graphs in parallel.
        seed: Random seed; None selects a fresh random seed.
        straight_inverters: Require straight wires through inverters.
        tiles_to_skip_between_pis: Empty tiles reserved between primary inputs; must lie in [0, 2**20].
        randomize_tiles_to_skip_between_pis: Choose each PI spacing from max(0, tiles_to_skip_between_pis - 1) through tiles_to_skip_between_pis, using seed.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        on_worker_progress: Callback receiving (worker, count, description, completed, total, active); None disables worker reports.
        statistics: Statistics object filled by the algorithm; None discards statistics.
        custom_cost_objective: Layout cost callback used when cost is CUSTOM.
    """

def apply_qca_one_library(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.qca.qca_layout:
    """
    Apply the selected gate library to produce a cell-level layout.
    The cell grid spans the gate-level layout. QCA and iNML tile-based clock zones
    follow the gate-level clocking. Supported synchronization elements apply each
    gate tile's delay to its entire clock zone, including cells added later. molQCA
    cell types carry the clock phases defined by the gate library. Input and output
    cells retain their object names. The Bestagon library produces an SiDB layout
    on the H-Si(100)-2x1 lattice.

    Args:
        layout: Gate-level layout to map.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.

    Returns:
        The QCA, molQCA, iNML, or SiDB layout produced by the selected library.

    Raises:
        ValueError: The library cannot implement a gate type or orientation; the message names its tile.
    """

def apply_sim7_mol_library(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.mol_qca.mol_qca_layout:
    """
    Apply the selected gate library to produce a cell-level layout.
    The cell grid spans the gate-level layout. QCA and iNML tile-based clock zones
    follow the gate-level clocking. Supported synchronization elements apply each
    gate tile's delay to its entire clock zone, including cells added later. molQCA
    cell types carry the clock phases defined by the gate library. Input and output
    cells retain their object names. The Bestagon library produces an SiDB layout
    on the H-Si(100)-2x1 lattice.

    Args:
        layout: Gate-level layout to map.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.

    Returns:
        The QCA, molQCA, iNML, or SiDB layout produced by the selected library.

    Raises:
        ValueError: The library cannot implement a gate type or orientation; the message names its tile.
    """

def apply_topolinano_library(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> mnt.pyfiction.inml.inml_layout:
    """
    Apply the selected gate library to produce a cell-level layout.
    The cell grid spans the gate-level layout. QCA and iNML tile-based clock zones
    follow the gate-level clocking. Supported synchronization elements apply each
    gate tile's delay to its entire clock zone, including cells added later. molQCA
    cell types carry the clock phases defined by the gate library. Input and output
    cells retain their object names. The Bestagon library produces an SiDB layout
    on the H-Si(100)-2x1 lattice.

    Args:
        layout: Gate-level layout to map.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.

    Returns:
        The QCA, molQCA, iNML, or SiDB layout produced by the selected library.

    Raises:
        ValueError: The library cannot implement a gate type or orientation; the message names its tile.
    """

def apply_bestagon_library(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout, on_progress: Callable[[str, int, int], None] | None = None
) -> mnt.pyfiction.sidb.sidb_layout:
    """
    Apply the selected gate library to produce a cell-level layout.
    The cell grid spans the gate-level layout. QCA and iNML tile-based clock zones
    follow the gate-level clocking. Supported synchronization elements apply each
    gate tile's delay to its entire clock zone, including cells added later. molQCA
    cell types carry the clock phases defined by the gate library. Input and output
    cells retain their object names. The Bestagon library produces an SiDB layout
    on the H-Si(100)-2x1 lattice.

    Args:
        layout: Gate-level layout to map.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.

    Returns:
        The QCA, molQCA, iNML, or SiDB layout produced by the selected library.

    Raises:
        ValueError: The library cannot implement a gate type or orientation; the message names its tile.
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

@overload
def color_routing(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            int,
        ]
    ],
    *,
    conduct_partial_routing: bool = False,
    crossings: bool = False,
    path_limit: int | None = None,
    engine: graph_coloring_engine = ...,
    partial_sat: bool = False,
) -> bool: ...
@overload
def color_routing(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            int,
        ]
    ],
    *,
    conduct_partial_routing: bool = False,
    crossings: bool = False,
    path_limit: int | None = None,
    engine: graph_coloring_engine = ...,
    partial_sat: bool = False,
) -> bool: ...
@overload
def color_routing(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    objectives: Sequence[
        tuple[
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
            int,
        ]
    ],
    *,
    conduct_partial_routing: bool = False,
    crossings: bool = False,
    path_limit: int | None = None,
    engine: graph_coloring_engine = ...,
    partial_sat: bool = False,
) -> bool:
    """
    Route the supplied connections in place by coloring their path intersections.
    Returns True when all objectives are satisfied or conduct_partial_routing is enabled.

    Args:
        conduct_partial_routing: Do not abort if some objectives cannot be fulfilled, but partially route the layout as much as possible.
        crossings: Allow wires to cross on separate layers.
        path_limit: Maximum paths considered per objective; None enumerates all paths.
        engine: The engine to use.
        partial_sat: Allow partial solutions when the SAT coloring engine is selected.
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

class hexagonalization_stats:
    """This struct stores statistics about the hexagonalization process."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
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
        """Number of physical wires, excluding primary inputs and outputs."""

    @property
    def num_crossings(self) -> int:
        """Number of crossings."""

def hexagonalization(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    *,
    input_pin_extension: hexagonalization_io_pin_extension_mode = ...,
    output_pin_extension: hexagonalization_io_pin_extension_mode = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    statistics: hexagonalization_stats | None = None,
) -> mnt.pyfiction.layouts.hexagonal_gate_layout:
    """
    Convert a 2DDWave-clocked Cartesian gate layout to an even-row hexagonal layout.
    The keyword options select input and output pin extension and progress reporting.
    Raises ValueError for unsupported clocking or invalid occupied geometry.
    Raises OverflowError when transformed coordinates exceed the signed coordinate range.

    Args:
        input_pin_extension: Input pin extension mode: NONE, EXTEND, or EXTEND_PLANAR.
        output_pin_extension: Output pin extension mode: NONE, EXTEND, or EXTEND_PLANAR.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        statistics: Statistics object filled by the algorithm; None discards statistics.
    """

class post_layout_optimization_stats:
    """
    This struct stores statistics about the post-layout optimization
    process.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
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
    *,
    max_gate_relocations: int | None = None,
    optimize_pos_only: bool = False,
    planar_optimization: bool = False,
    timeout: int = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    statistics: post_layout_optimization_stats | None = None,
) -> None:
    """
    Optimize a 2DDWave-clocked Cartesian gate layout in place.
    The keyword options bound relocations and runtime; statistics receives the completed result.
    Raises ValueError for unsupported clocking or invalid occupied geometry.
    Raises OverflowError when the extent exceeds signed routing coordinates.

    Args:
        max_gate_relocations: Maximum number of relocations to try for each gate. Defaults to the number of tiles in the given layout if not specified.
        optimize_pos_only: Only optimize PO positions.
        planar_optimization: Require crossing-free wiring during optimization.
        timeout: Execution budget in milliseconds.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        statistics: Statistics object filled by the algorithm; None discards statistics.
    """

class wiring_reduction_stats:
    """This struct stores statistics about the wiring reduction process."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def time_total(self) -> dt.timedelta:
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
    *,
    timeout: int = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    statistics: wiring_reduction_stats | None = None,
) -> None:
    """
    Reduce wires in a 2DDWave-clocked gate layout in place with A* routing.
    The keyword options bound runtime and report progress.
    Raises ValueError for unsupported clocking or invalid occupied geometry.
    Raises OverflowError when the extent exceeds signed routing coordinates.

    Args:
        timeout: Execution budget in milliseconds.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        statistics: Statistics object filled by the algorithm; None discards statistics.
    """

@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    src: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> bool: ...
@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    src: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> bool: ...
@overload
def is_crossable_wire(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    src: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    successor: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
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
    path: Sequence[mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int]],
    destination: mnt.pyfiction.layouts.layout_input_port,
) -> None: ...
@overload
def route_path(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    path: Sequence[mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int]],
    destination: mnt.pyfiction.layouts.layout_input_port,
) -> None: ...
@overload
def route_path(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    path: Sequence[mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int]],
    destination: mnt.pyfiction.layouts.layout_input_port,
) -> None:
    """
    Routes a path to one explicit logical input without changing other
    input slots.

    Occupied intermediate ground coordinates use their free crossing
    layer. Endpoints and all intermediate placements are checked before
    mutation. A failed allocation removes newly created wires.

    Args:
        layout: Layout to edit.
        path: Path containing at least its occupied source and target
              coordinates.
        destination: Ordered destination input.

    Template Args:
        Geometry: Coordinate geometry of the gate-level layout.
        Path: Coordinate path type.

    Raises:
        ValueError: If an endpoint or intermediate placement is
                    unavailable.
        IndexError: If the destination input index is invalid.
    """

@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coordinate, mnt.pyfiction.layouts.coordinate, int]]: ...
@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coordinate, mnt.pyfiction.layouts.coordinate, int]]: ...
@overload
def extract_routing_objectives(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
) -> list[tuple[mnt.pyfiction.layouts.coordinate, mnt.pyfiction.layouts.coordinate, int]]:
    """
    Return connections between retained gates, fanouts, and terminals.
    Intermediate single-sink wires are followed through declared topology. Missing
    inputs produce no objective; disconnected input slots keep their indices.

    Args:
        layout: Gate-level layout to inspect.

    Returns:
        A list of (source, target, input_index) tuples with endpoint coordinates and the destination input index.

    Raises:
        ValueError: An intermediate wire chain contains a cycle.
    """

@overload
def clear_routing(lyt: mnt.pyfiction.layouts.cartesian_gate_layout) -> None: ...
@overload
def clear_routing(lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout) -> None: ...
@overload
def clear_routing(lyt: mnt.pyfiction.layouts.hexagonal_gate_layout) -> None:
    """
    Removes routing wires and disconnects retained objects while
    preserving identities and input indices.

    Args:
        lyt: Layout to edit.

    Template Args:
        Geometry: Coordinate geometry of the gate-level layout.
    """

@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: bool | None,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: bool | None,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
) -> mnt.pyfiction.layouts.layout_object_id:
    """
    Places a primary input at the given coordinate and returns its output port.
    """

@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: bool | None,
) -> mnt.pyfiction.layouts.layout_object_id: ...
@overload
def place(
    lyt: mnt.pyfiction.layouts.hexagonal_gate_layout,
    t: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    ntk: mnt.pyfiction.networks.technology_network,
    n: int,
    a: mnt.pyfiction.layouts.layout_object_id,
    b: mnt.pyfiction.layouts.layout_object_id,
    c: mnt.pyfiction.layouts.layout_object_id,
) -> mnt.pyfiction.layouts.layout_object_id: ...
