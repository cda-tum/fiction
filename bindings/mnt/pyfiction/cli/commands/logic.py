# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Logic commands: technology mapping, network transformations, simulation, and AIG optimization.

AIG optimization runs in ``aigverse``. Its networks are not the same Python objects as
``mnt.pyfiction``'s, so :func:`to_aigverse` and :func:`from_aigverse` hand an AIG across through
files in the session's temporary directory: AIGER on the way out, gate-level Verilog on the way
back, because binary AIGER written through a text stream is corrupt on Windows.
"""

from __future__ import annotations

import math
import secrets
from typing import TYPE_CHECKING

from aigverse import abc
from aigverse.algorithms import aig_cut_rewriting, aig_resubstitution, balancing, cleanup_dangling, sop_refactoring
from aigverse.generators import (
    binary_decoder,
    carry_lookahead_adder,
    multiplexer,
    random_aig,
    ripple_carry_adder,
    ripple_carry_multiplier,
)
from aigverse.io import read_aiger_into_aig
from aigverse.io import write_verilog as aigverse_write_verilog
from rich.table import Table

from mnt.pyfiction import (
    aig_network,
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    count_gate_types,
    dynamic_truth_table,
    fanout_substitution,
    fanout_substitution_params,
    get_name,
    network_balancing,
    network_balancing_params,
    read_aig_network,
    set_name,
    simulate,
    substitution_strategy,
    technology_mapping,
    technology_mapping_params,
    technology_mapping_stats,
    technology_network,
    write_aiger,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command, one_store, store_flags
from mnt.pyfiction.cli.session import stats_to_dict
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable

    from aigverse.networks import Aig

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session

GATE_FLAGS = (
    ("and", "and2"),
    ("nand", "nand2"),
    ("or", "or2"),
    ("nor", "nor2"),
    ("xor", "xor2"),
    ("xnor", "xnor2"),
    ("lt", "lt2"),
    ("gt", "gt2"),
    ("le", "le2"),
    ("ge", "ge2"),
    ("inv", "inv"),
    ("maj", "maj3"),
    ("dot", "dot"),
    ("and3", "and3"),
    ("xor-and", "xor_and"),
    ("or-and", "or_and"),
    ("onehot", "onehot"),
    ("gamble", "gamble"),
    ("mux", "mux"),
    ("and-xor", "and_xor"),
)
"""The ``map`` flags and the ``technology_mapping_params`` fields they set."""

# resubstitution and refactoring return ``None`` only for ``inplace=True``, which the passes never set
AIG_PASSES: dict[str, Callable[[Aig], Aig | None]] = {
    "rewrite": aig_cut_rewriting,
    "resub": aig_resubstitution,
    "refactor": sop_refactoring,
    "balance": balancing,
    "cleanup": cleanup_dangling,
}
"""The ``aig`` passes and the ``aigverse`` algorithms that run them."""

GENERATORS: dict[str, Callable[[int], Aig]] = {
    "rca": ripple_carry_adder,
    "cla": carry_lookahead_adder,
    "multiplier": ripple_carry_multiplier,
    "mux": multiplexer,
    "decoder": binary_decoder,
}
"""The ``generate`` kinds and the ``aigverse`` generators that build them."""

ABC_SCRIPTS = ("resyn", "resyn2", "resyn3", "compress", "compress2", "resyn2rs", "compress2rs", "dc2")
"""The named ABC scripts ``aigverse.abc`` provides."""


def to_aigverse(session: Session, network: aig_network) -> Aig:
    """Hand an AIG to ``aigverse``.

    Args:
        session: The session, for a temporary file.
        network: The AIG.

    Returns:
        The same network as an ``aigverse`` AIG with its names.
    """
    path = session.temp_file(".aig")
    write_aiger(network, str(path))
    return read_aiger_into_aig(str(path))


def from_aigverse(session: Session, aig: Aig, name: str, like: aig_network | None = None) -> aig_network:
    """Take an AIG back from ``aigverse``.

    Verilog carries no input or output names, so they are copied from ``like`` when given.

    Args:
        session: The session, for a temporary file.
        aig: The ``aigverse`` AIG.
        name: The network name to assign.
        like: A network with the same inputs and outputs whose names the result takes over.

    Returns:
        The same network as an ``mnt.pyfiction`` AIG.
    """
    path = session.temp_file(".v")
    aigverse_write_verilog(aig, str(path))
    network = read_aig_network(str(path))
    set_name(network, name)
    if like is not None:
        for source, target in zip(like.pis(), network.pis(), strict=True):
            if like.has_name(source):
                network.set_name(target, like.get_name(source))
        for index in range(like.num_pos()):
            if like.has_output_name(index):
                network.set_output_name(index, like.get_output_name(index))
    return network


def _map_arguments(parser: Parser) -> None:
    gates = parser.add_argument_group("gate types")
    for flag, _ in GATE_FLAGS:
        gates.add_argument(f"--{flag}", action="store_true", help=f"allow {flag.upper().replace('-', ' and ')}")
    gates.add_argument("--all2", action="store_true", help="every 2-input function")
    gates.add_argument("--all3", action="store_true", help="every 3-input function")
    gates.add_argument("--all", action="store_true", help="every supported function")
    parser.add_argument("--decay", action="store_true", help="also try to reduce the gate count")


@command("map", Category.LOGIC, _map_arguments)
def map_command(session: Session, args: argparse.Namespace) -> Result:
    """Map the active network onto a set of gate types, producing a technology network.

    Select at least one gate type, e.g. 'map --and --or --inv' for AND-OR-inverter networks.
    """
    params = technology_mapping_params()
    if args.all:
        params = all_supported_standard_functions()
    elif args.all3:
        params = all_standard_3_input_functions()
    elif args.all2:
        params = all_standard_2_input_functions()
    selected = [field for flag, field in GATE_FLAGS if getattr(args, flag.replace("-", "_"))]
    for field in selected:
        setattr(params, field, True)
    if not selected and not (args.all or args.all2 or args.all3):
        msg = "select at least one gate type, e.g. 'map --and --or --inv'"
        raise CommandError(msg)
    params.decay = args.decay

    network = session.networks.current()
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, params, stats)
    if not get_name(mapped):
        set_name(mapped, get_name(network))
    session.networks.add(mapped)
    return {"network": describe(mapped), "stats": stats_to_dict(stats)}


def _fanouts_arguments(parser: Parser) -> None:
    parser.add_argument("-d", "--degree", type=int, choices=[2, 3], default=2, help="fan-out nodes' output degree")
    parser.add_argument(
        "-s",
        "--strategy",
        choices=["breadth", "depth", "random"],
        default="breadth",
        help="how cascaded fan-outs are arranged (default: breadth)",
    )
    parser.add_argument("-t", "--threshold", type=int, default=1, help="maximum output degree left unsubstituted")
    parser.add_argument("--seed", type=int, help="seed for the random strategy")


@command("fanouts", Category.LOGIC, _fanouts_arguments)
def fanouts(session: Session, args: argparse.Namespace) -> Result:
    """Replace high-degree outputs of the active network by fan-out nodes.

    Physical design does this with default settings; run it beforehand to choose the settings.
    """
    params = fanout_substitution_params()
    params.degree = args.degree
    params.threshold = args.threshold
    params.strategy = getattr(substitution_strategy, args.strategy.upper())
    if args.seed is not None:
        params.seed = args.seed
    network = fanout_substitution(session.as_technology_network(session.networks.current()), params)
    session.networks.add(network)
    return {"network": describe(network)}


def _balance_arguments(parser: Parser) -> None:
    parser.add_argument("-u", "--unify-outputs", action="store_true", help="balance the outputs against each other")


@command("balance", Category.LOGIC, _balance_arguments)
def balance(session: Session, args: argparse.Namespace) -> Result:
    """Balance the paths of the active network with buffers, so every path to a gate has the same length.

    Physical design does not need this and balanced networks produce much larger layouts.
    """
    params = network_balancing_params()
    params.unify_outputs = args.unify_outputs
    network = network_balancing(session.as_technology_network(session.networks.current()), params)
    session.networks.add(network)
    return {"network": describe(network)}


def _gates_arguments(parser: Parser) -> None:
    store_flags(parser, "network", "gate_layout")
    parser.add_argument("--detailed", action="store_true", help="also list the rarer gate types")


@command("gates", Category.LOGIC, _gates_arguments)
def gates(session: Session, args: argparse.Namespace) -> Result:
    """Count the gate types of the active network or gate-level layout."""
    store = one_store(args, "network", "gate_layout")
    element = session.networks.current() if store == "network" else session.gate_layouts.current()
    counts = stats_to_dict(count_gate_types(element))
    table = Table(box=None, show_header=False, padding=(0, 2))
    for line in count_gate_types(element).report(args.detailed).splitlines():
        name, _, value = line.removeprefix("[i] ").partition("=")
        table.add_row(name.strip(), value.strip())
    session.console.print(table)
    return counts


def _simulate_arguments(parser: Parser) -> None:
    store_flags(parser, "network", "gate_layout")
    parser.add_argument("--store", action="store_true", help="add the output functions to the truth table store")
    parser.add_argument("--silent", action="store_true", help="do not print the truth tables")


@command("simulate", Category.LOGIC, _simulate_arguments)
def simulate_command(session: Session, args: argparse.Namespace) -> Result:
    """Compute the truth table of every output of the active network or gate-level layout.

    Layouts are simulated on the logic level, following the clocking; timing is not considered.
    """
    store = one_store(args, "network", "gate_layout")
    element = session.networks.current() if store == "network" else session.gate_layouts.current()
    tables: list[dict[str, object]] = []
    for output, bits in simulate(element).items():
        binary = "".join("1" if bit else "0" for bit in bits)
        table = dynamic_truth_table(int(math.log2(len(binary))))
        table.create_from_binary_string(binary)
        if args.store:
            session.truth_tables.add(table)
        if not args.silent:
            session.info(f"{output}: {binary} (0x{table.to_hex()})")
        tables.append({"output": output, **describe(table)})
    return {"tables": tables}


def _random_arguments(parser: Parser) -> None:
    parser.add_argument("-n", "--inputs", type=int, required=True, help="number of primary inputs")
    parser.add_argument("-g", "--gates", type=int, required=True, help="number of AND gates")
    parser.add_argument("--seed", type=int, help="random seed; a fresh one is drawn when omitted")


@command("random", Category.LOGIC, _random_arguments)
def random_command(session: Session, args: argparse.Namespace) -> Result:
    """Generate a random AIG; the seed becomes its name."""
    seed = args.seed if args.seed is not None else secrets.randbelow(2**32)
    network = from_aigverse(session, random_aig(num_pis=args.inputs, num_gates=args.gates, seed=seed), str(seed))
    session.networks.add(network)
    return {"network": describe(network), "seed": seed}


def _generate_arguments(parser: Parser) -> None:
    parser.add_argument("kind", choices=list(GENERATORS), help="rca and cla are adders, multiplier, mux, decoder")
    parser.add_argument("-b", "--bitwidth", type=int, required=True, help="operand width, or select bits for decoder")


@command("generate", Category.LOGIC, _generate_arguments)
def generate(session: Session, args: argparse.Namespace) -> Result:
    """Generate an arithmetic or control circuit as an AIG."""
    network = from_aigverse(session, GENERATORS[args.kind](args.bitwidth), f"{args.kind}{args.bitwidth}")
    session.networks.add(network)
    return {"network": describe(network)}


def _aig_arguments(parser: Parser) -> None:
    parser.add_argument("passes", nargs="+", choices=list(AIG_PASSES), metavar="PASS", help=", ".join(AIG_PASSES))


@command("aig", Category.LOGIC, _aig_arguments)
def aig_command(session: Session, args: argparse.Namespace) -> Result:
    """Run optimization passes on the active AIG, in the given order.

    Passes: rewrite (cut rewriting), resub (resubstitution), refactor (SOP refactoring), balance
    (ESOP balancing), cleanup (remove dangling nodes). Only AIGs read with '--type aig' qualify.
    """
    aig = _active_aig(session)
    optimized = to_aigverse(session, aig)
    for name in args.passes:
        result = AIG_PASSES[name](optimized)
        if result is None:
            msg = f"pass '{name}' produced no network"
            raise CommandError(msg)
        optimized = result
    network = from_aigverse(session, optimized, get_name(aig), like=aig)
    session.networks.add(network)
    session.info(f"{aig.num_gates()} -> {network.num_gates()} gates")
    return {"network": describe(network), "passes": list(args.passes), "gates_before": aig.num_gates()}


def _abc_arguments(parser: Parser) -> None:
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("-c", "--commands", metavar="COMMANDS", help="a ';'-separated ABC command string")
    source.add_argument("-s", "--script", choices=ABC_SCRIPTS, help="a named ABC script")


@command("abc", Category.LOGIC, _abc_arguments)
def abc_command(session: Session, args: argparse.Namespace) -> Result:
    """Optimize the active AIG with an external ABC installation.

    ABC is found on PATH as 'abc' or through the AIGVERSE_ABC environment variable. The read and
    write steps are added around the given commands.
    """
    if not abc.is_available():
        msg = "ABC was not found; install it on PATH or point AIGVERSE_ABC at the binary"
        raise CommandError(msg)
    aig = _active_aig(session)
    original = to_aigverse(session, aig)
    if args.script is not None:
        optimized = getattr(abc, args.script)(original)
    else:
        optimized = abc.run_script(original, args.commands)
    network = from_aigverse(session, optimized, get_name(aig), like=aig)
    session.networks.add(network)
    session.info(f"{aig.num_gates()} -> {network.num_gates()} gates")
    return {"network": describe(network), "gates_before": aig.num_gates()}


def _active_aig(session: Session) -> aig_network:
    network = session.networks.current()
    if not isinstance(network, aig_network):
        kind = "technology network" if isinstance(network, technology_network) else type(network).__name__
        msg = f"the active network is a {kind}; read the file with '--type aig'"
        raise CommandError(msg)
    return network
