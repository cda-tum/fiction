# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Filename validation and results shared by the file writers."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING, TypeVar

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.stores import element_name

if TYPE_CHECKING:
    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.fiction.cli.stores import CellLayout

T = TypeVar("T")


def output_argument(parser: Parser) -> None:
    """Add an optional output filename.

    Args:
        parser: The command parser.
    """
    parser.add_argument(
        "file", type=Path, nargs="?", help="output file; defaults to the active element's name and format suffix"
    )


def output_path(element: object, file: Path | None, suffix: str) -> Path:
    """Choose a filename without changing the destination.

    Args:
        element: The element to write.
        file: An explicit filename, or None to use the element's name.
        suffix: The format's canonical suffix.

    Returns:
        The output path.

    Raises:
        CommandError: The element has no usable name or the path is a directory.
    """
    if file is None:
        name = element_name(element)
        if not name:
            msg = "the active element has no name; give a file to write to"
            raise CommandError(msg)
        if any(char in name for char in ("/", "\\", ":")) or name in {".", ".."}:
            msg = "the element name contains path components; give an explicit output path"
            raise CommandError(msg)
        file = Path(f"{name}{suffix}")
    if file.is_dir():
        msg = f"'{file}' is a directory; give a file to write to"
        raise CommandError(msg)
    return file


def require_cell_type(layout: CellLayout, types: tuple[type[T], ...], suffix: str) -> T:
    """Require a cell layout supported by the format.

    Args:
        layout: The layout to write.
        types: Supported layout types.
        suffix: The output format.

    Returns:
        The layout, narrowed to the supported types.

    Raises:
        CommandError: The layout type is unsupported.
    """
    if not isinstance(layout, types):
        names = " or ".join(t.__name__ for t in types)
        msg = f"'{suffix}' files take a {names}, not a {type(layout).__name__}"
        raise CommandError(msg)
    return layout


def written(session: Session, path: Path) -> Result:
    """Report a completed file write.

    Args:
        session: The active shell session.
        path: The written file.

    Returns:
        The output filename for the session log.
    """
    session.info(f"wrote {path}")
    return {"file": str(path)}
