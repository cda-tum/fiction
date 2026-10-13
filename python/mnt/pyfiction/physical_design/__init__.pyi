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
    def time_total(self) -> datetime.timedelta: ...
    @property
    def x_size(self) -> int: ...
    @property
    def y_size(self) -> int: ...
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
    def num_aspect_ratios(self) -> int: ...

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
    necessary functions to comply with `is_network_type` and concrete
    gate-level layouts, respectively. It is, thereby, mostly technology-
    independent but can make certain assumptions if needed, for instance
    for ToPoliNano-compliant circuits.

    This approach requires the Z3 SMT solver to be installed on the
    system. Due to this circumstance, it is excluded from (CLI)
    compilation by default. To enable it, pass `-DFICTION_Z3=ON` to the
    cmake call.

    May throw a high_degree_fanin_exception if `ntk` contains any node
    with a fan-in too large to be handled by the specified clocking
    scheme.

    Args:
        ntk: The network that is to place and route.
        pst: Statistics.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit if one is found under the given parameters;
        `std::nullopt`, otherwise.

    Raises:
        std::invalid_argument: If `Lyt` is a shifted Cartesian or
                               hexagonal layout and
                               `layout_arrangement` is empty.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    layout_type: object,
    statistics: exact_stats | None = None,
    scheme: str = "2DDWave",
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = None,
    upper_bound_area: int = 65535,
    upper_bound_x: int = 65535,
    upper_bound_y: int = 65535,
    fixed_size: bool = False,
    num_threads: int = 1,
    synchronization_elements: bool = False,
    crossings: bool = False,
    border_io: bool = False,
    straight_inverters: bool = False,
    desynchronize: bool = False,
    minimize_wires: bool = False,
    minimize_crossings: bool = False,
    timeout: int = 4294967,
    technology_specifics: technology_constraints = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    on_worker_progress: Callable[[int, int, str, int, int, bool], None] | None = None,
) -> object: ...

class num_clks(enum.Enum):
    """Number of clock phases of a clocking scheme."""

    THREE = 0
    """Three clocks."""

    FOUR = 1
    """Four clocks."""

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
        pst: Statistics.

    Returns:
        A gate-level layout of type `Lyt` that implements `ntk` as an FCN
        circuit.

    Raises:
        std::invalid_argument: If `Lyt` is a shifted Cartesian or
                               hexagonal layout and
                               `layout_arrangement` is empty.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    layout_type: object,
    statistics: orthogonal_stats | None = None,
    number_of_clock_phases: num_clks = ...,
    layout_arrangement: mnt.pyfiction.layouts.arrangement | None = None,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> object: ...

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
        pst: A pointer to a statistics object to record execution details.
             Defaults to nullptr.
        custom_cost_objective: A custom cost objective that is evaluated
                               at every expansion of the search space
                               graph. Should be a function that can be
                               calculated based on the current partial
                               layout and returns an uint64_t that should
                               be minimized.

    Returns:
        The smallest layout yielded by the graph-oriented layout design
        algorithm under the given parameters.

    Raises:
        networks::high_degree_fanin_exception: If `ntk` has a node with
                                               more than two fanins.
        std::invalid_argument: If the cost objective is `CUSTOM` and no
                               custom cost objective is provided, or if
                               `tiles_to_skip_between_pis` does not lie
                               in :math:`[0, 2^{20}]`.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    via cells. Input and output cells carry the names of their objects. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed object
                     mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        Geometry: Coordinate geometry of the gate-level layout.

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
    via cells. Input and output cells carry the names of their objects. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed object
                     mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        Geometry: Coordinate geometry of the gate-level layout.

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
    via cells. Input and output cells carry the names of their objects. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed object
                     mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        Geometry: Coordinate geometry of the gate-level layout.

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
    via cells. Input and output cells carry the names of their objects. An
    SiDB layout lies on the H-Si(100)-2x1 lattice.

    May pass through, and thereby throw, an
    `unsupported_gate_type_exception` or an
    `unsupported_gate_orientation_exception`.

    Args:
        lyt: The gate-level layout.
        on_progress: Optional callback reporting completed object
                     mappings.

    Template Args:
        GateLibrary: Type of the gate library to apply.
        Geometry: Coordinate geometry of the gate-level layout.

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
        objectives: The routing objectives as source coordinates and
                    indexed destination inputs to fulfill.
        pst: Statistics.

    Returns:
        `true` iff routing was successful, i.e., iff all objectives could
        be satisfied.

    Algorithm options are keyword-only; the signature lists their defaults.
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
    Transforms a 2DDWave-clocked Cartesian layout into a hexagonal even
    row clocked layout suitable for SiDBs by remapping all gates and wires
    as originally proposed in \\"Scalable Physical Design for Silicon
    Dangling Bond Logic: How a 45° Turn Prevents the Reinvention of the
    Wheel\\" by S. Hofmann, M. Walter, and R. Wille in IEEE NANO 2023
    (https://ieeexplore.ieee.org/document/10231278).

    Args:
        lyt: 2DDWave-clocked Cartesian gate-level layout to hexagonalize.
        stats: Statistics.

    Returns:
        Hexagonal representation of the Cartesian layout.

    Raises:
        std::invalid_argument: If clocking or placed geometry does not
                               satisfy the input contract.
        std::overflow_error: If the transformed coordinates exceed the
                             signed coordinate range.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

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
    *,
    max_gate_relocations: int | None = None,
    optimize_pos_only: bool = False,
    planar_optimization: bool = False,
    timeout: int = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
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
        pst: Statistics.

    Raises:
        std::invalid_argument: If clocking or occupied geometry is
                               invalid.
        std::overflow_error: If the extent leaves no room for signed
                             routing coordinates.

    Note:
        This function requires the gate-level layout to be 2DDWave-
        clocked!

    Note:
        Exceptions may leave a partially edited layout. Object identities,
        occupancy, and connection storage remain structurally valid;
        logical equivalence and physical validity are not guaranteed after
        failure.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

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
    *,
    timeout: int = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
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
        pst: Statistics.

    Raises:
        std::invalid_argument: If clocking or occupied geometry is
                               invalid.
        std::overflow_error: If the extent leaves no room for signed
                             routing coordinates.

    Algorithm options are keyword-only; the signature lists their defaults.
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
        lyt: Layout to edit.
        path: Path containing at least its occupied source and target
              coordinates.
        destination: Ordered destination input.

    Template Args:
        Geometry: Coordinate geometry of the gate-level layout.
        Path: Coordinate path type.

    Raises:
        std::invalid_argument: If an endpoint or intermediate placement is
                               unavailable.
        std::out_of_range: If the destination input index is invalid.
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
    Extracts connections between retained gates, fanouts, and terminals
    with destination input indices.

    Intermediate single-sink wires are followed through declared topology.
    Missing inputs produce no objective; disconnected input slots keep
    their indices. Cycles of intermediate wires reject.

    Args:
        lyt: Layout to inspect.

    Template Args:
        Lyt: Gate-level layout type.

    Returns:
        Routing objectives that preserve logical input numbering.

    Raises:
        std::invalid_argument: If an intermediate wire chain contains a
                               cycle.
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
