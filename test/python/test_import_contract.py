# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Pin the public module structure of ``mnt.pyfiction``."""

from __future__ import annotations

import importlib
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- runs the lazy-loading check in a fresh interpreter
import sys

import pytest

from mnt import pyfiction

SUBMODULES = [
    "fcn",
    "inml",
    "layouts",
    "mol_qca",
    "networks",
    "physical_design",
    "qca",
    "sidb",
    "synthesis",
    "utils",
    "verification",
]

NESTED_SUBMODULES = [
    "fcn.io",
    "inml.io",
    "layouts.coords",
    "layouts.io",
    "mol_qca.io",
    "networks.io",
    "physical_design.path_finding",
    "qca.io",
    "sidb.generators",
    "sidb.io",
    "sidb.model",
    "sidb.simulation",
    "sidb.simulation.analysis",
    "sidb.simulation.defects",
    "sidb.simulation.engines",
    "sidb.simulation.io",
    "sidb.simulation.logic",
]


def test_root_exposes_only_submodules() -> None:
    """The package root lists the submodules and the version, and no bound names."""
    assert sorted(pyfiction.__all__) == sorted(["__version__", *SUBMODULES])
    assert set(SUBMODULES) <= set(dir(pyfiction))
    assert not hasattr(pyfiction, "cartesian_layout")


@pytest.mark.parametrize("name", SUBMODULES + NESTED_SUBMODULES)
def test_submodule_resolves_through_import(name: str) -> None:
    """Every submodule, nested ones included, resolves through the import system.

    Args:
        name: Submodule path below ``mnt.pyfiction``.
    """
    module = importlib.import_module(f"mnt.pyfiction.{name}")

    parent = pyfiction
    for part in name.split("."):
        parent = getattr(parent, part)
    assert parent is module


def test_submodules_load_lazily() -> None:
    """Importing the package loads no submodule until the first attribute access."""
    script = (
        "import sys\n"
        "import mnt.pyfiction as pf\n"
        "assert 'mnt.pyfiction.sidb' not in sys.modules\n"
        "assert pf.sidb.simulation.engines.quickexact\n"
        "assert 'mnt.pyfiction.sidb' in sys.modules\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script


def test_coordinate_namespace_in_fresh_interpreter() -> None:
    """Coordinate imports expose types and utilities that layout APIs accept."""
    script = (
        "from mnt.pyfiction.layouts import cartesian_layout\n"
        "from mnt.pyfiction.layouts.coords import (\n"
        "    offset_coordinate, cube_coordinate, offset_area, cube_area, offset_volume, cube_volume\n"
        ")\n"
        "for coordinate, area, volume in (\n"
        "    (offset_coordinate, offset_area, offset_volume),\n"
        "    (cube_coordinate, cube_area, cube_volume),\n"
        "):\n"
        "    assert coordinate.__module__ == 'mnt.pyfiction.layouts.coords'\n"
        "    assert area(coordinate(2, 3, 1)) == 12\n"
        "    assert volume(coordinate(2, 3, 1)) == 24\n"
        "assert cartesian_layout(offset_coordinate(2, 3)).x() == 2\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script
