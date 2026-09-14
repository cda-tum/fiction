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

from mnt.pyfiction import aig_network, get_name, technology_network
from mnt.pyfiction.cli.aigverse_bridge import from_aigverse, to_aigverse
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable

    from aigverse.networks import Aig

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


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
    "aig", Category.LOGIC, _aig_arguments, inputs="Active network.", example="generate mux -b 1; aig rewrite cleanup"
)
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


def _active_aig(session: Session) -> aig_network:
    """Require an AIG in the active network store."""
    network = session.networks.current()
    if not isinstance(network, aig_network):
        kind = "technology network" if isinstance(network, technology_network) else type(network).__name__
        msg = f"the active network is a {kind}; read the file with '--type aig'"
        raise CommandError(msg)
    return network
