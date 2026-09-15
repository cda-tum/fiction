# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The abc command."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from aigverse import abc

from mnt import pyfiction
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.stores import describe
from mnt.pyfiction import aig_network, convert_network, get_name, network_target, set_name

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


ABC_SCRIPTS = ("resyn", "resyn2", "resyn3", "compress", "compress2", "resyn2rs", "compress2rs", "dc2")
"""The named ABC scripts ``aigverse.abc`` provides."""


def _abc_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("--no-read", action="store_true", help="let the custom flow provide its input")
    parser.add_argument("--no-strash", action="store_true", help="omit the initial strash command")
    parser.add_argument("--no-write", action="store_true", help="leave the network store unchanged")
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("-c", "--commands", metavar="COMMANDS", help="a ';'-separated ABC command string")
    source.add_argument("-s", "--script", choices=ABC_SCRIPTS, help="a named ABC script")


@command("abc", Category.LOGIC, _abc_arguments, inputs="Active network.", example="generate mux -b 1; abc -c strash")
def abc_command(session: Session, args: argparse.Namespace) -> Result:
    """Optimize the active AIG or XAG with an external ABC installation.

    ABC is found on PATH as 'abc' or through the AIGVERSE_ABC environment variable. The read and
    write steps are added around the given commands.
    """
    if not abc.is_available():
        msg = "ABC was not found; install it on PATH or point AIGVERSE_ABC at the binary"
        raise CommandError(msg)
    network = None if args.no_read else session.networks.current()
    if network is not None and not isinstance(network, (aig_network, pyfiction.xag_network)):
        msg_0 = "ABC requires an AIG or XAG; use read --type aig or --type xag"
        raise CommandError(msg_0)
    aig = convert_network(network, network_target.AIG) if isinstance(network, pyfiction.xag_network) else network
    input_path = session.temp_file(".aig")
    output_path = session.temp_file(".aig")
    flow: list[str] = []
    try:
        if aig is not None:
            pyfiction.write_aiger(aig, str(input_path))
            flow.append(f'read_aiger "{input_path.as_posix()}"')
        if not args.no_strash:
            flow.append("strash")
        flow.extend(abc.SCRIPTS[args.script] if args.script else [args.commands])
        if not args.no_write:
            flow.append(f'write_aiger -s "{output_path.as_posix()}"')
        output = abc.run_commands("; ".join(flow), cwd=Path.cwd())
        if args.no_write:
            session.output(output.rstrip())
            return {"replacement": False, "output": output}
        if not output_path.is_file() or not output_path.stat().st_size:
            msg_0 = "ABC produced no output network; the store is unchanged"
            raise CommandError(msg_0)
        result = pyfiction.read_aig_network(str(output_path))
        if aig is not None:
            set_name(result, get_name(aig))
        session.networks.add(result)
        return {"network": describe(result), "output": output}
    finally:
        input_path.unlink(missing_ok=True)
        output_path.unlink(missing_ok=True)
