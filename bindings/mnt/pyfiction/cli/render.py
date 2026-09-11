# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The one table renderer of the shell, so ``ps``, ``-v``, and ``check`` all look alike."""

from __future__ import annotations

from rich.markup import escape
from rich.table import Table


def flatten(description: dict[str, object], prefix: str = "") -> list[tuple[str, object]]:
    """Flatten a nested description into label and value pairs.

    Nested dictionaries contribute their keys under a space-separated prefix, so ``size.x`` reads
    as ``size x``.

    Args:
        description: A dictionary from :func:`~mnt.pyfiction.cli.stores.describe` or a statistics
            dictionary.
        prefix: What to put in front of every label of this level.

    Returns:
        The rows in the order the dictionary holds them.
    """
    rows: list[tuple[str, object]] = []
    for key, value in description.items():
        if isinstance(value, dict):
            rows.extend(flatten(value, f"{prefix}{key} "))
        else:
            rows.append((f"{prefix}{key}", value))
    return rows


def table(description: dict[str, object]) -> Table:
    """Render a description as the aligned two-column table the shell prints everywhere.

    Args:
        description: A dictionary from :func:`~mnt.pyfiction.cli.stores.describe` or a statistics
            dictionary.

    Returns:
        The table, ready to print on a console.
    """
    rendered = Table(box=None, show_header=False, padding=(0, 2))
    for key, value in flatten(description):
        rendered.add_row(f"[bold]{key}[/]", escape(str(value)))
    return rendered
