# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Package exports for builds with optional native solvers."""

from __future__ import annotations

import importlib.util
from pathlib import Path

import pytest

from mnt import pyfiction
from mnt.pyfiction import pyfiction as native


@pytest.mark.parametrize("solver", ["z3", "alglib"])
def test_optional_solver_exports(solver: str, monkeypatch: pytest.MonkeyPatch) -> None:
    """A package without one solver keeps the other exports and omits unavailable names."""
    unavailable = (
        [
            "exact_cartesian",
            "exact_even_column_cartesian",
            "exact_even_column_hex",
            "exact_even_row_cartesian",
            "exact_hexagonal",
            "exact_odd_column_hex",
            "exact_odd_row_cartesian",
            "exact_odd_row_hex",
            "exact_params",
            "exact_shifted_cartesian",
            "exact_stats",
            "technology_constraints",
        ]
        if solver == "z3"
        else ["clustercomplete", "clustercomplete_params", "ground_state_space_reporting"]
    )
    for name in unavailable:
        monkeypatch.delattr(native, name, raising=False)

    source = Path(pyfiction.__file__)
    spec = importlib.util.spec_from_file_location("mnt.pyfiction", source)
    assert spec is not None
    assert spec.loader is not None
    package = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(package)

    assert package.technology_network is native.technology_network
    assert package.dynamic_truth_table is native.dynamic_truth_table
    assert package.exact_sidb_simulation_engine is native.exact_sidb_simulation_engine
    assert all(not hasattr(package, name) and name not in package.__all__ for name in unavailable)
    for name in ("exact_params", "clustercomplete"):
        assert hasattr(package, name) == hasattr(native, name)
        assert (name in package.__all__) == hasattr(native, name)
