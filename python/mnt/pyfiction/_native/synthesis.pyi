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

import mnt.pyfiction._native.networks

class TruthTable:
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

    @staticmethod
    def from_binary(binary: str) -> TruthTable:
        """Constructs a truth table from most-significant-bit-first binary text."""

    @staticmethod
    def from_hex(hex: str, *, num_vars: int) -> TruthTable:
        """
        Constructs a truth table from hexadecimal text and an explicit variable count.
        """

    @staticmethod
    def from_expression(expression: str, *, num_vars: int) -> TruthTable:
        """
        Constructs a truth table from a kitty Boolean expression and variable count.
        """

@overload
def standard_functions() -> dict[str, list[TruthTable]]:
    """
    Returns fresh truth tables for every named standard function. Each value lists the outputs in specification order.
    """

@overload
def standard_functions(name: str) -> list[TruthTable]:
    """
    Returns fresh truth tables for the named function, in specification output order. Unknown names raise ValueError.
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork, params: fanout_substitution_params = ...
) -> mnt.pyfiction._native.networks.TechnologyNetwork:
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork, params: fanout_substitution_params = ...
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork, params: network_balancing_params = ...
) -> mnt.pyfiction._native.networks.TechnologyNetwork:
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

def is_balanced(
    network: mnt.pyfiction._native.networks.TechnologyNetwork, params: network_balancing_params = ...
) -> bool:
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
    def mapper_stats(self) -> mnt.pyfiction._native.synthesis.mapper_stats:
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction._native.networks.TechnologyNetwork: ...
@overload
def technology_mapping(
    network: mnt.pyfiction._native.networks.AigNetwork,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction._native.networks.TechnologyNetwork: ...
@overload
def technology_mapping(
    network: mnt.pyfiction._native.networks.XagNetwork,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction._native.networks.TechnologyNetwork: ...
@overload
def technology_mapping(
    network: mnt.pyfiction._native.networks.MigNetwork,
    params: technology_mapping_params = ...,
    stats: technology_mapping_stats | None = None,
) -> mnt.pyfiction._native.networks.TechnologyNetwork:
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork,
    target: Literal[network_target.TEC] = ...,
) -> mnt.pyfiction._native.networks.TechnologyNetwork:
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
    network: mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork,
    target: Literal[network_target.AIG],
) -> mnt.pyfiction._native.networks.AigNetwork: ...
@overload
def convert_network(
    network: mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork,
    target: Literal[network_target.XAG],
) -> mnt.pyfiction._native.networks.XagNetwork: ...
@overload
def convert_network(
    network: mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork,
    target: Literal[network_target.MIG],
) -> mnt.pyfiction._native.networks.MigNetwork: ...
@overload
def convert_network(
    network: mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork,
    target: network_target,
) -> (
    mnt.pyfiction._native.networks.TechnologyNetwork
    | mnt.pyfiction._native.networks.AigNetwork
    | mnt.pyfiction._native.networks.XagNetwork
    | mnt.pyfiction._native.networks.MigNetwork
): ...
