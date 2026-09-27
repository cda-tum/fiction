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
    "physical_design.routing",
    "qca.io",
    "sidb.generators",
    "sidb.io",
    "sidb.simulation",
    "sidb.simulation.analysis",
    "sidb.simulation.defects",
    "sidb.simulation.io",
    "sidb.simulation.logic",
]


def test_root_exposes_only_submodules() -> None:
    """The package root lists the submodules and the version, and no bound names."""
    assert sorted(pyfiction.__all__) == sorted(["__version__", *SUBMODULES])
    assert set(SUBMODULES) <= set(dir(pyfiction))
    assert not hasattr(pyfiction, "CartesianGateLayout")


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
        "assert pf.sidb.simulation.quickexact\n"
        "assert 'mnt.pyfiction.sidb' in sys.modules\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script


def test_coordinate_namespace_in_fresh_interpreter() -> None:
    """Immutable coordinate keys and tuple inputs work on domain layouts."""
    script = (
        "from mnt.pyfiction.layouts import CartesianGateLayout\n"
        "from mnt.pyfiction.layouts.coords import OffsetCoordinate, CubeCoordinate\n"
        "for coordinate in (OffsetCoordinate, CubeCoordinate):\n"
        "    assert {coordinate(1, 2): 3}[coordinate(1, 2)] == 3\n"
        "assert CartesianGateLayout((2, 3)).x() == 2\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script


def test_public_types_share_native_identity() -> None:
    """Public objects pass between separately compiled native modules."""
    native_layouts = importlib.import_module("mnt.pyfiction._native.layouts")
    assert pyfiction.layouts.CartesianGateLayout is native_layouts.CartesianGateLayout
    layout = pyfiction.layouts.CartesianGateLayout((2, 2))
    layout.create_pi("a", (0, 0))
    assert pyfiction.verification.count_gate_types(layout) is not None


@pytest.mark.parametrize("name", SUBMODULES + NESTED_SUBMODULES)
def test_public_exports_are_explicit(name: str) -> None:
    """Every declared public export exists and excludes native module objects.

    Args:
        name: Public module path.
    """
    module = importlib.import_module(f"mnt.pyfiction.{name}")
    assert all(hasattr(module, member) for member in module.__all__)
    assert "_native" not in module.__all__


def test_orthogonal_options_without_exact_solver() -> None:
    """An optional exact solver does not control the orthogonal clock-phase enum."""
    script = (
        "import importlib\n"
        "native = importlib.import_module('mnt.pyfiction._native.physical_design')\n"
        "for name in ('ExactParams', 'ExactStats', 'TechnologyConstraints', 'exact_cartesian'):\n"
        "    if hasattr(native, name): delattr(native, name)\n"
        "from mnt.pyfiction import physical_design as pd\n"
        "from mnt.pyfiction.networks import TechnologyNetwork\n"
        "assert not pd.exact_available()\n"
        "assert pd.OrthogonalParams().number_of_clock_phases == pd.ClockPhases.FOUR\n"
        "try:\n"
        "    pd.exact(TechnologyNetwork())\n"
        "except RuntimeError as error:\n"
        "    assert 'Z3' in str(error)\n"
        "else:\n"
        "    raise AssertionError('missing solver must be reported')\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script


def test_simulation_without_clustercomplete() -> None:
    """Basic SiDB engines remain available when ALGLIB is absent."""
    script = (
        "import importlib\n"
        "native = importlib.import_module('mnt.pyfiction._native.sidb.simulation.engines')\n"
        "for name in ('ClusterCompleteParams', 'GroundStateSpaceReporting', 'clustercomplete'):\n"
        "    if hasattr(native, name): delattr(native, name)\n"
        "from mnt.pyfiction.sidb import SiDBLayout, simulation\n"
        "assert not hasattr(simulation, 'clustercomplete')\n"
        "assert simulation.quickexact(SiDBLayout()).ground_states() == []\n"
        "assert simulation.quicksim\n"
        "assert simulation.exhaustive_ground_state_simulation\n"
    )
    subprocess.run([sys.executable, "-c", script], check=True)  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and script
