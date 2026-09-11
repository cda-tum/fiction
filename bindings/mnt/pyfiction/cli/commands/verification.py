# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Verification commands: equivalence checking and design rule checking."""

from __future__ import annotations

import json
from typing import TYPE_CHECKING

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    equivalence_checking_stats,
    gate_level_drv_stats,
    gate_level_drvs,
    get_name,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command, store_flags
from mnt.pyfiction.cli.render import table as render_table
from mnt.pyfiction.cli.session import stats_to_dict

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


def _equiv_arguments(parser: Parser) -> None:
    store_flags(parser, "network", "gate_layout")


@command("equiv", Category.VERIFICATION, _equiv_arguments)
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
    if result == eq_type.NO:
        session.info(f"{spec_name} and {impl_name} are not equivalent")
    elif result == eq_type.WEAK:
        session.info(f"{spec_name} and {impl_name} are weakly equivalent with a delay of {stats.tp_diff} clock cycles")
    else:
        session.info(f"{spec_name} and {impl_name} are strongly equivalent")
    return {"specification": spec_name, "implementation": impl_name, **stats_to_dict(stats)}


def _last_two(session: Session, store: str) -> tuple[object, object]:
    items = session.networks.items if store == "network" else session.gate_layouts.items
    if len(items) < 2:  # ruff: ignore[magic-value-comparison] -- two elements are what a comparison needs
        msg = f"comparing two {store.replace('_', ' ')}s needs two of them in store"
        raise CommandError(msg)
    return items[-2], items[-1]


@command("check", Category.VERIFICATION)
def check(session: Session, args: argparse.Namespace) -> Result:
    """Check the active gate-level layout for design rule violations and print the full report."""
    del args
    layout = session.gate_layouts.current()
    stats = gate_level_drv_stats()
    gate_level_drvs(layout, statistics=stats)
    report: dict[str, object] = json.loads(stats.report)
    session.console.print(render_table(report))
    return report
