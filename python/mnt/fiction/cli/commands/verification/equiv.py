# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The equiv command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command, store_flags
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.pyfiction import eq_type, equivalence_checking, equivalence_checking_stats, get_name

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _equiv_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser, "network", "gate_layout")


@command(
    "equiv",
    Category.VERIFICATION,
    _equiv_arguments,
    inputs="Store elements selected by the flags below.",
    example="generate mux -b 1; ortho; equiv -n -g",
    progress=True,
)
def equiv(session: Session, args: argparse.Namespace) -> Result:
    """Check the active gate-level layout against the active network, or two store elements against each other.

    With -n and -g, the network is the specification and the layout the implementation. With only
    -n or only -g, the two most recent elements of that store are compared. Strong equivalence
    means the same function at full throughput; weak equivalence means the same function with a
    delay, reported in clock cycles.
    """
    if args.network and args.gate_layout:
        specification = session.as_technology_network(session.networks.current())
        implementation = session.gate_layouts.current()
    elif args.network:
        specification, implementation = (session.as_technology_network(n) for n in _last_two(session, "network"))
    elif args.gate_layout:
        specification, implementation = _last_two(session, "gate_layout")
    else:
        msg = "select the stores to compare: -n, -g, or both"
        raise CommandError(msg)

    stats = equivalence_checking_stats()
    result = equivalence_checking(specification, implementation, stats)
    spec_name, impl_name = get_name(specification), get_name(implementation)
    blocked = [
        operand
        for operand, report in (("specification", stats.spec_drv_stats), ("implementation", stats.impl_drv_stats))
        if report.drvs
    ]
    if blocked:
        reason = f"design rule violations in {' and '.join(blocked)}"
        session.output(f"{spec_name} and {impl_name}: not checked ({reason})")
        return {
            "specification": spec_name,
            "implementation": impl_name,
            **stats_to_dict(stats),
            "eq": "NOT_CHECKED",
            "reason": reason,
        }
    if result == eq_type.NO:
        session.output(f"{spec_name} and {impl_name} are not equivalent")
    elif result == eq_type.WEAK:
        session.output(
            f"{spec_name} and {impl_name} are weakly equivalent with a delay of {stats.tp_diff} clock cycles"
        )
    else:
        session.output(f"{spec_name} and {impl_name} are strongly equivalent")
    return {"specification": spec_name, "implementation": impl_name, **stats_to_dict(stats)}


def _last_two(session: Session, store: str) -> tuple[object, object]:
    """Add the command's arguments to the parser."""
    items = session.networks.items if store == "network" else session.gate_layouts.items
    if len(items) < 2:  # ruff: ignore[magic-value-comparison] -- two elements are what a comparison needs
        msg = f"comparing two {store.replace('_', ' ')}s needs two of them in store"
        raise CommandError(msg)
    return items[-2], items[-1]
