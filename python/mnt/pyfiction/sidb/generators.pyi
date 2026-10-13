# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Generators of SiDB layouts and circuits."""

import enum
from collections.abc import Callable, Sequence

import mnt.pyfiction.layouts
import mnt.pyfiction.sidb
import mnt.pyfiction.sidb.simulation
import mnt.pyfiction.sidb.simulation.logic
import mnt.pyfiction.synthesis

class positive_charges(enum.Enum):
    """Whether positively charged SiDBs may occur in the generated layout."""

    ALLOWED = 0
    """Positive charges are allowed."""

    FORBIDDEN = 1
    """
    Positive charges are forbidden: SiDBs that would enable them are
    removed again.
    """

    MAY_OCCUR = 2
    """
    Positive charges have to be possible: generation is repeated until
    they are.
    """

def generate_random_sidb_layout(
    lyt_skeleton: mnt.pyfiction.sidb.sidb_layout | None = None,
    *,
    coordinate_pair: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site] = ...,
    number_of_sidbs: int = 0,
    positive_sidbs: positive_charges = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    maximal_attempts: int = 10000000,
    number_of_unique_generated_layouts: int = 1,
    maximal_attempts_for_multiple_layouts: int = 1000000,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> mnt.pyfiction.sidb.sidb_layout | None:
    """
    Generates a random SiDB layout by placing SiDBs at random sites of an
    area, optionally on top of a skeleton. The skeleton's lattice, dots,
    and defects carry over; sites that hold a defect or that a neutral
    defect affects are left empty. Depending on the positive charge
    policy, SiDBs that would allow positive charges are removed again, or
    the generation is repeated until positive charges are possible.

    Args:
        skeleton: The skeleton to place SiDBs on, if any.

    Returns:
        The generated layout, or `std::nullopt` if not all SiDBs could be
        placed within the attempt limit.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

def generate_multiple_random_sidb_layouts(
    lyt_skeleton: mnt.pyfiction.sidb.sidb_layout | None = None,
    *,
    coordinate_pair: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site] = ...,
    number_of_sidbs: int = 0,
    positive_sidbs: positive_charges = ...,
    base: int = 3,
    mu_minus: float = -0.32,
    epsilon_r: float = 5.6,
    lambda_tf: float = 5.0,
    maximal_attempts: int = 10000000,
    number_of_unique_generated_layouts: int = 1,
    maximal_attempts_for_multiple_layouts: int = 1000000,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> list[mnt.pyfiction.sidb.sidb_layout] | None:
    """
    Generates several unique random SiDB layouts with
    `generate_random_layout`.

    Args:
        skeleton: The skeleton to place SiDBs on, if any.

    Returns:
        The layouts, or `std::nullopt` if none could be generated within
        the attempt limit.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

class design_sidb_gates_stats:
    """Statistics of the gate designers."""

    def __init__(self) -> None:
        """Default constructor."""

class design_sidb_gates_mode(enum.Enum):
    """The design mode."""

    QUICKCELL = 0
    """
    *QuickCell*: prune the canvas layouts with the filters of the
    operational check, then simulate the rest.
    """

    AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER = 1
    """Simulate every canvas layout."""

    RANDOM = 2
    """
    Place canvas SiDBs at random until an operational gate is found or the
    attempt limit is reached.
    """

    PRUNING_ONLY = 3
    """
    Only run the pruning filters of *QuickCell* and return the surviving
    candidates.
    """

class termination_condition(enum.Enum):
    AFTER_FIRST_SOLUTION = 0

    ALL_COMBINATIONS_ENUMERATED = 1

def design_sidb_gates(
    skeleton: mnt.pyfiction.sidb.sidb_layout,
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    *,
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
    design_mode: design_sidb_gates_mode = ...,
    canvas: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site] = ...,
    number_of_canvas_sidbs: int = 1,
    maximal_random_design_attempts: int = 1000000,
    termination_cond: termination_condition = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    stats: design_sidb_gates_stats | None = None,
) -> list[mnt.pyfiction.sidb.sidb_layout]:
    """
    Designs SiDB gates on a skeleton: canvas SiDBs are placed in the
    canvas area so that the skeleton's input and output wires implement
    the given Boolean function(s). The design mode chooses between
    simulating every combination of canvas SiDBs, *QuickCell*'s pruning
    followed by simulation, random placement, and pruning only.

    Worker exceptions propagate to the caller after all started workers
    finish.

    Random placement samples at most `maximal_random_design_attempts`
    candidates without enumerating canvas layouts.

    The timeout covers setup and all search phases. Expiration discards
    partial results and stops all workers before throwing. Allocation and
    non-interruptible setup may exceed the cooperative deadline.

    *QuickCell* is described in "Towards Fast Automatic Design of Silicon
    Dangling Bond Logic" by J. Drewniok, M. Walter, S. S. H. Ng, K. Walus,
    and R. Wille in DATE 2025
    (https://ieeexplore.ieee.org/abstract/document/10992885) and
    "QuickCell: Fast Automatic Design of Standard Cells for Silicon
    Dangling Bond Logic" by the same authors in TCAD 2025
    (https://ieeexplore.ieee.org/document/11146893). The exhaustive
    designer is described in "Minimal Design of SiDB Gates: An Optimal
    Basis for Circuits Based on Silicon Dangling Bonds" by J. Drewniok, M.
    Walter, and R. Wille in NANOARCH 2023
    (https://dl.acm.org/doi/10.1145/3611315.3633241).

    Args:
        skeleton: The skeleton with its input and output wires.
        spec: The Boolean function(s) to implement; must not be empty.
        stats: Statistics.

    Returns:
        The designed gates.

    Raises:
        std::invalid_argument: if `spec` is empty or the input wire count
                               differs from the specification.
        utils::timeout_error: if the gate-design deadline is reached.

    Algorithm options are keyword-only; the signature lists their defaults.
    """

class sidb_complex_gate_design_policy(enum.Enum):
    """
    This struct represents the policy for complex (i.e., crossing, double
    wire, half-adder) gate design.
    """

    USING_PREDEFINED = 0
    """Use predefined complex gates if possible."""

    DESIGN_ON_THE_FLY = 1
    """Design complex gates on-the-fly."""

def on_the_fly_sidb_circuit_design(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    *,
    timeout: int = ...,
    design_gate_timeout: int = ...,
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
    design_mode: design_sidb_gates_mode = ...,
    canvas: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site] = ...,
    number_of_canvas_sidbs: int = 1,
    maximal_random_design_attempts: int = 1000000,
    termination_cond: termination_condition = ...,
    on_progress: Callable[[str, int, int], None] | None = None,
    canvas_sidb_complex_gates: int = 3,
    using_predefined_crossing_and_double_wire_if_possible: sidb_complex_gate_design_policy = ...,
    influence_radius_charged_defects: float = 15.0,
) -> mnt.pyfiction.sidb.sidb_layout:
    """
    Design an SiDB circuit from a placed and routed pointy-top hexagonal gate-level layout.

    The function copies the layout and keyword options before releasing the GIL. The input layout is not modified.
    This function does not perform placement and routing or accept a defective surface.
    The optional timeout bounds the whole circuit search in milliseconds. Deadline checks are cooperative;
    use a separate process when an exact cutoff is required. A timeout never returns a partial circuit.

    Args:
        layout: A pointy-top hexagonal gate-level layout with a row arrangement and supported Bestagon port orientations.
        timeout: Whole-circuit deadline in milliseconds.
        design_gate_timeout: Deadline for each individual gate in milliseconds.
        base, mu_minus, epsilon_r, lambda_tf: Physical simulation values.
        on_progress: Optional gate-design progress callback.

    Returns:
        An SiDB layout that can be exported with write_sqd_layout or write_sidb_layout_svg.

    Raises:
        TimeoutError: The circuit or an individual gate exceeds its timeout.
        RuntimeError: A gate cannot be designed with the supplied parameters.
        ValueError: Gate mapping uses a column arrangement, a gate type or orientation is unsupported, or gate parameters are invalid.
    """
