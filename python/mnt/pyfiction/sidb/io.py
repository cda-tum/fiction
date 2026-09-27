# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of SiDB layouts."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING

from mnt.pyfiction._native.sidb import io as _native
from mnt.pyfiction._native.sidb.simulation import io as _simulation_io

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.sidb import ChargeDistribution, SiDBLayout
    from mnt.pyfiction.sidb.analysis import CriticalTemperatureDomain, OperationalDomain
    from mnt.pyfiction.sidb.simulation import SimulationResult

from mnt.pyfiction._native.sidb.io import (
    ColorMode,
    LatticeMode,
    MissingPositionError,
    SqdParsingError,
    SvgParams,
    UnsupportedDefectIndexError,
)
from mnt.pyfiction._native.sidb.simulation.io import (
    SampleWritingMode,
    WriteOperationalDomainParams,
)

__all__ = [
    "ColorMode",
    "LatticeMode",
    "MissingPositionError",
    "SampleWritingMode",
    "SqdParsingError",
    "SvgParams",
    "UnsupportedDefectIndexError",
    "WriteOperationalDomainParams",
    "print_sidb_layout",
    "read_sqd_layout",
    "read_surface_defects",
    "write_critical_temperature_domain",
    "write_critical_temperature_domain_to_string",
    "write_location_and_ground_state",
    "write_operational_domain",
    "write_operational_domain_to_string",
    "write_sidb_layout_svg",
    "write_sidb_layout_svg_to_string",
    "write_sqd_layout",
    "write_sqd_sim_result",
]


def read_sqd_layout(path: str | PathLike[str], *, name: str = "") -> SiDBLayout:
    """Read SiDBs and surface defects from an SQD file.

    Args:
        path: SQD input file.
        name: Optional layout name override.

    Returns:
        The parsed SiDB layout.
    """
    return _native.read_sqd_layout(fspath(path), name)


def read_surface_defects(path: str | PathLike[str], *, name: str = "") -> SiDBLayout:
    """Read surface defects from an SQD file.

    Args:
        path: SQD input file.
        name: Optional layout name override.

    Returns:
        The parsed SiDB layout.
    """
    return _native.read_surface_defects(fspath(path), name)


def write_sqd_layout(
    layout: SiDBLayout, path: str | PathLike[str], *, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """Write SiDBs and defects to an SQD file.

    Args:
        layout: SiDB layout to serialize.
        path: SQD output file.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _native.write_sqd_layout(layout, fspath(path), on_progress)


def write_sidb_layout_svg(
    layout: SiDBLayout,
    path: str | PathLike[str],
    *,
    charges: ChargeDistribution | None = None,
    params: SvgParams | None = None,
) -> None:
    """Draw SiDBs and optional charge states as SVG. Surface defects are not drawn.

    Args:
        layout: SiDB layout to draw.
        path: SVG output file.
        charges: Optional charge distribution used to color SiDBs.
        params: Drawing options. None uses the native defaults.
    """
    options = params if params is not None else SvgParams()
    if charges is None:
        _native.write_sidb_layout_svg(layout, fspath(path), options)
    else:
        _native.write_sidb_layout_svg(layout, charges, fspath(path), options)


def write_sidb_layout_svg_to_string(
    layout: SiDBLayout, *, charges: ChargeDistribution | None = None, params: SvgParams | None = None
) -> str:
    """Draw SiDBs and optional charge states as SVG. Surface defects are not drawn.

    Args:
        layout: SiDB layout to draw.
        charges: Optional charge distribution used to color SiDBs.
        params: Drawing options. None uses the native defaults.

    Returns:
        The SVG document.
    """
    options = params if params is not None else SvgParams()
    if charges is None:
        return _native.write_sidb_layout_svg_to_string(layout, options)
    return _native.write_sidb_layout_svg_to_string(layout, charges, options)


def print_sidb_layout(
    layout: SiDBLayout,
    *,
    charges: ChargeDistribution | None = None,
    lat_color: bool = True,
    crop_layout: bool = False,
    draw_lattice: bool = True,
) -> str:
    """Render a SiDB layout as a text lattice picture.

    Args:
        layout: SiDB layout to draw.
        charges: Optional charge distribution used to color SiDBs.
        lat_color: Use ANSI colors.
        crop_layout: Include padding around the occupied sites.
        draw_lattice: Draw unoccupied lattice sites.

    Returns:
        The text picture; this function does not print it.
    """
    return _native.print_sidb_layout(layout, charges, lat_color, crop_layout, draw_lattice)


def write_sqd_sim_result(sim_result: SimulationResult, path: str | PathLike[str]) -> None:
    """Write simulation results in SiQAD format.

    Args:
        sim_result: Simulation result to serialize.
        path: Output file.
    """
    _simulation_io.write_sqd_sim_result(sim_result, fspath(path))


def write_location_and_ground_state(sim_result: SimulationResult, path: str | PathLike[str]) -> None:
    """Write SiDB locations and all ground states in semicolon-separated CSV format.

    Args:
        sim_result: Simulation result to serialize.
        path: Output file.
    """
    _simulation_io.write_location_and_ground_state(sim_result, fspath(path))


def write_operational_domain(
    domain: OperationalDomain, path: str | PathLike[str], *, params: WriteOperationalDomainParams | None = None
) -> None:
    """Serialize a operational domain as CSV.

    Args:
        domain: Domain samples to serialize.
        path: CSV output file.
        params: Sample selection and formatting options. None uses the native defaults.
    """
    _simulation_io.write_operational_domain(
        domain, fspath(path), params if params is not None else WriteOperationalDomainParams()
    )


def write_operational_domain_to_string(
    domain: OperationalDomain, *, params: WriteOperationalDomainParams | None = None
) -> str:
    """Serialize a operational domain as CSV.

    Args:
        domain: Domain samples to serialize.
        params: Sample selection and formatting options. None uses the native defaults.

    Returns:
        The CSV document.
    """
    return _simulation_io.write_operational_domain_to_string(
        domain, params if params is not None else WriteOperationalDomainParams()
    )


def write_critical_temperature_domain(
    domain: CriticalTemperatureDomain, path: str | PathLike[str], *, params: WriteOperationalDomainParams | None = None
) -> None:
    """Serialize a critical temperature domain as CSV.

    Args:
        domain: Domain samples to serialize.
        path: CSV output file.
        params: Sample selection and formatting options. None uses the native defaults.
    """
    _simulation_io.write_critical_temperature_domain(
        domain, fspath(path), params if params is not None else WriteOperationalDomainParams()
    )


def write_critical_temperature_domain_to_string(
    domain: CriticalTemperatureDomain, *, params: WriteOperationalDomainParams | None = None
) -> str:
    """Serialize a critical temperature domain as CSV.

    Args:
        domain: Domain samples to serialize.
        params: Sample selection and formatting options. None uses the native defaults.

    Returns:
        The CSV document.
    """
    return _simulation_io.write_critical_temperature_domain_to_string(
        domain, params if params is not None else WriteOperationalDomainParams()
    )
