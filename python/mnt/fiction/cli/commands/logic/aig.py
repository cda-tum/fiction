# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The aig command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from aigverse.algorithms import aig_cut_rewriting, aig_resubstitution, balancing, cleanup_dangling, sop_refactoring

from mnt.fiction.cli.aigverse_bridge import from_aigverse, to_aigverse
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.stores import describe, size_and_depth
from mnt.pyfiction import aig_network, get_name, technology_network

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable

    from aigverse.networks import Aig

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


AIG_PASSES: dict[str, Callable[[Aig], Aig | None]] = {
    "rewrite": aig_cut_rewriting,
    "resub": aig_resubstitution,
    "refactor": sop_refactoring,
    "balance": balancing,
    "cleanup": cleanup_dangling,
}
"""The ``aig`` passes and the ``aigverse`` algorithms that run them."""


def _aig_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("passes", nargs="+", choices=list(AIG_PASSES), metavar="PASS", help=", ".join(AIG_PASSES))


@command(
    "aig",
    Category.LOGIC,
    _aig_arguments,
    inputs="Active network.",
    example="generate mux -b 1; aig rewrite cleanup",
    progress=True,
)
def aig_command(session: Session, args: argparse.Namespace) -> Result:
    """Run optimization passes on the active AIG, in the given order.

    Passes: rewrite (cut rewriting), resub (resubstitution), refactor (SOP refactoring), balance
    (SOP balancing, which minimizes depth at the cost of gates), cleanup (remove dangling nodes).
    Only AIGs read with '--type aig' qualify.
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
    session.info(size_and_depth(aig, network))
    return {
        "network": describe(network),
        "passes": list(args.passes),
        "gates_before": aig.num_gates(),
        "depth_before": aig.depth(),
    }


def _active_aig(session: Session) -> aig_network:
    """Require an AIG in the active network store."""
    network = session.networks.current()
    if not isinstance(network, aig_network):
        kind = "technology network" if isinstance(network, technology_network) else type(network).__name__
        msg = f"the active network is a {kind}; read the file with '--type aig'"
        raise CommandError(msg)
    return network
