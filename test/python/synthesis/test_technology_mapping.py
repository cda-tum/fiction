# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import (
    aig_network,
    get_name,
    mig_network,
    set_name,
    simulate_outputs,
    technology_network,
    xag_network,
)
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import (
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    convert_network,
    missing_required_gates_exception,
    network_target,
    technology_mapping,
    technology_mapping_stats,
)
from mnt.pyfiction.verification import count_gate_types, eq_type, equivalence_checking

if TYPE_CHECKING:
    from collections.abc import Callable, Mapping
    from pathlib import Path

    from mnt.pyfiction.synthesis import TechnologyMappingOptions


@pytest.mark.parametrize(
    ("source_gate", "expression", "expected"),
    [
        ("lt2", "a & ~b", [False, False, True, False]),
        ("gt2", "~a & b", [False, True, False, False]),
        ("le2", "~(~a & b)", [True, False, True, True]),
        ("ge2", "~(a & ~b)", [True, True, False, True]),
    ],
)
@pytest.mark.parametrize("gate", ["lt2", "gt2", "le2", "ge2"])
def test_mapping_respects_asymmetric_gate_selection(
    tmp_path: Path, source_gate: str, gate: str, expression: str, expected: list[bool]
) -> None:
    """The mapped network uses the selected gate and preserves the truth table."""
    path = tmp_path / f"{source_gate}.v"
    path.write_text(
        f"module top(a,b,f);\ninput a,b;\noutput f;\nassign f = {expression};\nendmodule\n", encoding="utf-8"
    )
    network = read_network(str(path))
    options: dict[str, Any] = {"inv": True, gate: True}
    mapped = technology_mapping(network, **options)
    counts = count_gate_types(mapped)
    assert getattr(counts, f"num_{gate}") == 1
    assert all(getattr(counts, f"num_{other}") == 0 for other in ("lt2", "gt2", "le2", "ge2") if other != gate)
    assert simulate_outputs(network) == [("f", expected)]
    assert simulate_outputs(mapped) == [("f", expected)]


TWO_INPUT = {"and2", "nand2", "or2", "nor2", "xor2", "xnor2"}
THREE_INPUT = {"and3", "xor_and", "or_and", "onehot", "maj3", "gamble", "dot", "mux", "and_xor"}
GATE_LIBRARIES = [
    pytest.param(and_or_not, {"inv", "and2", "or2"}, id="and_or_not"),
    pytest.param(and_or_not_maj, {"inv", "and2", "or2", "maj3"}, id="and_or_not_maj"),
    pytest.param(all_standard_2_input_functions, {"inv"} | TWO_INPUT, id="all_standard_2_input_functions"),
    pytest.param(all_standard_3_input_functions, {"inv"} | THREE_INPUT, id="all_standard_3_input_functions"),
    pytest.param(
        all_supported_standard_functions, {"inv"} | TWO_INPUT | THREE_INPUT, id="all_supported_standard_functions"
    ),
]


def _check_library(network: technology_network, options: Mapping[str, Any]) -> None:
    """Recognized gates belong to the selected library; permuted DOT/MUX may classify as other."""
    counts = count_gate_types(network)
    for gate, enabled in options.items():
        if gate != "decay" and not enabled:
            assert getattr(counts, f"num_{gate}") == 0, gate


def test_missing_gate_exception_export() -> None:
    """Users can catch the exception exposed by the technology mapper."""
    assert issubclass(missing_required_gates_exception, RuntimeError)


def test_mapping_default(mux21: technology_network) -> None:
    """An empty gate library is rejected before native mapping."""
    with pytest.raises(missing_required_gates_exception, match="missing required gates"):
        technology_mapping(mux21)


def test_mapping_without_statistics(mux21: technology_network) -> None:
    mapped = technology_mapping(mux21, **and_or_not())
    assert simulate_outputs(mapped) == [("out", [True, True, False, False, True, False, True, False])]


@pytest.mark.parametrize(("make_options", "enabled"), GATE_LIBRARIES)
def test_mapping_with_gate_libraries(
    mux21: technology_network, make_options: Callable[[], TechnologyMappingOptions], enabled: set[str]
) -> None:
    options = make_options()
    assert set(options) == {"decay", "inv", "lt2", "gt2", "le2", "ge2"} | TWO_INPUT | THREE_INPUT
    assert all(type(value) is bool for value in options.values())
    assert {key for key, value in options.items() if value} == enabled
    stats = technology_mapping_stats()
    mapped = technology_mapping(mux21, stats=stats, **options)
    assert not stats.mapper_stats.mapping_error
    assert equivalence_checking(mux21, mapped) == eq_type.STRONG
    assert simulate_outputs(mapped) == [("out", [True, True, False, False, True, False, True, False])]
    _check_library(mapped, options)
    options["inv"] = False
    assert make_options()["inv"] is True


@pytest.mark.parametrize(
    ("target", "required"),
    [
        (network_target.AIG, {"inv", "and2"}),
        (network_target.XAG, {"inv", "and2", "xor2"}),
        (network_target.MIG, {"inv", "maj3"}),
    ],
)
def test_base_network_requirements(
    interface_network: technology_network, target: network_target, required: set[str]
) -> None:
    """Each native source type requires its basis gates, even if its current graph omits one."""
    network = convert_network(interface_network, target)
    for missing in required:
        options: dict[str, Any] = dict.fromkeys(required, True)
        options[missing] = False
        with pytest.raises(missing_required_gates_exception):
            technology_mapping(network, **options)


@pytest.mark.parametrize("cls", [aig_network, xag_network, mig_network])
def test_mapping_accepts_every_network_type(
    resources_dir: Path, cls: type[aig_network | xag_network | mig_network]
) -> None:
    network = read_network(str(resources_dir / "mux21.v"), network_type=cls)
    options = all_supported_standard_functions()
    options["lt2"] = True
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, stats=stats, **options)
    assert not stats.mapper_stats.mapping_error
    assert simulate_outputs(mapped) == [("out", [True, True, False, False, True, False, True, False])]
    _check_library(mapped, options)


@pytest.mark.parametrize(
    ("gate", "bits"),
    [
        ("and2", "1000"),
        ("nand2", "0111"),
        ("or2", "1110"),
        ("nor2", "0001"),
        ("xor2", "0110"),
        ("xnor2", "1001"),
        ("and3", "10000000"),
        ("xor_and", "00101000"),
        ("or_and", "10101000"),
        ("onehot", "00010110"),
        ("maj3", "11101000"),
        ("gamble", "10000001"),
        ("dot", "01010010"),
        ("mux", "11011000"),
        ("and_xor", "01101010"),
    ],
)
def test_selected_standard_gates(blif_network: Callable[[str], technology_network], gate: str, bits: str) -> None:
    inputs = "a b" if len(bits) == 4 else "a b c"
    arity = len(inputs.split())
    cubes = "".join(f"{i:0{arity}b}"[::-1] + " 1\n" for i, bit in enumerate(reversed(bits)) if bit == "1")
    network = blif_network(f".model gate\n.inputs {inputs}\n.outputs f\n.names {inputs} f\n{cubes}.end\n")
    options = dict.fromkeys(all_supported_standard_functions(), False)
    options.update(inv=True)
    options[gate] = True
    if gate in {"xor2", "xnor2"}:
        options["and2"] = True
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, stats=stats, **options)
    assert not stats.mapper_stats.mapping_error
    assert simulate_outputs(mapped) == [("f", [bit == "1" for bit in bits])]
    _check_library(mapped, options)


@pytest.mark.parametrize(("selected", "success"), [("lt2", True), ("le2", False)])
def test_single_comparison_without_inverter(
    blif_network: Callable[[str], technology_network], selected: str, *, success: bool
) -> None:
    network = blif_network(".model lt\n.inputs a b\n.outputs f\n.names a b f\n10 1\n.end\n")
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, stats=stats, **{selected: True})
    assert stats.mapper_stats.mapping_error is not success
    if success:
        assert simulate_outputs(mapped) == [("f", [False, False, True, False])]
    else:
        assert mapped.num_pos() == 0
    assert simulate_outputs(network) == [("f", [False, False, True, False])]


@pytest.mark.parametrize("cubes", ["01 1\n10 1\n", "11- 1\n1-1 1\n-11 1\n"])
def test_unmatched_source_gate_reports_mapping_failure(
    blif_network: Callable[[str], technology_network], cubes: str
) -> None:
    inputs = "a b" if cubes.startswith("01") else "a b c"
    network = blif_network(f".model unsupported\n.inputs {inputs}\n.outputs f\n.names {inputs} f\n{cubes}.end\n")
    before = simulate_outputs(network)
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, inv=True, and2=True, stats=stats)
    assert stats.mapper_stats.mapping_error
    assert mapped.num_pos() == 0
    assert simulate_outputs(network) == before


def test_mapping_preserves_interfaces(
    interface_network: technology_network, interface_outputs: list[tuple[str, list[bool]]]
) -> None:
    set_name(interface_network, "interfaces")
    options = all_supported_standard_functions()
    stats = technology_mapping_stats()
    mapped = technology_mapping(interface_network, stats=stats, **options)
    assert not stats.mapper_stats.mapping_error
    assert get_name(mapped) == "interfaces"
    assert [mapped.get_name(pi) for pi in mapped.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(mapped) == interface_outputs
    _check_library(mapped, options)


def test_decay_uses_constant_fanins(mux21: technology_network) -> None:
    options = all_standard_3_input_functions()
    options["decay"] = True
    stats = technology_mapping_stats()
    mapped = technology_mapping(mux21, stats=stats, **options)
    assert not stats.mapper_stats.mapping_error
    assert simulate_outputs(mapped) == [("out", [True, True, False, False, True, False, True, False])]
    for gate in mapped.gates():
        if len(mapped.fanins(gate)) == 3:
            assert any(mapped.is_constant(n) for n in mapped.fanins(gate))


@pytest.mark.parametrize("inv", [False, True])
def test_mapping_preserves_separate_constants(blif_network: Callable[[str], technology_network], *, inv: bool) -> None:
    network = blif_network(
        ".model constants\n.inputs a b\n.outputs zero one and_true and_false or_true or_false\n"
        ".names zero\n.names one\n1\n.names a one and_true\n11 1\n.names a zero and_false\n11 1\n"
        ".names b one or_true\n1- 1\n-1 1\n.names b zero or_false\n1- 1\n-1 1\n.end\n"
    )
    expected = [
        (name, [bit == "1" for bit in bits])
        for name, bits in [
            ("zero", "0000"),
            ("one", "1111"),
            ("and_true", "1010"),
            ("and_false", "0000"),
            ("or_true", "1111"),
            ("or_false", "1100"),
        ]
    ]
    assert simulate_outputs(network) == expected
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, inv=inv, and2=True, or2=True, stats=stats)
    assert not stats.mapper_stats.mapping_error
    assert simulate_outputs(mapped) == expected
    assert simulate_outputs(network) == expected
