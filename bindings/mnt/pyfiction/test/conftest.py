"""Fixtures shared across the pyfiction test suite."""

from __future__ import annotations

from pathlib import Path

import pytest

from mnt.pyfiction import read_technology_network, technology_network

RESOURCES_DIR = Path(__file__).resolve().parent / "resources"


@pytest.fixture
def resources_dir() -> Path:
    """Directory holding the Verilog and SQD input files the tests read.

    Returns:
        Absolute path to the shared ``resources`` directory.
    """
    return RESOURCES_DIR


@pytest.fixture
def mux21() -> technology_network:
    """A 2:1 multiplexer, the network most physical design tests place and route.

    Read once per test rather than once per session, because callers pass it to algorithms
    that modify it in place.

    Returns:
        The ``mux21.v`` network as a ``technology_network``.
    """
    return read_technology_network(str(RESOURCES_DIR / "mux21.v"))
