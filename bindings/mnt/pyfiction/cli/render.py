# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Render grouped store descriptions and tables of algorithm statistics."""

from __future__ import annotations

from rich.console import Group as RenderGroup
from rich.markup import escape
from rich.table import Table

GROUPS: tuple[tuple[str, tuple[str, ...]], ...] = (
    ("Size", ("size x", "size y", "size z", "size area")),
    ("I/O", ("inputs", "outputs")),
    ("Elements", ("gates", "wires", "crossings", "synchronization_elements", "cells", "dots")),
    ("Timing", ("critical_path", "throughput")),
)
"""The ``ps`` sections and the description keys each one folds onto a single line."""

HEADINGS: tuple[str, ...] = ("name", "type", "technology", "topology", "lattice", "clocking")
"""Identity keys that head a ``ps`` block instead of taking a row of their own."""

UNITS: dict[str, str] = {
    "size area": "tiles",
    "inputs": "in",
    "outputs": "out",
    "gates": "gates",
    "wires": "wires",
    "crossings": "crossings",
    "synchronization_elements": "sync",
    "cells": "cells",
    "dots": "dots",
    "critical_path": "critical path",
}
"""What each grouped figure is called on its shared line."""


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
    for label, display in table_rows(description):
        rendered.add_row(f"[bold]{label}[/]", escape(display))
    return rendered


def table_rows(description: dict[str, object]) -> list[tuple[str, str]]:
    """Return the label and displayed value of every figure of a description.

    Args:
        description: A description, or the leftover figures of one.

    Returns:
        Label and value pairs in the order the description holds them.
    """
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
    rows: list[tuple[str, str]] = []
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
        rows.append((label, display))
    return rows


def summary(description: dict[str, object]) -> RenderGroup:
    """Render a description as the grouped block ``ps`` prints.

    Related figures share one line under a short heading, so a gate-level layout reads in four rows
    rather than fourteen, and no column is stretched by the longest label.

    Args:
        description: A dictionary from :func:`~mnt.pyfiction.cli.stores.describe`.

    Returns:
        The block, ready to print on a console.
    """
    rows = dict(flatten(description))
    rendered = Table(box=None, show_header=False, padding=(0, 2))

    # the heading stands outside the table, so a long name cannot widen the label column
    heading = " · ".join(str(rows[key]) for key in HEADINGS if rows.get(key) is not None)

    grouped: set[str] = set()
    for title, keys in GROUPS:
        present = [key for key in keys if rows.get(key) is not None]
        if not present:
            continue
        grouped.update(present)
        if title == "Size":
            extent = " x ".join(str(rows[key]) for key in present if key != "size area")
            area = rows.get("size area")
            value = f"{extent} ({area} tiles)" if extent and area is not None else extent or f"{area} tiles"
        elif title == "I/O":
            value = " / ".join(f"{rows[key]} {UNITS[key]}" for key in present)
        elif title == "Timing":
            pieces = [
                f"critical path {rows['critical_path']}" if "critical_path" in present else "",
                f"throughput 1/{rows['throughput']}" if "throughput" in present else "",
            ]
            value = " · ".join(piece for piece in pieces if piece)
        else:
            value = " · ".join(f"{rows[key]} {UNITS[key]}" for key in present)
        rendered.add_row(f"[bold]{title}[/]", escape(value))

    leftover = {key: value for key, value in rows.items() if key not in grouped and key not in HEADINGS}
    if leftover:
        for label, value in table_rows(leftover):
            rendered.add_row(f"[bold]{label}[/]", escape(value))
    return RenderGroup(f"  [bold]{escape(heading)}[/]", rendered) if heading else RenderGroup(rendered)
