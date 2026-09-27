# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.physical_design import (
    HexagonalizationIoPinExtensionMode,
    HexagonalizationParams,
    hexagonalization,
    orthogonal,
)
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking


def test_hexagonalization_default(mux21):
    cart_layout = orthogonal(mux21).layout
    assert equivalence_checking(mux21, cart_layout).eq == EquivalenceType.STRONG
    hex_layout = hexagonalization(cart_layout).layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG


def test_hexagonalization_with_parameters(mux21):
    cart_layout = orthogonal(mux21).layout
    assert equivalence_checking(mux21, cart_layout).eq == EquivalenceType.STRONG
    params = HexagonalizationParams()
    hex_layout = hexagonalization(cart_layout, params=params).layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG


def test_hexagonalization_with_stats(mux21):
    cart_layout = orthogonal(mux21).layout
    assert equivalence_checking(mux21, cart_layout).eq == EquivalenceType.STRONG
    result = hexagonalization(cart_layout)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0


def test_hexagonalization_with_stats_and_parameters(mux21):
    cart_layout = orthogonal(mux21).layout
    assert equivalence_checking(mux21, cart_layout).eq == EquivalenceType.STRONG

    params = HexagonalizationParams()
    params.input_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.NONE
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0

    params.input_pin_extension = HexagonalizationIoPinExtensionMode.NONE
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND_PLANAR
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.NONE
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0

    params.input_pin_extension = HexagonalizationIoPinExtensionMode.NONE
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND_PLANAR
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND_PLANAR
    params.output_pin_extension = HexagonalizationIoPinExtensionMode.EXTEND_PLANAR
    result = hexagonalization(cart_layout, params=params)
    stats = result.stats
    hex_layout = result.layout
    assert equivalence_checking(mux21, hex_layout).eq == EquivalenceType.STRONG
    assert equivalence_checking(cart_layout, hex_layout).eq == EquivalenceType.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()
