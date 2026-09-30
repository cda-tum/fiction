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
import shutil
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- runs the lazy-loading check in a fresh interpreter
import sys
from fnmatch import fnmatchcase

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


@pytest.mark.skipif(sys.platform != "linux", reason="Inspect ELF exports with GNU nm")
@pytest.mark.parametrize("name", SUBMODULES)
def test_extension_exports(name: str) -> None:
    """Extensions expose their initializer and nanobind's shared exception ABI.

    Args:
        name: Top-level extension module below ``mnt.pyfiction``.
    """
    nm = shutil.which("nm")
    assert nm is not None, "nm is required to check Linux extension exports"
    module = importlib.import_module(f"mnt.pyfiction.{name}")
    assert module.__file__ is not None
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- resolved nm and an imported extension path
        [nm, "-D", "--defined-only", "--format=just-symbols", "--demangle", module.__file__],
        check=True,
        capture_output=True,
        text=True,
    )
    exports = result.stdout.splitlines()
    entry_point = f"PyInit_{name}"
    assert entry_point in exports
    exceptions = ("python_error", "builtin_exception")
    unexpected = [
        symbol
        for symbol in exports
        if symbol != entry_point
        and not any(fnmatchcase(symbol, f"*nanobind::abi*::{exception}*") for exception in exceptions)
    ]
    assert not unexpected, f"Unexpected exports from {name}: {unexpected}"
    assert any(fnmatchcase(symbol, "typeinfo for nanobind::abi*::python_error") for symbol in exports)
    if name in {"sidb", "synthesis"}:
        assert any(fnmatchcase(symbol, "typeinfo for nanobind::abi*::builtin_exception") for symbol in exports)


def test_coordinate_namespace_in_fresh_interpreter() -> None:
    """Coordinate imports expose the type and utilities that layout APIs accept."""
    script = (
        "from mnt.pyfiction.layouts import area, cartesian_layout, coordinate, volume\n"
        "assert coordinate.__module__ == 'mnt.pyfiction.layouts'\n"
        "assert area(coordinate(2, 3, 1)) == 12\n"
        "assert volume(coordinate(2, 3, 1)) == 24\n"
        "assert cartesian_layout(coordinate(2, 3)).x() == 2\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script
