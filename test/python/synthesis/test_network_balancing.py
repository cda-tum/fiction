# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.synthesis import NetworkBalancingParams, is_balanced, network_balancing

if TYPE_CHECKING:
    from mnt.pyfiction.networks import TechnologyNetwork


def test_balancing(mux21: TechnologyNetwork) -> None:
    """Balancing aligns paths in a network whose output is driven directly by a gate."""
    assert mux21.num_gates() == 4

    assert not is_balanced(mux21)

    balanced_default = network_balancing(mux21)
    assert balanced_default.num_gates() == 6
    assert is_balanced(balanced_default)

    unify_params = NetworkBalancingParams()
    unify_params.unify_outputs = True
    balanced_unify = network_balancing(mux21, params=unify_params)
    assert balanced_unify.num_gates() == 6
    assert is_balanced(balanced_unify, params=unify_params)
