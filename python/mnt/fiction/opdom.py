# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Compute and plot an operational domain from an SQD gate layout."""

from __future__ import annotations

import time
from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction.sidb.io import read_sqd_layout

from .cli.commands.io._common import _existing_file
from .cli.commands.simulation.opdom import domain_arguments, run_domain
from .cli.errors import HelpRequested
from .cli.opdom_plotting import validate_outputs
from .cli.parsing import Parser
from .cli.session import Session
from .cli.statistics import json_value
from .cli.stores import CellEntry

if TYPE_CHECKING:
    import argparse
    from collections.abc import Sequence


def main(argv: Sequence[str] | None = None) -> int:
    """Run the installed command or module entry point.

    Args:
        argv: Arguments without the executable name; None reads the process arguments.

    Returns:
        Zero on success or help, one on a computation or output failure.
    """
    parser = Parser(
        "fiction-opdom",
        __doc__ or "Operational domain computation.",
        inputs="One SQD gate layout and an explicit logic specification.",
        example="fiction-opdom gate.sqd --gate xor --plot domain.png --plot domain.html",
        unavailable=None,
    )
    parser.add_argument("input", type=Path, help="SQD gate layout")
    parser.add_argument("--csv", dest="file", type=Path, metavar="FILE", help="write a CSV domain")
    parser.add_argument("--log", type=Path, metavar="FILE", help="write JSON statistics")
    parser.add_argument("--quiet", action="store_true", help="suppress progress and notices; retain results and errors")
    domain_arguments(parser, require_spec=True)
    session = Session()
    started = time.perf_counter()
    entry: dict[str, object] = {"command": "opdom", "args": {}, "status": "error"}
    status = 0
    try:
        args = parser.parse_args(argv)
        _configure_session(session, args, entry)
        entry["result"] = _run(session, args)
        entry["status"] = "ok"
    except HelpRequested as request:
        session.console.print(request.text, markup=False, highlight=False)
    except (Exception, KeyboardInterrupt) as error:  # ruff: ignore[blind-except] -- native failures return a CLI exit status
        message = "interrupted" if isinstance(error, KeyboardInterrupt) else str(error)
        session.error(message)
        entry["error"] = message
        status = 1
    finally:
        entry["runtime_s"] = time.perf_counter() - started
        session.close()
    return status or int(session.close_failed)


def _configure_session(session: Session, args: argparse.Namespace, entry: dict[str, object]) -> None:
    """Configure output and logging for one invocation.

    Args:
        session: The command session.
        args: Parsed options.
        entry: The log entry.
    """
    if args.file is None and not args.plot:
        args.plot = [Path(f"{args.input.stem}_opdom.png")]
    validate_outputs(args, log_path=args.log)
    session.log_path = args.log
    session.quiet = args.quiet
    session.log.append(entry)
    entry["args"] = json_value(vars(args))


def _run(session: Session, args: argparse.Namespace) -> dict[str, object] | None:
    """Load one SQD layout and run shared domain orchestration.

    Args:
        session: Supplies the stores and progress display.
        args: SQD input and computation options.

    Returns:
        Statistics and output paths for the log.
    """
    _existing_file(args.input, (".sqd",))
    session.cell_layouts.add(CellEntry(read_sqd_layout(str(args.input), args.input.stem)))
    with session.progress("opdom"):
        return run_domain(session, args)


if __name__ == "__main__":
    raise SystemExit(main())
