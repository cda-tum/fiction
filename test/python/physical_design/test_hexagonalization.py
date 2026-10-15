# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

from mnt.pyfiction.physical_design import (
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_stats,
    orthogonal,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


def test_hexagonalization_default(mux21: technology_network) -> None:
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    hex_layout = hexagonalization(cart_layout)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG


def test_hexagonalization_with_parameters(mux21: technology_network) -> None:
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    hex_layout = hexagonalization(cart_layout)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG


def test_hexagonalization_with_stats(mux21: technology_network) -> None:
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0


def test_hexagonalization_with_stats_and_parameters(mux21: technology_network) -> None:
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG

    options: dict[str, Any] = {
        "input_pin_extension": hexagonalization_io_pin_extension_mode.EXTEND,
        "output_pin_extension": hexagonalization_io_pin_extension_mode.NONE,
    }
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert hex_layout.get_tile(pi).y == 0

    options["input_pin_extension"] = hexagonalization_io_pin_extension_mode.NONE
    options["output_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert hex_layout.get_tile(po).y == hex_layout.height() - 1

    options["input_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND
    options["output_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert hex_layout.get_tile(pi).y == 0
    for po in hex_layout.pos():
        assert hex_layout.get_tile(po).y == hex_layout.height() - 1

    options["input_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    options["output_pin_extension"] = hexagonalization_io_pin_extension_mode.NONE
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert hex_layout.get_tile(pi).y == 0

    options["input_pin_extension"] = hexagonalization_io_pin_extension_mode.NONE
    options["output_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert hex_layout.get_tile(po).y == hex_layout.height() - 1

    options["input_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    options["output_pin_extension"] = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats, **options)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert hex_layout.get_tile(pi).y == 0
    for po in hex_layout.pos():
        assert hex_layout.get_tile(po).y == hex_layout.height() - 1
