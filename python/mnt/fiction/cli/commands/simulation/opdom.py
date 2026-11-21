# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Operational domains of the shell's active SiDB layout."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from mnt.fiction.cli.opdom import domain_arguments, run_domain
from mnt.fiction.cli.opdom_plotting import validate_outputs
from mnt.fiction.cli.registry import Category, command

from ._common import _active_sidb_layout

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _opdom_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("file", type=Path, help="the CSV file to write the domain to")
    domain_arguments(parser)


@command(
    "opdom",
    Category.SIMULATION,
    _opdom_arguments,
    inputs="Active SiDB layout; gate checks also use the active truth table.",
    example='read and.sqd; tt -e "(ab)"; opdom domain.csv',
    progress=True,
)
def opdom(session: Session, args: argparse.Namespace) -> Result:
    """Compute the operational domain of the active SiDB gate and export CSV or plots.

    The domain is the set of parameter points where the gate implements the expected functions.
    Grid search is the default; random sampling, flood fill, and contour tracing start from N
    random samples. The x and y axes sweep epsilon_r and lambda_tf by default; -z adds a third.
    """
    validate_outputs(args, log_path=session.log_path)
    layout = _active_sidb_layout(session)
    fallback = None if args.gate or args.table or args.expression else [session.truth_tables.current()]
    return run_domain(layout, session, args, fallback)
