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

from mnt.pyfiction.layouts import cartesian_gate_layout, cartesian_layout, stacked_cartesian_layout
from mnt.pyfiction.layouts.coords import cube_coordinate, offset_coordinate
from mnt.pyfiction.physical_design import post_layout_optimization_params, reserve_input_nodes
from mnt.pyfiction.sidb.simulation import sidb_simulation_result
from mnt.pyfiction.sidb.simulation.logic import (
    bdl_wire,
    critical_temperature_domain,
    operational_domain,
    operational_status,
    parameter_point,
)

if TYPE_CHECKING:
    from typing_extensions import assert_type

    from mnt.pyfiction.networks import technology_network


def test_coordinate_input_and_output_types() -> None:
    """Tuple inputs convert to coordinates, while outputs retain their class."""
    coordinate = offset_coordinate(tuple_repr=(1, 2))
    assert offset_coordinate(c=coordinate) == coordinate
    cube = cube_coordinate(tuple_repr=(1, 2, 3))
    assert cube_coordinate(c=cube) == cube
    layout = cartesian_layout((2, 2))
    layout.resize((3, 3, 1))
    east = layout.east((0, 0))
    assert east == offset_coordinate(1, 0)
    stacked = stacked_cartesian_layout((2, 2, 3))
    above = stacked.above((0, 0, 0))
    assert above == cube_coordinate(0, 0, 1)
    if TYPE_CHECKING:
        assert_type(east, offset_coordinate)
        assert_type(above, cube_coordinate)


def test_optional_relocation_limit() -> None:
    """The relocation limit accepts an integer and can be cleared."""
    params = post_layout_optimization_params()
    if TYPE_CHECKING:
        assert_type(params.max_gate_relocations, int | None)
    params.max_gate_relocations = 7
    assert params.max_gate_relocations == 7
    params.max_gate_relocations = None
    assert params.max_gate_relocations is None
    with pytest.raises(TypeError):
        params.max_gate_relocations = "not a count"  # type: ignore[assignment]


def test_reserved_input_node_mapping(mux21: technology_network) -> None:
    """Each primary input maps to a reserved layout node."""
    network = mux21
    layout = cartesian_gate_layout()
    mapping = reserve_input_nodes(layout, network)
    if TYPE_CHECKING:
        assert_type(mapping, dict[int, int])
    assert set(mapping) == set(network.pis())
    assert len(set(mapping.values())) == network.num_pis()
    assert layout.num_pis() == network.num_pis()


def test_wire_port_direction() -> None:
    """Wire port direction and input/output flags round-trip through Python."""
    wire = bdl_wire()
    port = bdl_wire.port_direction(bdl_wire.port_direction.cardinal.SOUTH, pi=True)
    wire.direction = port
    actual = wire.direction
    if TYPE_CHECKING:
        assert_type(actual, bdl_wire.port_direction)
    assert actual.dir == bdl_wire.port_direction.cardinal.SOUTH.value
    assert actual.pi
    assert not actual.po


def test_domain_iterator_types() -> None:
    """Both domains iterate over parameter-point keys."""
    point = parameter_point([1.0, 2.0])
    domain = operational_domain()
    domain[point] = operational_status.OPERATIONAL
    actual = next(iter(domain))
    assert actual == point
    temperatures = critical_temperature_domain()
    temperatures[point] = (operational_status.OPERATIONAL, 10.0)
    temperature_point = next(iter(temperatures))
    assert temperature_point == point
    if TYPE_CHECKING:
        assert_type(actual, parameter_point)
        assert_type(temperature_point, parameter_point)


def test_additional_simulation_parameter_types() -> None:
    """Additional parameters expose string keys and supported scalar values."""
    result = sidb_simulation_result()
    assert result.additional_simulation_parameters == {}
    if TYPE_CHECKING:
        assert_type(result.additional_simulation_parameters, dict[str, int | float | bool | str])
