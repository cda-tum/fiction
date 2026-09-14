# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared command helpers."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING

from mnt.pyfiction import (
    cartesian_gate_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.render import table as render_table
from mnt.pyfiction.cli.statistics import stats_to_dict
from mnt.pyfiction.cli.stores import describe
from mnt.pyfiction.cli.topologies import TOPOLOGIES

if TYPE_CHECKING:
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
    from mnt.pyfiction.cli.stores import GateLayout


MILLISECONDS = 1000
"""The bindings take timeouts in milliseconds; the commands take seconds."""


def _added(session: Session, layout: GateLayout, stats: object, *, verbose: bool) -> Result:
    """Describe a layout a command just added, printing its statistics as the shared table when asked.

    Args:
        session: The session, for the console.
        layout: The layout that was added to the store.
        stats: The algorithm's statistics object.
        verbose: Print the statistics.

    Returns:
        The log entry of the command.
    """
    statistics = stats_to_dict(stats)
    if verbose:
        session.console.print(render_table(statistics))
    return {"gate_layout": describe(layout), "stats": statistics}


def _seconds_to_ms(seconds: float | None) -> int | None:
    """Convert a positive timeout to whole milliseconds without shortening it.

    Args:
        seconds: Timeout in seconds, or None for the native default.

    Returns:
        Timeout rounded up to milliseconds, or None.

    Raises:
        CommandError: If the timeout is non-finite, non-positive, or exceeds uint32.
    """
    if seconds is None:
        return None
    if not math.isfinite(seconds) or seconds <= 0 or seconds * MILLISECONDS > 2**32 - 1:
        msg = "the timeout must be positive"
        raise CommandError(msg)
    return math.ceil(seconds * MILLISECONDS)


def _cartesian_2ddwave(session: Session) -> GateLayout:
    """Require an active Cartesian layout with 2DDWave clocking."""
    layout = session.gate_layouts.current()
    if not isinstance(layout, cartesian_gate_layout):
        msg = f"the active layout is {TOPOLOGIES[type(layout)]}; a Cartesian layout is needed"
        raise CommandError(msg)
    if layout.get_clocking_scheme_name().upper() != "2DDWAVE":
        msg = f"the active layout is {layout.get_clocking_scheme_name()}-clocked; 2DDWave is needed"
        raise CommandError(msg)
    return layout
