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
    labels = {
        "size x": "Width",
        "size y": "Height",
        "size z": "Layers",
        "size area": "Grid area",
        "critical_path": "Critical path",
        "runtime_s": "Runtime (s)",
        "time_total_s": "Runtime (s)",
        "area_nm2": "Area (nm²)",
        "ground_state_energy_ev": "Ground state energy (eV)",
    }
    for key, value in flatten(description):
        prefix, _, leaf = key.rpartition(" ")
        label = labels.get(key) or (
            f"{prefix.capitalize()} {labels[leaf]}" if leaf in labels else key.replace("_", " ").capitalize()
        )
        if key == "throughput":
            display = f"1/{value}"
        elif value is None:
            display = "—"
        else:
            display = str(value)
        rendered.add_row(f"[bold]{label}[/]", escape(display))
    return rendered
