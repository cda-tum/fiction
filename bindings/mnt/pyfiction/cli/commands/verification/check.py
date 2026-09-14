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

from mnt.pyfiction import (
    gate_level_drv_stats,
    gate_level_drvs,
)
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.render import table as render_table

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


@command("check", Category.VERIFICATION, inputs="Active gate-level layout.", example="generate mux -b 1; ortho; check")
def check(session: Session, args: argparse.Namespace) -> Result:
    """Check the active gate-level layout for design rule violations and print the full report."""
    del args
    layout = session.gate_layouts.current()
    stats = gate_level_drv_stats()
    gate_level_drvs(layout, statistics=stats)
    report: dict[str, object] = json.loads(stats.report)
    session.output(f"{stats.drvs} violations, {stats.warnings} warnings")
    issues = {key: value for key, value in report.items() if value}
    if (stats.drvs or stats.warnings) and issues:
        session.console.print(render_table(issues))
    else:
        session.output("No design rule violations or warnings.")
    return report
