# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.networks.io import read_technology_network
from mnt.pyfiction.verification import eq_type, equivalence_checking, equivalence_checking_stats

if TYPE_CHECKING:
    from pathlib import Path


def test_non_eq(resources_dir: Path) -> None:
    xor2_net = read_technology_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_technology_network(str(resources_dir / "xnor2.v"))

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
    left = read_technology_network(str(left_file))
    right = read_technology_network(str(right_file))
    extra = read_technology_network(str(extra_file))
    assert equivalence_checking(left, right) == eq_type.STRONG
    assert equivalence_checking(left, extra) == eq_type.NO
