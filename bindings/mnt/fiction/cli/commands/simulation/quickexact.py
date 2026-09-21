# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The quickexact command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import quickexact, quickexact_params

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _active_sidb_layout, _apply_physical, _physical_arguments, _store_result


def _quickexact_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    _physical_arguments(parser, base=False, potential=True)


@command(
    "quickexact",
    Category.SIMULATION,
    _quickexact_arguments,
    inputs="Active cell-level layout.",
    example="read layout.sqd; quickexact",
    progress=True,
)
def quickexact_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout exactly with QuickExact, finding every physically valid charge distribution.

    The number of charge states per SiDB (2 or 3) is detected automatically.
    """
    layout = _active_sidb_layout(session)
    params = quickexact_params()
    params.on_progress = session.report_progress
    parameters = _apply_physical(params.simulation_parameters, args)
    params.global_potential = args.global_potential
    parameters["global_potential"] = args.global_potential
    return _store_result(session, layout, quickexact(layout, params), parameters)
