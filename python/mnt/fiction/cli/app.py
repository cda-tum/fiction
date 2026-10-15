# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Route fiction commands without loading unrelated interfaces."""

from __future__ import annotations

import argparse
import sys
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Sequence


def main(argv: Sequence[str] | None = None) -> int:
    """Dispatch a dedicated workflow or the backwards-compatible shell.

    Args:
        argv: Arguments without the executable name; None reads process arguments.

    Returns:
        The selected interface's exit status.
    """
    args = list(sys.argv[1:] if argv is None else argv)
    if args and args[0] == "opdom":
        from mnt.fiction.opdom import main as opdom_main  # ruff: ignore[import-outside-top-level] -- load the selected interface

        return opdom_main(args[1:])
    if args in (["-h"], ["--help"]):
        parser = argparse.ArgumentParser(
            prog="fiction",
            description="Design automation for field-coupled nanotechnologies.",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""Routes:
  fiction opdom INPUT.sqd --gate xor --csv domain.csv
      Compute an operational domain from explicit inputs.
  fiction repl
      Start the full interactive shell (also the default without a command).

Run 'fiction opdom --help' or 'fiction repl --help' for route-specific options.
Legacy shell options remain valid: -c COMMANDS, -f FILE, -i, --log FILE, --quiet,
and --version. Piped input runs shell commands without an interactive prompt.
Put the route before its options.""",
        )
        parser.print_help()
        return 0
    if args and args[0] == "repl":
        args = args[1:]
    from .repl import main as repl_main  # ruff: ignore[import-outside-top-level] -- load the shell only on its route

    return repl_main(args)
