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
from pathlib import Path

import pytest

from mnt.pyfiction import cartesian_gate_layout, orthogonal, read_technology_network, technology_network

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
        "sidb",
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
    return read_technology_network(str(resources_dir / "mux21.v"))


@pytest.fixture
def interface_network(resources_dir: Path) -> technology_network:
    """A network with constants, inverted edges, LUTs, labels, and an unused input.

    Returns:
        The network used to check conversion and file round trips.
    """
    return read_technology_network(str(resources_dir / "network_interfaces.blif"))


@pytest.fixture
def mux21_layout(mux21: technology_network) -> cartesian_gate_layout:
    """The ``mux21`` network placed and routed with ``orthogonal``.

    Returns:
        A 2DDWave-clocked Cartesian gate-level layout.
    """
    return orthogonal(mux21)
