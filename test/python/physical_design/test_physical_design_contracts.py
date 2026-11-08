# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import physical_design
from mnt.pyfiction.networks import high_degree_fanin_exception, simulate_outputs, technology_network
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import graph_oriented_layout_design, orthogonal

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from mnt.pyfiction.layouts import cartesian_gate_layout


@pytest.mark.parametrize("design", [orthogonal, graph_oriented_layout_design], ids=["orthogonal", "gold"])
@pytest.mark.parametrize(
    ("inputs", "outputs", "body", "expected"),
    [
        ("", "zero one", ".names zero\n.names one\n1\n", [("zero", [False]), ("one", [True])]),
        (
            "input",
            "copy one zero",
            ".names input copy\n1 1\n.names one\n1\n.names zero\n",
            [("copy", [True, False]), ("one", [True, True]), ("zero", [False, False])],
        ),
        (
            "unused a b",
            "less",
            ".names b a less\n01 1\n",
            [("less", [False, False, False, False, True, True, False, False])],
        ),
        (
            "a",
            "copy repeated",
            ".names a copy\n1 1\n.names a a repeated\n11 1\n",
            [("copy", [True, False]), ("repeated", [True, False])],
        ),
        (
            "a b c",
            "sum carry",
            ".names a b c sum\n001 1\n010 1\n100 1\n111 1\n.names a b c carry\n11- 1\n1-1 1\n-11 1\n",
            None,
        ),
    ],
    ids=[
        "constant-outputs",
        "mixed-output-order",
        "unused-input-and-ordered-comparison",
        "repeated-input-and-direct-output",
        "high-fanin",
    ],
)
def test_small_network_design_contracts(
    tmp_path: Path,
    design: Callable[..., cartesian_gate_layout | None],
    inputs: str,
    outputs: str,
    body: str,
    expected: list[tuple[str, list[bool]]] | None,
) -> None:
    """Place distinct network boundary shapes with an independent Boolean oracle."""
    source = tmp_path / "boundary.blif"
    input_line = f".inputs {inputs}\n" if inputs else ""
    source.write_text(f".model boundary\n{input_line}.outputs {outputs}\n{body}.end\n")
    network = read_network(str(source))
    options = {} if design is orthogonal else {"return_first": True, "enable_multithreading": False, "timeout": 10000}
    if expected is None:
        with pytest.raises(high_degree_fanin_exception):
            design(network, **options)
        return
    result = design(network, **options)
    assert result is not None
    assert result.get_layout_name() == "boundary"
    assert [result.get_input_name(i) for i in range(result.num_pis())] == inputs.split()
    assert [result.get_output_name(i) for i in range(result.num_pos())] == outputs.split()
    assert simulate_outputs(result) == expected
    assert all(
        result.is_within_bounds(result.get_tile(node))
        for node in [*result.pis(), *result.pos(), *result.gates(), *result.wires()]
    )


def test_gold_empty_network_and_missing_custom_cost() -> None:
    """Keep an empty result empty and reject a missing CUSTOM objective."""
    empty = graph_oriented_layout_design(technology_network(), return_first=True, timeout=10000)
    assert empty is not None
    assert empty.is_empty()
    assert empty.area() == 0
    with pytest.raises(ValueError, match="custom"):
        graph_oriented_layout_design(technology_network(), cost=physical_design.gold_cost_objective.CUSTOM)


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_rejects_high_fanin_in_twoddwave(tmp_path: Path) -> None:
    """A majority network cannot use a clocking scheme with only two incoming neighbors."""
    source = tmp_path / "majority.blif"
    source.write_text(".model majority\n.inputs a b c\n.outputs f\n.names a b c f\n11- 1\n1-1 1\n-11 1\n.end\n")
    with pytest.raises(high_degree_fanin_exception):
        physical_design.exact(read_network(str(source)), scheme="2DDWave", timeout=1000)
