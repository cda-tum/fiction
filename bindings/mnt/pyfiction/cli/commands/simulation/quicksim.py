# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The quicksim command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    quicksim,
    quicksim_params,
)
from mnt.pyfiction.cli.parsing import positive_int, probability
from mnt.pyfiction.cli.registry import Category, command

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import _active_sidb_layout, _apply_physical, _physical_arguments, _store_result


def _quicksim_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    _physical_arguments(parser, base=False)
    parser.add_argument("-i", "--iterations", type=positive_int, default=80, help="iteration steps")
    parser.add_argument("-a", "--alpha", type=probability, default=0.7, help="alpha parameter")


@command(
    "quicksim",
    Category.SIMULATION,
    _quicksim_arguments,
    inputs="Active cell-level layout.",
    example="read layout.sqd; quicksim",
)
def quicksim_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout heuristically with QuickSim, approximating the ground state."""
    layout = _active_sidb_layout(session, unsimulated=True)
    params = quicksim_params()
    parameters = _apply_physical(params.simulation_parameters, args)
    params.iteration_steps = args.iterations
    params.alpha = args.alpha
    parameters.update(iterations=args.iterations, alpha=args.alpha)
    return _store_result(session, layout, quicksim(layout, params), parameters)
