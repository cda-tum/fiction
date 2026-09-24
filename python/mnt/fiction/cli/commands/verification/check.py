# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The check command."""

from __future__ import annotations

import json
from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.render import table as render_table
from mnt.pyfiction import gate_level_drv_params, gate_level_drv_stats, gate_level_drvs

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


@command(
    "check",
    Category.VERIFICATION,
    inputs="Active gate-level layout.",
    example="generate mux -b 1; ortho; check",
    progress=True,
)
def check(session: Session, args: argparse.Namespace) -> Result:
    """Check the active gate-level layout and print violations and warnings."""
    del args
    layout = session.gate_layouts.current()
    stats = gate_level_drv_stats()
    params = gate_level_drv_params()
    params.on_progress = session.report_progress
    gate_level_drvs(layout, params, statistics=stats)
    report: dict[str, object] = json.loads(stats.report)
    session.output(f"{stats.drvs} violations, {stats.warnings} warnings")
    issues = {key: value for key, value in report.items() if value}
    if (stats.drvs or stats.warnings) and issues:
        session.console.print(render_table(issues))
    else:
        session.output("No design rule violations or warnings.")
    return report
