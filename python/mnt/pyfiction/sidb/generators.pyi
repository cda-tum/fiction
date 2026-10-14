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
    Generate a random SiDB layout within coordinate_pair, optionally extending lyt_skeleton.
    The keyword options select physical values, charge restrictions, and attempt limits.
    Returns None when the attempt limit does not produce a layout.

    Args:
        coordinate_pair: The area to place SiDBs in, as two opposite corners.
        number_of_sidbs: Number of SiDBs to place.
        positive_sidbs: Allow positive charges, forbid positive charges, or require that positive charges may occur.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        maximal_attempts: Maximum number of placement attempts.
        number_of_unique_generated_layouts: Number of unique layouts to generate with `generate_multiple_random_layouts`.
        maximal_attempts_for_multiple_layouts: Maximum number of generation attempts for multiple layouts.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
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
    Generate distinct random SiDB layouts, optionally extending lyt_skeleton.
    The keyword options select the area, physical values, requested count, and attempt limits.
    Returns None if no layout is generated; the attempt limit can yield fewer layouts than requested.

    Args:
        coordinate_pair: The area to place SiDBs in, as two opposite corners.
        number_of_sidbs: Number of SiDBs to place.
        positive_sidbs: Allow positive charges, forbid positive charges, or require that positive charges may occur.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        maximal_attempts: Maximum number of placement attempts.
        number_of_unique_generated_layouts: Number of unique layouts to generate with `generate_multiple_random_layouts`.
        maximal_attempts_for_multiple_layouts: Maximum number of generation attempts for multiple layouts.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
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
    Design SiDB gates implementing the supplied truth tables within the canvas.
    The keyword options select physical values, BDL detection, the design mode, and termination.
    Expiration raises TimeoutError; on_progress reports completed search work.

    Args:
        timeout: Execution budget in milliseconds.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        design_mode: Gate-design approach: QUICKCELL, AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER, RANDOM, or PRUNING_ONLY.
        canvas: The canvas: the area canvas SiDBs are placed in, as two opposite corners.
        number_of_canvas_sidbs: Number of canvas SiDBs.
        maximal_random_design_attempts: Maximum number of layouts evaluated by random gate design across all threads.
        termination_cond: When to stop.
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        stats: Statistics object filled by the algorithm; None discards statistics.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
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
        on_progress: Optional gate-design progress callback.
        base: Number of charge states: 2 for negative and neutral, or 3 to include positive charges.
        mu_minus: Negative-charge transition energy in eV.
        epsilon_r: Relative permittivity (dimensionless).
        lambda_tf: Thomas-Fermi screening distance in nm.
        threshold_bdl_interdistance: Maximum separation between BDL pairs in one wire in nm.
        minimum_distance: Minimum SiDB separation for a BDL pair in nm.
        maximum_distance: Maximum SiDB separation for a BDL pair in nm.
        input_bdl_config: Input encoding: perturber absence or perturber distance.
        canvas_sidb_complex_gates: Number of canvas SiDBs dedicated to complex gates.
        using_predefined_crossing_and_double_wire_if_possible: Select predefined or synthesized crossing and double-wire gates.
        influence_radius_charged_defects: Radius around each gate center in nm for incorporating charged defects.
        sim_engine: SiDB simulation engine used for operational analysis.
        op_condition: Require the selected operational condition for every input pattern.
        strategy_to_analyze_operational_status: Operational-status analysis strategy.
        design_mode: Gate-design approach: QUICKCELL, AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER, RANDOM, or PRUNING_ONLY.
        canvas: Gate-design area as two opposite lattice-site corners.
        number_of_canvas_sidbs: Number of SiDBs placed in the gate-design canvas.
        maximal_random_design_attempts: Maximum candidate layouts evaluated across all random-design threads.
        termination_cond: Stop after the first solution or collect all solutions allowed by the design mode.

    Returns:
        An SiDB layout that can be exported with write_sqd_layout or write_sidb_layout_svg.

    Raises:
        TimeoutError: The circuit or an individual gate exceeds its timeout.
        RuntimeError: A gate cannot be designed with the supplied parameters.
        ValueError: Gate mapping uses a column arrangement, a gate type or orientation is unsupported, or gate parameters are invalid.
    """
