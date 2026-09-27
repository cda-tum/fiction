# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking


def test_non_eq(resources_dir):
    xor2_net = read_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_network(str(resources_dir / "xnor2.v"))

    result = equivalence_checking(xor2_net, xnor2_net)
    assert result.eq == EquivalenceType.NO
    assert result.counter_example == [True, False]
