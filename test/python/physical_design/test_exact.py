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

from mnt.pyfiction import physical_design
from mnt.pyfiction.layouts import arrangement, hexagonal_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_default(mux21):
    layout = physical_design.exact(mux21)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_with_parameters(mux21):

    layout = physical_design.exact(mux21, border_io=True, crossings=True, scheme="ESR")
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_cartesian_with_stats(mux21):
    stats = physical_design.exact_stats()

    layout = physical_design.exact(mux21, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_default(mux21):

    layout = physical_design.exact(mux21, layout_type=hexagonal_gate_layout, layout_arrangement=arrangement.EVEN_ROW)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_with_parameters(mux21):

    layout = physical_design.exact(
        mux21,
        layout_type=hexagonal_gate_layout,
        border_io=True,
        crossings=True,
        scheme="ESR",
        layout_arrangement=arrangement.EVEN_ROW,
    )
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_hexagonal_with_stats(mux21):
    stats = physical_design.exact_stats()

    layout = physical_design.exact(
        mux21, layout_type=hexagonal_gate_layout, statistics=stats, layout_arrangement=arrangement.EVEN_ROW
    )
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.parametrize("cls", [shifted_cartesian_gate_layout, hexagonal_gate_layout])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_requires_an_arrangement_for_shifted_and_hexagonal_layouts(mux21, cls):
    with pytest.raises(ValueError, match="arrangement"):
        physical_design.exact(mux21, layout_type=cls)


@pytest.mark.parametrize("cls", [shifted_cartesian_gate_layout, hexagonal_gate_layout])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_selects_layout_class(
    mux21: technology_network, cls: type[shifted_cartesian_gate_layout | hexagonal_gate_layout]
) -> None:
    """Preserve the selected representation and explicit arrangement."""
    layout = physical_design.exact(mux21, layout_type=cls, layout_arrangement=arrangement.EVEN_ROW, timeout=10000)
    assert layout is not None
    assert type(layout) is cls
    assert isinstance(layout, (shifted_cartesian_gate_layout, hexagonal_gate_layout))
    assert layout.get_arrangement() == arrangement.EVEN_ROW
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


@pytest.mark.parametrize("selector", [None, "cartesian", int])
@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_rejects_invalid_selectors(mux21: technology_network, selector: Any) -> None:
    """Reject unrelated output selectors before solver work."""
    with pytest.raises(TypeError):
        physical_design.exact(mux21, layout_type=selector)


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_exact_unsatisfiable_result(mux21: technology_network) -> None:
    """A fixed one-tile layout cannot contain a multiplexer."""
    assert physical_design.exact(mux21, fixed_size=True, upper_bound_x=1, upper_bound_y=1, timeout=1000) is None


@pytest.mark.parametrize(
    "name", ["exact_cartesian", "exact_shifted_cartesian", "exact_hexagonal", "orthogonal_hexagonal"]
)
def test_physical_design_specialization_names_are_removed(name: str) -> None:
    """Expose one name per physical design algorithm."""
    assert not hasattr(physical_design, name)
