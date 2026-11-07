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

from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.verification import eq_type, equivalence_checking, equivalence_checking_stats

if TYPE_CHECKING:
    from pathlib import Path


def test_non_eq(resources_dir: Path) -> None:
    xor2_net = read_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_network(str(resources_dir / "xnor2.v"))

    stats = equivalence_checking_stats()
    assert stats.counter_example == []

    eq = equivalence_checking(xor2_net, xnor2_net, stats)
    assert eq == eq_type.NO
    assert stats.counter_example == [True, False]


def test_equivalence_matches_named_interfaces(tmp_path: Path) -> None:
    left_file = tmp_path / "left.blif"
    right_file = tmp_path / "right.blif"
    extra_file = tmp_path / "extra.blif"
    logic = ".names a b compare\n10 1\n.names a pass\n1 1\n.end\n"
    left_file.write_text(".model left\n.inputs a b\n.outputs compare pass\n" + logic)
    right_file.write_text(".model right\n.inputs b a\n.outputs pass compare\n" + logic)
    extra_file.write_text(".model extra\n.inputs b a unused\n.outputs pass compare\n" + logic)
    left = read_network(str(left_file))
    right = read_network(str(right_file))
    extra = read_network(str(extra_file))
    assert equivalence_checking(left, right) == eq_type.STRONG
    assert equivalence_checking(left, extra) == eq_type.NO


def test_input_mapping_ties_duplicated_inputs(tmp_path: Path) -> None:
    """An explicit mapping lets an implementation read one specification input through several inputs."""
    spec_file = tmp_path / "spec.blif"
    impl_file = tmp_path / "impl.blif"
    spec_file.write_text(".model spec\n.inputs a b\n.outputs f\n.names a b f\n11 1\n.end\n")
    # x & (y | z) computes a & b once y and z both read b
    impl_file.write_text(".model impl\n.inputs x y z\n.outputs f\n.names x y z f\n11- 1\n1-1 1\n.end\n")
    spec = read_network(str(spec_file))
    impl = read_network(str(impl_file))
    assert equivalence_checking(spec, impl) == eq_type.NO
    assert equivalence_checking(spec, impl, input_mapping=[0, 1, 1]) == eq_type.STRONG
    stats = equivalence_checking_stats()
    assert equivalence_checking(spec, impl, stats, input_mapping=[0, 0, 1]) == eq_type.NO
    assert stats.counter_example
    with pytest.raises(ValueError, match="input mapping"):
        equivalence_checking(spec, impl, input_mapping=[0, 1])
    with pytest.raises(ValueError, match="input mapping"):
        equivalence_checking(spec, impl, input_mapping=[0, 1, 2])
