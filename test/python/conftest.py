# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Fixtures and directory markers shared across the pyfiction test suite."""

from __future__ import annotations

import shutil
from itertools import starmap
from pathlib import Path
from typing import TYPE_CHECKING

import pytest

from mnt import pyfiction

if TYPE_CHECKING:
    from mnt.pyfiction.layouts import cartesian_gate_layout
    from mnt.pyfiction.networks import technology_network

RESOURCES_DIR = Path(__file__).resolve().parent / "resources"
pytest_plugins = ["pytester"]


@pytest.hookimpl(tryfirst=True)
def pytest_collection_modifyitems(items: list[pytest.Item]) -> None:
    """Apply ancestor-directory markers before pytest evaluates marker expressions.

    Args:
        items: Collected tests, including tests outside this suite.
    """
    test_dir = Path(__file__).resolve().parent
    marked_dirs = {
        "layouts",
        "networks",
        "physical_design",
        "synthesis",
        "verification",
        "fcn",
        "inml",
        "qca",
        "mol_qca",
        "sidb",
        "utils",
        "cli",
        "simulation",
        "io",
        "generators",
    }
    for item in items:
        if item.path.is_relative_to(test_dir):
            for directory in item.path.relative_to(test_dir).parent.parts:
                if directory in marked_dirs:
                    item.add_marker(getattr(pytest.mark, directory))


SYNTHETIC_INPUTS = {
    "FA.v": """\
module top(x, y, cin, s, cout);
input x, y, cin;
output s, cout;

wire w1, w2, w3;

assign w1 = x & y;
assign w2 = x ^ y;
assign w3 = cin & w2;
assign cout = w1 | w3;
assign s = cin ^ w2;

endmodule

""",
    "mux21.v": """\
module top(in0, in1, in2, out);
input in0, in1, in2;
output out;

wire w1, w2, w3;

assign w1 = ~in2;
assign w2 = in0 & w1;
assign w3 = in1 & in2;
assign out = w2 | w3;

endmodule

""",
    "xor2.v": """\
module top(in0, in1, out);
input a, b;
output out;

wire w1, w2, w3;

assign w1 = a & b;
assign w2 = a | b;
assign w3 = ~w1;
assign out = w2 & w3;

endmodule

""",
    "xnor2.v": """\
module top(a, b, out);
input a, b;
output out;
wire w1, w2, w3, w4, w5;

assign w1 = ~b;
assign w2 = a & w1;
assign w3 = ~a;
assign w4 = w3 & b;
assign w5 = w2 | w4;
assign out = ~w5;

endmodule

""",
    "network_interfaces.blif": """\
.model interfaces
.inputs apple banana cherry unused
.outputs zero one inverted xor majority lut copy
.names zero
.names one
1
.names apple inverted
0 1
.names apple banana xor
01 1
10 1
.names apple banana cherry majority
11- 1
1-1 1
-11 1
.names apple banana cherry lut
001 1
010 1
111 1
.names apple copy
1 1
.end

""",
}
"""Small network and layout files used by reader and writer tests."""


@pytest.fixture(scope="session")
def resources_dir(tmp_path_factory: pytest.TempPathFactory) -> Path:
    """Materialize synthetic inputs beside copies of the SiDB reference layouts.

    Args:
        tmp_path_factory: The worker's temporary-directory factory.

    Returns:
        The directory holding this worker's input files.
    """
    directory = tmp_path_factory.mktemp("inputs")
    for name, content in SYNTHETIC_INPUTS.items():
        (directory / name).write_text(content, encoding="utf-8")
    for source in RESOURCES_DIR.glob("*.sqd"):
        shutil.copyfile(source, directory / source.name)
    return directory


@pytest.fixture
def mux21(resources_dir: Path) -> technology_network:
    """A 2:1 multiplexer, the network most physical design tests place and route.

    Read once per test rather than once per session, because callers pass it to algorithms
    that modify it in place.

    Returns:
        The ``mux21.v`` network as a ``technology_network``.
    """
    return pyfiction.networks.io.read_network(str(resources_dir / "mux21.v"))


def _parity_verilog() -> str:
    """The 16-input parity tree of the C++ blueprints as Verilog.

    Pairs of signals are combined by an XNOR built from AND and NOT gates, the root by an XOR built from AND, NOT
    and OR gates; the hybrid strategy keeps crossings on this network where pure duplication copies whole levels.

    Returns:
        Verilog source of the network with inputs ``pi00`` to ``pi15`` and output ``po0``.
    """
    inputs = [f"pi{i:02d}" for i in range(16)]
    wires: list[str] = []
    assigns: list[str] = []

    def xnor(x: str, y: str) -> str:
        a, b, c = (f"n{len(wires) + k}" for k in range(3))
        wires.extend((a, b, c))
        assigns.extend((f"assign {a} = {x} & ~{y};", f"assign {b} = ~{x} & {y};", f"assign {c} = ~{a} & ~{b};"))
        return c

    def tree(pairs: list[tuple[str, str]]) -> str:
        # the blueprint pairs inputs from the highest index down and combines neighbouring pairs
        signals = list(starmap(xnor, pairs))
        while len(signals) > 1:
            signals = [xnor(signals[i], signals[i + 1]) for i in range(0, len(signals), 2)]
        return signals[0]

    pairs = [(inputs[i], inputs[i + 1]) for i in range(14, -1, -2)]
    left = tree(pairs[:4])
    right = tree(pairs[4:])
    wires.extend(("n_left", "n_right"))
    assigns.extend((f"assign n_left = {left} & ~{right};", f"assign n_right = ~{left} & {right};"))

    return (
        f"module top({', '.join(inputs)}, po0);\n"
        f"input {', '.join(inputs)};\n"
        "output po0;\n"
        f"wire {', '.join(wires)};\n" + "\n".join(assigns) + "\nassign po0 = n_left | n_right;\nendmodule\n"
    )


@pytest.fixture
def full_adder(resources_dir: Path) -> technology_network:
    """A full adder, whose planarization duplicates inputs.

    Returns:
        The ``FA.v`` network as a ``technology_network``.
    """
    return pyfiction.networks.io.read_network(str(resources_dir / "FA.v"))


@pytest.fixture
def parity_network(tmp_path: Path) -> technology_network:
    """Read the sixteen-input parity blueprint.

    Returns:
        A network whose embedding exercises wider ranks.
    """
    path = tmp_path / "parity.v"
    path.write_text(_parity_verilog(), encoding="utf-8")
    return pyfiction.networks.io.read_network(str(path))


@pytest.fixture
def interface_network(resources_dir: Path) -> technology_network:
    """A network with constants, inverted edges, LUTs, labels, and an unused input.

    Returns:
        The network used to check conversion and file round trips.
    """
    return pyfiction.networks.io.read_network(str(resources_dir / "network_interfaces.blif"))


@pytest.fixture
def mux21_layout(mux21: technology_network) -> cartesian_gate_layout:
    """The ``mux21`` network placed and routed with ``orthogonal``.

    Returns:
        A 2DDWave-clocked Cartesian gate-level layout.
    """
    return pyfiction.physical_design.orthogonal(mux21)
