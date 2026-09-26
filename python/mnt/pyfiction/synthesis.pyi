# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Truth tables and logic network transformations."""

import datetime
import enum
from collections.abc import Callable
from typing import Literal, overload

import mnt.pyfiction.networks

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

class fanout_substitution_params:
    """Parameters for the fanout substitution algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives completed work and the phase total."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def strategy(self) -> substitution_strategy:
        """
        Substitution strategy of high-degree fanout networks (depth-first vs.
        breadth-first).
        """

    @strategy.setter
    def strategy(self, arg: substitution_strategy, /) -> None: ...
    @property
    def degree(self) -> int:
        """Maximum output degree of each fan-out node."""

    @degree.setter
    def degree(self, arg: int, /) -> None: ...
    @property
    def threshold(self) -> int:
        """
        Maximum number of outputs any gate is allowed to have before
        substitution applies.
        """

    @threshold.setter
    def threshold(self, arg: int, /) -> None: ...
    @property
    def seed(self) -> int | None:
        """
        Seed used for random substitution, generated randomly if not
        specified.
        """

    @seed.setter
    def seed(self, arg: int | None, /) -> None: ...

def fanout_substitution(
    network: mnt.pyfiction.networks.technology_network, params: fanout_substitution_params = ...
) -> mnt.pyfiction.networks.technology_network:
    """
    Substitutes high-output degrees in a logic network with fanout nodes
    that compute the identity function. For this purpose, `create_buf` is
    utilized. Therefore, `NtkDest` should support identity nodes. If it
    does not, no new nodes will in fact be created. In either case, the
    returned network will be logically equivalent to the input one.

    The process is rather naive with two possible strategies to pick from:
    breath-first and depth-first. The former creates partially balanced
    fanout trees while the latter leads to fanout chains. Further
    parameterization includes thresholds for the maximum number of output
    each node and fanout is allowed to have.

    The returned network is newly created from scratch because its type
    `NtkDest` may differ from `NtkSrc`.

    Args:
        ntk_src: The input logic network.
        ps: Parameters.

    Template Args:
        NtkDest: Type of the returned logic network.
        NtkSrc: Type of the input logic network.

    Returns:
        A fanout-substituted logic network of type `NtkDest` that is
        logically equivalent to `ntk_src`.

    Note:
        The physical design algorithms natively provided in fiction do not
        require their input networks to be fanout-substituted. If that is
        necessary, they will do it themselves. Providing already
        substituted networks does however allow for the control over
        maximum output degrees.
    """

def is_fanout_substituted(
    network: mnt.pyfiction.networks.technology_network, params: fanout_substitution_params = ...
) -> bool:
    """
    Checks if a logic network is properly fanout-substituted with regard
    to the provided parameters, i.e., if no node exceeds the specified
    fanout limits.

    Args:
        ntk: The logic network to check.
        ps: Parameters.

    Template Args:
        Ntk: Logic network type.

    Returns:
        `true` iff `ntk` is properly fanout-substituted with regard to
        `ps`.
    """

class network_balancing_params:
    """Parameters for the network balancing algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives completed work and the phase total."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def unify_outputs(self) -> bool:
        """Flag to indicate that all output nodes should be in the same rank."""

    @unify_outputs.setter
    def unify_outputs(self, arg: bool, /) -> None: ...

def network_balancing(
    network: mnt.pyfiction.networks.technology_network, params: network_balancing_params = ...
) -> mnt.pyfiction.networks.technology_network:
    """
    Balances a logic network with buffer nodes that compute the identity
    function. For this purpose, `create_buf` is utilized. Therefore,
    `NtkDest` should support identity nodes. If it does not, no new nodes
    will in fact be created. In either case, the returned network will be
    logically equivalent to the input one.

    The process is rather naive and is not combined with fanout
    substitution.

    The returned network is newly created from scratch because its type
    `NtkDest` may differ from `NtkSrc`.

    Args:
        ntk_src: The input logic network.
        ps: Parameters.

    Template Args:
        NtkDest: Type of the returned logic network.
        NtkSrc: Type of the input logic network.

    Returns:
        A path-balanced logic network of type `NtkDest` that is logically
        equivalent to `ntk_src`.

    Note:
        The physical design algorithms natively provided in fiction do not
        require their input networks to be balanced. If that is necessary,
        they will do it themselves. Providing already balanced networks
        may lead to substantial overhead.
    """

def is_balanced(network: mnt.pyfiction.networks.technology_network, params: network_balancing_params = ...) -> bool:
    """
    Checks if a logic network is properly path-balanced with regard to the
    provided parameters.

    Args:
        ntk: The logic network to check.
        ps: Parameters.

    Template Args:
        Ntk: Logic network type.

    Returns:
        `true` iff `ntk` is properly path-balanced with regard to `ps`.
    """

class missing_required_gates_exception(RuntimeError): ...

class technology_mapping_params:
    def __init__(self) -> None:
        """Default constructor."""

    @property
    def decay(self) -> bool:
        """
        Enforce the application of at least one constant input to three-input
        gates.
        """

    @decay.setter
    def decay(self, arg: bool, /) -> None: ...
    @property
    def inv(self) -> bool:
        """1-input NOT gate (inverter)."""

    @inv.setter
    def inv(self, arg: bool, /) -> None: ...
    @property
    def and2(self) -> bool:
        """2-input AND gate."""

    @and2.setter
    def and2(self, arg: bool, /) -> None: ...
    @property
    def nand2(self) -> bool:
        """2-input NAND gate."""

    @nand2.setter
    def nand2(self, arg: bool, /) -> None: ...
    @property
    def or2(self) -> bool:
        """2-input OR gate."""

    @or2.setter
    def or2(self, arg: bool, /) -> None: ...
    @property
    def nor2(self) -> bool:
        """2-input NOR gate."""

    @nor2.setter
    def nor2(self, arg: bool, /) -> None: ...
    @property
    def xor2(self) -> bool:
        """2-input XOR gate."""

    @xor2.setter
    def xor2(self, arg: bool, /) -> None: ...
    @property
    def xnor2(self) -> bool:
        """2-input XNOR gate."""

    @xnor2.setter
    def xnor2(self, arg: bool, /) -> None: ...
    @property
    def lt2(self) -> bool:
        """2-input less-than gate."""

    @lt2.setter
    def lt2(self, arg: bool, /) -> None: ...
    @property
    def gt2(self) -> bool:
        """2-input greater-than gate."""

    @gt2.setter
    def gt2(self, arg: bool, /) -> None: ...
    @property
    def le2(self) -> bool:
        """2-input less-or-equal gate."""

    @le2.setter
    def le2(self, arg: bool, /) -> None: ...
    @property
    def ge2(self) -> bool:
        """2-input greater-or-equal gate."""

    @ge2.setter
    def ge2(self, arg: bool, /) -> None: ...
    @property
    def and3(self) -> bool:
        """3-input AND gate."""

    @and3.setter
    def and3(self, arg: bool, /) -> None: ...
    @property
    def xor_and(self) -> bool:
        """3-input XOR-AND gate."""

    @xor_and.setter
    def xor_and(self, arg: bool, /) -> None: ...
    @property
    def or_and(self) -> bool:
        """3-input OR-AND gate."""

    @or_and.setter
    def or_and(self, arg: bool, /) -> None: ...
    @property
    def onehot(self) -> bool:
        """3-input ONEHOT gate."""

    @onehot.setter
    def onehot(self, arg: bool, /) -> None: ...
    @property
    def maj3(self) -> bool:
        """3-input MAJ gate."""

    @maj3.setter
    def maj3(self, arg: bool, /) -> None: ...
    @property
    def gamble(self) -> bool:
        """3-input GAMBLE gate."""

    @gamble.setter
    def gamble(self, arg: bool, /) -> None: ...
    @property
    def dot(self) -> bool:
        """3-input DOT gate."""

    @dot.setter
    def dot(self, arg: bool, /) -> None: ...
    @property
    def mux(self) -> bool:
        """3-input MUX gate (ITE)."""

    @mux.setter
    def mux(self, arg: bool, /) -> None: ...
    @property
    def and_xor(self) -> bool:
        """3-input AND-XOR gate."""

    @and_xor.setter
    def and_xor(self, arg: bool, /) -> None: ...

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
    def time_multioutput(self) -> datetime.timedelta: ...
    @property
    def time_total(self) -> datetime.timedelta: ...
    @property
    def round_stats(self) -> list[str]: ...

class technology_mapping_stats:
    """Statistics for technology mapping."""

    def __init__(self) -> None:
        """Default constructor."""

    def report(self) -> None:
        """Report statistics."""

    @property
    def mapper_stats(self) -> mapper_stats:
        """Statistics for mockturtle's mapper."""

def and_or_not() -> technology_mapping_params:
    """
    Auxiliary function to create technology mapping parameters for AND,
    OR, and NOT gates.

    Returns:
        Technology mapping parameters.
    """

def and_or_not_maj() -> technology_mapping_params:
    """
    Auxiliary function to create technology mapping parameters for AND,
    OR, NOT, and MAJ gates.

    Returns:
        Technology mapping parameters.
    """

def all_standard_2_input_functions() -> technology_mapping_params:
    """
    Auxiliary function to create technology mapping parameters for AND,
    OR, NAND, NOR, XOR, XNOR, and NOT gates.

    Returns:
        Technology mapping parameters.
    """

def all_standard_3_input_functions() -> technology_mapping_params:
    """
    Auxiliary function to create technology mapping parameters for AND3,
    XOR_AND, OR_AND, ONEHOT, MAJ3, GAMBLE, DOT, MUX, and AND_XOR gates.

    Returns:
        Technology mapping parameters.
    """

def all_supported_standard_functions() -> technology_mapping_params:
    """
    Auxiliary function to create technology mapping parameters for all
    supported standard functions.

    Returns:
        Technology mapping parameters.
    """

@overload
def technology_mapping(
    network: mnt.pyfiction.networks.technology_network,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.aig_network,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.xag_network,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network: ...
@overload
def technology_mapping(
    network: mnt.pyfiction.networks.mig_network,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction.networks.technology_network:
    """
    Performs technology mapping on the given network. Technology mapping
    is the process of replacing the gates in a network with gates from a
    given technology library. This function utilizes `mockturtle::emap` to
    perform the technology mapping. This function is a wrapper around that
    interface to provide a more convenient usage.

    Args:
        ntk: Input logic network.
        params: Technology mapping parameters.
        pst: Technology mapping statistics.

    Template Args:
        Ntk: Input logic network type.

    Returns:
        Mapped network exclusively using gates from the provided library.

    Raises:
        missing_required_gates_exception: if the technology library does
                                          not contain required gates for
                                          the base network type (e.g., AIG
                                          requires INV and AND; XAG
                                          requires INV, AND, and XOR; MIG
                                          requires INV and MAJ).
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
    Converts a logic network into an equivalent one of another type.
    Thereby, this function is very similar to
    `mockturtle::cleanup_dangling`. However, it supports real buffer nodes
    used for fanouts and path balancing in fiction.

    Args:
        ntk: The input logic network.

    Template Args:
        NtkDest: Type of the returned logic network.
        NtkSrc: Type of the input logic network.

    Returns:
        A logic network of type `NtkDest` that is logically equivalent to
        `ntk`.

    Note:
        If `NtkDest` and `NtkSrc` are of the same type, this function
        returns `ntk` cleaned using `mockturtle::cleanup_dangling`.
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
