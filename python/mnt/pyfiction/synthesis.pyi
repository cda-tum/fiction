# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Truth tables and logic network transformations."""

import datetime as dt
import enum
from collections.abc import Callable
from typing import Literal, overload

import mnt.pyfiction.networks
from mnt.pyfiction._types import TechnologyMappingOptions as TechnologyMappingOptions

class dynamic_truth_table:
    @overload
    def __init__(self) -> None:
        """Constructs the constant-zero truth table."""

    @overload
    def __init__(self, num_vars: int) -> None:
        """Constructs a truth table with fewer than 38 variables."""

    def num_vars(self) -> int: ...
    def num_blocks(self) -> int: ...
    def num_bits(self) -> int: ...
    def create_from_binary_string(self, binary: str) -> None:
        """
        Sets the bits from a string of `0` and `1` characters, most significant bit first. The string must hold exactly `num_bits()` characters. Any other character raises a `ValueError`.
        """

    def create_from_hex_string(self, hex: str) -> None:
        """
        Sets the bits from a hexadecimal string, most significant digit first. The string must hold exactly `num_bits() / 4` digits, or one digit for fewer than two variables. Any non-hexadecimal character raises a `ValueError`.
        """

    def create_from_expression(self, expression: str) -> None:
        """
        Sets the bits from a Boolean expression over the variables `a` to `p`: constants `0` and `1`, negation `!E`, conjunction `(E...E)`, disjunction `{E...E}`, exclusive OR `[E...E]`, and majority `<EEE>`. The truth table must have at least as many variables as the largest one the expression uses.
        """

    def create_random(self) -> None:
        """Sets every bit to a random value."""

    def to_binary(self) -> str:
        """
        Returns the bits as a string of `0` and `1` characters, most significant bit first.
        """

    def to_hex(self) -> str:
        """
        Returns the bits as a hexadecimal string, most significant digit first.
        """

def create_id_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the identity
    function in one variable.

    Returns:
        Identity function in one variable.
    """

def create_not_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the negation in one
    variable.

    Returns:
        Negation in one variable.
    """

def create_and_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the conjunction in
    two variables.

    Returns:
        Conjunction in two variables.
    """

def create_or_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the disjunction in
    two variables.

    Returns:
        Disjunction in two variables.
    """

def create_nand_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the negated
    conjunction in two variables.

    Returns:
        Negated conjunction in two variables.
    """

def create_nor_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the negated
    disjunction in two variables.

    Returns:
        Negated disjunction in two variables.
    """

def create_xor_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the exclusive
    disjunction in two variables.

    Returns:
        Exclusive disjunction in two variables.
    """

def create_xnor_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the negated
    exclusive disjunction in two variables.

    Returns:
        Negated exclusive disjunction in two variables.
    """

def create_lt_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the less-than
    function in two variables.

    Returns:
        Less-than function in two variables.
    """

def create_gt_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the greater-than
    function in two variables.

    Returns:
        Greater-than function in two variables.
    """

def create_le_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the less-than-or-
    equal function in two variables.

    Returns:
        Less-than-or-equal function in two variables.
    """

def create_ge_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the greater-than-or-
    equal function in two variables.

    Returns:
        Greater-than-or-equal function in two variables.
    """

def create_and3_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the conjunction in
    three variables.

    Returns:
        Conjunction in three variables.
    """

def create_xor_and_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the XOR-AND function
    (a and (b xor c)) in three variables.

    Returns:
        XOR-AND in three variables.
    """

def create_or_and_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the OR-AND function
    (a and (b or c)) in three variables.

    Returns:
        OR-AND in three variables.
    """

def create_onehot_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the Onehot function
    (exactly one of a,b,c) in three variables.

    Returns:
        Onehot in three variables.
    """

def create_maj_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the majority
    function in three variables.

    Returns:
        Majority function in three variables.
    """

def create_gamble_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the Gamble function
    (all or none of a,b,c) in three variables.

    Returns:
        Gamble in three variables.
    """

def create_dot_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the Dot function (a
    xor (c or a and b)) in three variables.

    Returns:
        Dot function in three variables.
    """

def create_ite_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the ITE (MUX)
    function (if a then b else c) in three variables.

    Returns:
        ITE (MUX) in three variables.
    """

def create_and_xor_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the AND-XOR function
    (a xor b and c) in three variables.

    Returns:
        AND-XOR in three variables.
    """

def create_xor3_tt() -> dynamic_truth_table:
    """
    Creates and returns a truth table that implements the exclusive
    disjunction in three variables.

    Returns:
        Exclusive disjunction in three variables.
    """

def create_double_wire_tt() -> list[dynamic_truth_table]:
    """
    Creates and returns a vector of truth tables for a double wire multi-
    output function.

    This function generates a vector of truth tables, each representing
    one of the outputs of a double wire multi-output function in two
    variables. The function returns a vector containing two truth tables.

    Returns:
        Vector of truth tables, each representing an output of the double
        wire function.
    """

def create_crossing_wire_tt() -> list[dynamic_truth_table]:
    """
    Creates and returns a vector of truth tables for a crossing wire
    multi-output function.

    This function generates a vector of truth tables, each representing
    one of the outputs of a crossing wire multi-output function in two
    variables. The function returns a vector containing two truth tables.

    Returns:
        Vector of truth tables, each representing an output of the
        crossing wire function.
    """

def create_fan_out_tt() -> list[dynamic_truth_table]:
    """
    Creates and returns a vector of truth tables for a multi-output
    function with two variables.

    This function generates a vector of truth tables, each representing
    one of the outputs of a multi-output function in two variables.

    Returns:
        Vector of truth tables, each representing an output of the
        identity function.
    """

def create_half_adder_tt() -> list[dynamic_truth_table]:
    """
    Creates and returns a vector of truth tables for a half adder multi-
    output function.

    This function generates a vector of truth tables, each representing
    one of the outputs of a half adder multi-output function in two
    variables. The function returns a vector containing two truth tables.

    Returns:
        Vector of truth tables, each representing an output of the half
        adder function.
    """

class substitution_strategy(enum.Enum):
    """Breadth-first vs. depth-first fanout-tree substitution strategies."""

    BREADTH = 0
    """Breadth-first substitution. Creates balanced fanout trees."""

    DEPTH = 1
    """Depth-first substitution. Creates fanout trees with one deep branch."""

    RANDOM = 2
    """
    Random substitution. Inserts fanout buffers at random positions in the
    fanout tree.
    """

def fanout_substitution(
    network: mnt.pyfiction.networks.technology_network,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    strategy: substitution_strategy = ...,
    degree: int = 2,
    threshold: int = 1,
    seed: int | None = None,
) -> mnt.pyfiction.networks.technology_network:
    """
    Insert identity nodes so the network respects the keyword fanout limits.
    Returns a fresh technology network that is logically equivalent to the input.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        strategy: Substitution strategy of high-degree fanout networks (depth-first vs. breadth-first).
        degree: Maximum output degree of each fan-out node. Must be at least two.
        threshold: Maximum number of outputs any gate is allowed to have before substitution applies. Must be at least one.
        seed: Random seed; None selects a fresh random seed.
    """

def is_fanout_substituted(
    network: mnt.pyfiction.networks.technology_network,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    strategy: substitution_strategy = ...,
    degree: int = 2,
    threshold: int = 1,
    seed: int | None = None,
) -> bool:
    """
    Check whether every network node respects the keyword fanout limits.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        strategy: Substitution strategy of high-degree fanout networks (depth-first vs. breadth-first).
        degree: Maximum output degree of each fan-out node. Must be at least two.
        threshold: Maximum number of outputs any gate is allowed to have before substitution applies. Must be at least one.
        seed: Random seed; None selects a fresh random seed.
    """

def network_balancing(
    network: mnt.pyfiction.networks.technology_network,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    unify_outputs: bool = False,
) -> mnt.pyfiction.networks.technology_network:
    """
    Insert identity nodes to balance network paths.
    Returns a fresh technology network that is logically equivalent to the input.
    Set unify_outputs to place every output at the same logic depth.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        unify_outputs: Flag to indicate that all output nodes should be in the same rank.
    """

def is_balanced(
    network: mnt.pyfiction.networks.technology_network,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    unify_outputs: bool = False,
) -> bool:
    """
    Check whether network paths are balanced.
    Set unify_outputs to require every output at the same logic depth.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        unify_outputs: Flag to indicate that all output nodes should be in the same rank.
    """

class missing_required_gates_exception(RuntimeError): ...

class mapper_stats:
    """Technology mapper results, including failure status."""

    @property
    def mapping_error(self) -> bool: ...
    @property
    def area(self) -> float: ...
    @property
    def delay(self) -> float: ...
    @property
    def power(self) -> float: ...
    @property
    def inverters(self) -> int: ...
    @property
    def multioutput_gates(self) -> int: ...
    @property
    def time_multioutput(self) -> dt.timedelta: ...
    @property
    def time_total(self) -> dt.timedelta: ...
    @property
    def round_stats(self) -> list[str]: ...

class technology_mapping_stats:
    """Statistics for technology mapping."""

    def __init__(self) -> None:
        """Default constructor."""

    def report(self) -> None:
        """Report statistics."""

    @property
    def mapper_stats(self) -> mnt.pyfiction.synthesis.mapper_stats:
        """Statistics for mockturtle's mapper."""

def and_or_not() -> TechnologyMappingOptions:
    """Return boolean mapping keywords for AND, OR, and NOT gates."""

def and_or_not_maj() -> TechnologyMappingOptions:
    """Return boolean mapping keywords for AND, OR, NOT, and MAJ gates."""

def all_standard_2_input_functions() -> TechnologyMappingOptions:
    """Return boolean mapping keywords for standard two-input functions."""

def all_standard_3_input_functions() -> TechnologyMappingOptions:
    """Return boolean mapping keywords for standard three-input functions."""

def all_supported_standard_functions() -> TechnologyMappingOptions:
    """Return boolean mapping keywords for all supported standard functions."""

@overload
def technology_mapping(
    network: mnt.pyfiction.networks.technology_network,
    *,
    decay: bool = False,
    inv: bool = False,
    and2: bool = False,
    nand2: bool = False,
    or2: bool = False,
    nor2: bool = False,
    xor2: bool = False,
    xnor2: bool = False,
    lt2: bool = False,
    gt2: bool = False,
    le2: bool = False,
    ge2: bool = False,
    and3: bool = False,
    xor_and: bool = False,
    or_and: bool = False,
    onehot: bool = False,
    maj3: bool = False,
    gamble: bool = False,
    dot: bool = False,
    mux: bool = False,
    and_xor: bool = False,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.aig_network,
    *,
    decay: bool = False,
    inv: bool = False,
    and2: bool = False,
    nand2: bool = False,
    or2: bool = False,
    nor2: bool = False,
    xor2: bool = False,
    xnor2: bool = False,
    lt2: bool = False,
    gt2: bool = False,
    le2: bool = False,
    ge2: bool = False,
    and3: bool = False,
    xor_and: bool = False,
    or_and: bool = False,
    onehot: bool = False,
    maj3: bool = False,
    gamble: bool = False,
    dot: bool = False,
    mux: bool = False,
    and_xor: bool = False,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.xag_network,
    *,
    decay: bool = False,
    inv: bool = False,
    and2: bool = False,
    nand2: bool = False,
    or2: bool = False,
    nor2: bool = False,
    xor2: bool = False,
    xnor2: bool = False,
    lt2: bool = False,
    gt2: bool = False,
    le2: bool = False,
    ge2: bool = False,
    and3: bool = False,
    xor_and: bool = False,
    or_and: bool = False,
    onehot: bool = False,
    maj3: bool = False,
    gamble: bool = False,
    dot: bool = False,
    mux: bool = False,
    and_xor: bool = False,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.mig_network,
    *,
    decay: bool = False,
    inv: bool = False,
    and2: bool = False,
    nand2: bool = False,
    or2: bool = False,
    nor2: bool = False,
    xor2: bool = False,
    xnor2: bool = False,
    lt2: bool = False,
    gt2: bool = False,
    le2: bool = False,
    ge2: bool = False,
    and3: bool = False,
    xor_and: bool = False,
    or_and: bool = False,
    onehot: bool = False,
    maj3: bool = False,
    gamble: bool = False,
    dot: bool = False,
    mux: bool = False,
    and_xor: bool = False,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network:
    """
    Map the input network to the gates enabled by the boolean keyword options.
    No gates are enabled by default; missing required gates raise missing_required_gates_exception.
    Returns a technology network; stats receives mapper statistics when supplied.
    Gate-set factory dictionaries can be expanded into these keywords.

    Args:
        decay: Enforce the application of at least one constant input to three-input gates.
        inv: 1-input NOT gate (inverter).
        and2: 2-input AND gate.
        nand2: 2-input NAND gate.
        or2: 2-input OR gate.
        nor2: 2-input NOR gate.
        xor2: 2-input XOR gate.
        xnor2: 2-input XNOR gate.
        lt2: 2-input less-than gate.
        gt2: 2-input greater-than gate.
        le2: 2-input less-or-equal gate.
        ge2: 2-input greater-or-equal gate.
        and3: 3-input AND gate.
        xor_and: 3-input XOR-AND gate.
        or_and: 3-input OR-AND gate.
        onehot: 3-input ONEHOT gate.
        maj3: 3-input MAJ gate.
        gamble: 3-input GAMBLE gate.
        dot: 3-input DOT gate.
        mux: 3-input MUX gate (ITE).
        and_xor: 3-input AND-XOR gate.
        stats: Statistics object filled by the algorithm; None discards statistics.
    """

class network_target(enum.Enum):
    """The network types `convert_network` produces."""

    TEC = 0
    """A technology network."""

    AIG = 1
    """An AND-inverter graph."""

    XAG = 2
    """An XOR-AND-inverter graph."""

    MIG = 3
    """A majority-inverter graph."""

@overload
def convert_network(
    network: mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network,
    target: Literal[network_target.TEC] = ...,
) -> mnt.pyfiction.networks.technology_network:
    """
    Convert a logic network to the selected network type.
    Real buffer nodes used for fanouts and path balancing are supported. Converting
    to the input network's type returns a copy with dangling nodes removed.

    Args:
        network: Input logic network.
        target: Network type to produce: TEC, AIG, XAG, or MIG. Defaults to network_target.TEC.

    Returns:
        A logically equivalent network of the selected type.

    Raises:
        ValueError: A gate has neither a supported implementation nor a truth table.
    """

@overload
def convert_network(
    network: mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network,
    target: Literal[network_target.AIG],
) -> mnt.pyfiction.networks.aig_network: ...
@overload
def convert_network(
    network: mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network,
    target: Literal[network_target.XAG],
) -> mnt.pyfiction.networks.xag_network: ...
@overload
def convert_network(
    network: mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network,
    target: Literal[network_target.MIG],
) -> mnt.pyfiction.networks.mig_network: ...
@overload
def convert_network(
    network: mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network,
    target: network_target,
) -> (
    mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network
): ...
