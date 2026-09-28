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
import mnt.pyfiction.sidb.model
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

class generate_random_sidb_layout_params:
    """Parameters of the random layout generator."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def coordinate_pair(self) -> tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site]:
        """The area to place SiDBs in, as two opposite corners."""

    @coordinate_pair.setter
    def coordinate_pair(
        self, arg: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site], /
    ) -> None: ...
    @property
    def number_of_sidbs(self) -> int:
        """Number of SiDBs to place."""

    @number_of_sidbs.setter
    def number_of_sidbs(self, arg: int, /) -> None: ...
    @property
    def positive_sidbs(self) -> positive_charges:
        """Positive charge policy."""

    @positive_sidbs.setter
    def positive_sidbs(self, arg: positive_charges, /) -> None: ...
    @property
    def simulation_parameters(self) -> mnt.pyfiction.sidb.model.sidb_simulation_parameters:
        """Physical parameters for the positive charge check."""

    @simulation_parameters.setter
    def simulation_parameters(self, arg: mnt.pyfiction.sidb.model.sidb_simulation_parameters, /) -> None: ...
    @property
    def maximal_attempts(self) -> int:
        """Maximum number of placement attempts."""

    @maximal_attempts.setter
    def maximal_attempts(self, arg: int, /) -> None: ...
    @property
    def number_of_unique_generated_layouts(self) -> int:
        """
        Number of unique layouts to generate with
        `generate_multiple_random_layouts`.
        """

    @number_of_unique_generated_layouts.setter
    def number_of_unique_generated_layouts(self, arg: int, /) -> None: ...
    @property
    def maximal_attempts_for_multiple_layouts(self) -> int:
        """Maximum number of generation attempts for multiple layouts."""

    @maximal_attempts_for_multiple_layouts.setter
    def maximal_attempts_for_multiple_layouts(self, arg: int, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """
        Callback that receives the number of placed SiDBs and, for multiple
        layouts, the number of generated layouts.
        """

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

def generate_random_sidb_layout(
    params: generate_random_sidb_layout_params, lyt_skeleton: mnt.pyfiction.sidb.sidb_layout | None = None
) -> mnt.pyfiction.sidb.sidb_layout | None:
    """
    Generates a random SiDB layout by placing SiDBs at random sites of an
    area, optionally on top of a skeleton. The skeleton's lattice, dots,
    and defects carry over; sites that hold a defect or that a neutral
    defect affects are left empty. Depending on the positive charge
    policy, SiDBs that would allow positive charges are removed again, or
    the generation is repeated until positive charges are possible.

    Args:
        params: Parameters.
        skeleton: The skeleton to place SiDBs on, if any.

    Returns:
        The generated layout, or `std::nullopt` if not all SiDBs could be
        placed within the attempt limit.
    """

def generate_multiple_random_sidb_layouts(
    params: generate_random_sidb_layout_params, lyt_skeleton: mnt.pyfiction.sidb.sidb_layout | None = None
) -> list[mnt.pyfiction.sidb.sidb_layout] | None:
    """
    Generates several unique random SiDB layouts with
    `generate_random_layout`.

    Args:
        params: Parameters; `number_of_unique_generated_layouts` layouts
                are requested.
        skeleton: The skeleton to place SiDBs on, if any.

    Returns:
        The layouts, or `std::nullopt` if none could be generated within
        the attempt limit.
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

class design_sidb_gates_params:
    """Parameters of the gate designers."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def operational_params(self) -> mnt.pyfiction.sidb.simulation.logic.is_operational_params:
        """Parameters of the operational check."""

    @operational_params.setter
    def operational_params(self, arg: mnt.pyfiction.sidb.simulation.logic.is_operational_params, /) -> None: ...
    @property
    def design_mode(self) -> design_sidb_gates_mode:
        """The design mode."""

    @design_mode.setter
    def design_mode(self, arg: design_sidb_gates_mode, /) -> None: ...
    @property
    def canvas(self) -> tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site]:
        """
        The canvas: the area canvas SiDBs are placed in, as two opposite
        corners.
        """

    @canvas.setter
    def canvas(self, arg: tuple[mnt.pyfiction.sidb.lattice_site, mnt.pyfiction.sidb.lattice_site], /) -> None: ...
    @property
    def number_of_canvas_sidbs(self) -> int:
        """Number of canvas SiDBs."""

    @number_of_canvas_sidbs.setter
    def number_of_canvas_sidbs(self, arg: int, /) -> None: ...
    @property
    def maximal_random_design_attempts(self) -> int:
        """
        Maximum number of layouts evaluated by random gate design across all
        threads.
        """

    @maximal_random_design_attempts.setter
    def maximal_random_design_attempts(self, arg: int, /) -> None: ...
    @property
    def termination_cond(self) -> termination_condition:
        """When to stop."""

    @termination_cond.setter
    def termination_cond(self, arg: termination_condition, /) -> None: ...
    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Callback that receives the progress of the design mode's main loop."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...

def design_sidb_gates(
    skeleton: mnt.pyfiction.sidb.sidb_layout,
    spec: Sequence[mnt.pyfiction.synthesis.dynamic_truth_table],
    params: design_sidb_gates_params = ...,
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
        params: Parameters.
        stats: Statistics.

    Returns:
        The designed gates.

    Raises:
        std::invalid_argument: if `spec` is empty or the input wire count
                               differs from the specification.
        utils::timeout_error: if the gate-design deadline is reached.
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

class sidb_on_the_fly_gate_library_params:
    """
    This struct encapsulates parameters for the parameterized SiDB gate
    library.
    """

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def design_gate_params(self) -> design_sidb_gates_params:
        """This struct holds parameters to design SiDB gates."""

    @design_gate_params.setter
    def design_gate_params(self, arg: design_sidb_gates_params, /) -> None: ...
    @property
    def canvas_sidb_complex_gates(self) -> int:
        """
        This variable defines the number of canvas SiDBs dedicated to complex
        gates, such as crossing, double wire, and half-adder.
        """

    @canvas_sidb_complex_gates.setter
    def canvas_sidb_complex_gates(self, arg: int, /) -> None: ...
    @property
    def using_predefined_crossing_and_double_wire_if_possible(self) -> sidb_complex_gate_design_policy:
        """This variable specifies the policy for complex gate design."""

    @using_predefined_crossing_and_double_wire_if_possible.setter
    def using_predefined_crossing_and_double_wire_if_possible(
        self, arg: sidb_complex_gate_design_policy, /
    ) -> None: ...
    @property
    def influence_radius_charged_defects(self) -> float:
        """
        This variable specifies the radius in nanometers around the center of
        the hexagon where atomic defects are incorporated into the gate
        design.
        """

    @influence_radius_charged_defects.setter
    def influence_radius_charged_defects(self, arg: float, /) -> None: ...

class on_the_fly_sidb_circuit_design_params:
    """This struct stores the parameters to design an SiDB circuit."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def timeout(self) -> int:
        """
        Total timeout in milliseconds for all gates and layout conversion. The
        maximum value means unlimited; zero expires immediately. Every gate
        shares the same deadline, including crossings and double wires.
        Timeout checks are cooperative; allocation and non-interruptible setup
        can exceed the budget.
        """

    @timeout.setter
    def timeout(self, arg: int, /) -> None: ...
    @property
    def sidb_on_the_fly_gate_library_parameters(self) -> sidb_on_the_fly_gate_library_params:
        """Parameters for the SiDB on-the-fly gate library."""

    @sidb_on_the_fly_gate_library_parameters.setter
    def sidb_on_the_fly_gate_library_parameters(self, arg: sidb_on_the_fly_gate_library_params, /) -> None: ...

def on_the_fly_sidb_circuit_design(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout, params: on_the_fly_sidb_circuit_design_params = ...
) -> mnt.pyfiction.sidb.sidb_layout:
    """
    Design an SiDB circuit from a placed and routed hexagonal gate-level layout.

    The layout and parameters are copied before releasing the GIL. The input layout is not modified.
    This function does not perform placement and routing or accept a defective surface.
    The optional timeout bounds the whole circuit search in milliseconds. Deadline checks are cooperative;
    use a separate process when an exact cutoff is required. A timeout never returns a partial circuit.

    Args:
        layout: A hexagonal gate-level layout with supported Bestagon port orientations.
        params: Circuit and individual-gate design parameters.

    Returns:
        An SiDB layout that can be exported with write_sqd_layout or write_sidb_layout_svg.

    Raises:
        TimeoutError: The circuit or an individual gate exceeds its timeout.
        RuntimeError: A gate cannot be designed with the supplied parameters.
        ValueError: A gate type or orientation is unsupported, or gate parameters are invalid.
    """
