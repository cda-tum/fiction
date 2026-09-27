# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Runtime and static contracts of the generated native signatures."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import CartesianGateLayout
from mnt.pyfiction.layouts.coords import CubeCoordinate, OffsetCoordinate
from mnt.pyfiction.physical_design import PostLayoutOptimizationParams
from mnt.pyfiction.physical_design.routing import reserve_input_nodes
from mnt.pyfiction.sidb import SiDBLayout
from mnt.pyfiction.sidb.analysis import (
    BdlWire,
    CriticalTemperatureDomain,
    OperationalDomain,
    OperationalStatus,
    ParameterPoint,
)
from mnt.pyfiction.sidb.simulation import quickexact

if TYPE_CHECKING:
    from typing_extensions import assert_type

    from mnt.pyfiction.networks import TechnologyNetwork


def test_coordinate_input_and_output_types() -> None:
    """Tuple inputs convert to coordinates, while outputs retain their class."""
    coordinate = OffsetCoordinate(tuple_repr=(1, 2))
    assert OffsetCoordinate(c=coordinate) == coordinate
    cube = CubeCoordinate(tuple_repr=(1, 2, 3))
    assert CubeCoordinate(c=cube) == cube
    layout = CartesianGateLayout((2, 2))
    layout.resize((3, 3, 1))
    east = layout.east((0, 0))
    assert east == OffsetCoordinate(1, 0)
    source = layout.create_pi("a", (0, 0))
    output = layout.create_po(source, "y", (1, 0))
    assert output == east
    layout.name = "coordinates"
    assert layout.name == "coordinates"
    if TYPE_CHECKING:
        assert_type(source, OffsetCoordinate)
        assert_type(output, OffsetCoordinate)
        assert_type(east, OffsetCoordinate)


def test_optional_relocation_limit() -> None:
    """The relocation limit accepts an integer and can be cleared."""
    params = PostLayoutOptimizationParams()
    if TYPE_CHECKING:
        assert_type(params.max_gate_relocations, int | None)
    params.max_gate_relocations = 7
    assert params.max_gate_relocations == 7
    params.max_gate_relocations = None
    assert params.max_gate_relocations is None
    with pytest.raises(TypeError):
        params.max_gate_relocations = "not a count"  # ty: ignore[invalid-assignment]


def test_reserved_input_node_mapping(mux21: TechnologyNetwork) -> None:
    """Each primary input maps to a reserved layout node."""
    network = mux21
    layout = CartesianGateLayout()
    mapping = reserve_input_nodes(layout, network)
    if TYPE_CHECKING:
        assert_type(mapping, dict[int, int])
    assert set(mapping) == set(network.pis())
    assert len(set(mapping.values())) == network.num_pis()
    assert layout.num_pis() == network.num_pis()


def test_wire_port_direction() -> None:
    """Wire port direction and input/output flags round-trip through Python."""
    wire = BdlWire()
    port = BdlWire.port_direction(BdlWire.port_direction.cardinal.SOUTH, pi=True)
    wire.direction = port
    actual = wire.direction
    if TYPE_CHECKING:
        assert_type(actual, BdlWire.port_direction)
    assert actual.dir == BdlWire.port_direction.cardinal.SOUTH.value
    assert actual.pi
    assert not actual.po


def test_domain_iterator_types() -> None:
    """Both domains iterate over parameter-point keys."""
    point = ParameterPoint([1.0, 2.0])
    domain = OperationalDomain()
    domain[point] = OperationalStatus.OPERATIONAL
    actual = next(iter(domain))
    assert actual == point
    temperatures = CriticalTemperatureDomain()
    temperatures[point] = (OperationalStatus.OPERATIONAL, 10.0)
    temperature_point = next(iter(temperatures))
    assert temperature_point == point
    if TYPE_CHECKING:
        assert_type(actual, ParameterPoint)
        assert_type(temperature_point, ParameterPoint)


def test_additional_simulation_parameter_types() -> None:
    """Additional parameters expose string keys and supported scalar values."""
    result = quickexact(SiDBLayout())
    assert isinstance(result.additional_simulation_parameters, dict)
    if TYPE_CHECKING:
        assert_type(result.additional_simulation_parameters, dict[str, int | float | bool | str])
