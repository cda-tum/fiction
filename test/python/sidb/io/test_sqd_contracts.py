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

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout, site_at_row
from mnt.pyfiction.sidb.io import read_sqd_layout, sqd_parsing_error, write_sqd_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type

if TYPE_CHECKING:
    from pathlib import Path

EMPTY_SQD = "<siqad><layers><layer_prop><lat_vec/></layer_prop></layers><design/></siqad>"


@pytest.mark.parametrize("name", ["custom & <surface>", "Si(100) 2x1", "Si(111) 1x1", ""])
def test_explicit_geometry_precedes_lattice_name(tmp_path: Path, name: str) -> None:
    """SQD round trips preserve custom geometry even when its name matches a standard lattice."""
    layout = sidb_layout(lattice(name, (4.0, 1.0), (-2.0, 8.0), [(0.0, 0.0), (1.5, 2.5)]))
    for coordinates, tag in [
        ((-3, 2, 1), sidb_dot_tag.INPUT),
        ((0, 0, 0), sidb_dot_tag.NORMAL),
        ((5, 2, 0), sidb_dot_tag.LOGIC),
        ((8, 2, 1), sidb_dot_tag.OUTPUT),
    ]:
        layout.assign_sidb(lattice_site(*coordinates), tag)
    layout.assign_defect(lattice_site(1, -2), sidb_defect(sidb_defect_type.SI_VACANCY, -1, 5.6, 5.0))
    path = tmp_path / "custom.sqd"
    write_sqd_layout(layout, str(path))
    assert read_sqd_layout(str(path)) == layout


def test_every_bound_defect_round_trip(tmp_path: Path) -> None:
    """SQD preserves each supported defect label and excludes NONE from the surface."""
    layout = sidb_layout()
    for index, kind in enumerate(sidb_defect_type):
        layout.assign_defect(site_at_row(0, index), sidb_defect(kind))
    path = tmp_path / "defects.sqd"
    write_sqd_layout(layout, str(path))
    assert read_sqd_layout(str(path)) == layout


@pytest.mark.parametrize(
    "suffix", ["\0<invalid>", "\x1a<invalid>", "<other/>", EMPTY_SQD, "text", "text<!-- after -->", "<![CDATA[text]]>"]
)
def test_sqd_consumes_complete_file(tmp_path: Path, suffix: str) -> None:
    """A valid SQD prefix cannot hide trailing data or Windows text-mode end-of-file bytes."""
    path = tmp_path / "invalid.sqd"
    path.write_bytes((EMPTY_SQD + suffix).encode())
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


@pytest.mark.parametrize(
    "document",
    [
        "",
        "<other/>",
        "<siqad><design/></siqad>",
        "<siqad><layers><layer_prop/></layers><design/></siqad>",
        EMPTY_SQD.replace("<design/>", ""),
        EMPTY_SQD[:-1],
        "text" + EMPTY_SQD,
        EMPTY_SQD.replace("<lat_vec/>", "<lat_vec><name>Si(110) 1x1</name></lat_vec>"),
    ],
)
def test_sqd_requires_document_structure(tmp_path: Path, document: str) -> None:
    """Missing structure, unknown lattices, and malformed XML raise the bound parser exception."""
    path = tmp_path / "invalid.sqd"
    path.write_text(document, encoding="utf-8")
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


@pytest.mark.parametrize(
    "attributes",
    [
        'n="x" m="0" l="0"',
        'n="0" m="x" l="0"',
        'n="0" m="0" l="x"',
        'n="2147483648" m="0" l="0"',
        'n="0" m="-2147483649" l="0"',
        'n="1junk" m="2" l="1"',
        'n="1" m="2.7" l="1"',
        'n="1" m="2" l="1tail"',
        'n="0" m="0" l="-1"',
        'n="0" m="0" l="2"',
        'm="0" l="0"',
        'n="0" l="0"',
        'n="0" m="0"',
    ],
)
def test_sqd_coordinate_validation(tmp_path: Path, attributes: str) -> None:
    """Every coordinate attribute must be complete, integral, present, and representable."""
    path = tmp_path / "invalid.sqd"
    document = EMPTY_SQD.replace(
        "<design/>", f'<design><layer type="DB"><dbdot><latcoord {attributes}/></dbdot></layer></design>'
    )
    path.write_text(document, encoding="utf-8")
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


@pytest.mark.parametrize(
    "value",
    [
        'charge="x" eps_r="5.6" lambda_tf="5"',
        'charge="-1junk" eps_r="5.6" lambda_tf="5"',
        'charge="-1.5" eps_r="5.6" lambda_tf="5"',
        'charge="999999999999999999999999" eps_r="5.6" lambda_tf="5"',
        'charge="-1" eps_r="5.6junk" lambda_tf="5"',
        'charge="-1" eps_r="-1" lambda_tf="5"',
        'charge="-1" eps_r="nan" lambda_tf="5"',
        'charge="-1" eps_r="1e9999" lambda_tf="5"',
        'charge="-1" eps_r="5.6" lambda_tf="inf"',
        'charge="-1" eps_r="5.6" lambda_tf="-1"',
        'charge="-1" eps_r="5.6" lambda_tf="5junk"',
        'charge="-1" eps_r="5.6"',
        'charge="-1" lambda_tf="5"',
        'eps_r="5.6" lambda_tf="5"',
    ],
)
def test_sqd_defect_numeric_validation(tmp_path: Path, value: str) -> None:
    """Defect charge and material parameters reject missing, malformed, and non-finite values."""
    defect = (
        '<layer type="Defects"><defect><incl_coords><latcoord n="0" m="0" l="0"/></incl_coords>'
        f"<coulomb {value}/></defect></layer>"
    )
    path = tmp_path / "invalid.sqd"
    path.write_text(EMPTY_SQD.replace("<design/>", f"<design>{defect}</design>"), encoding="utf-8")
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


@pytest.mark.parametrize(
    "fragment",
    [
        "<layer><dbdot/></layer>",
        '<layer type="DB"><dbdot/></layer>',
        '<layer type="DB"><dbdot><latcoord n="0" m="0" l="0"/><type/></dbdot></layer>',
        '<layer type="DB"><dbdot><latcoord n="0" m="0" l="0"/><type>invalid</type></dbdot></layer>',
        '<layer type="Defects"><defect><incl_coords/></defect></layer>',
    ],
)
def test_sqd_dot_and_defect_structure(tmp_path: Path, fragment: str) -> None:
    """Layer types, dot coordinates, and explicit dot types must be valid."""
    path = tmp_path / "invalid.sqd"
    path.write_text(EMPTY_SQD.replace("<design/>", f"<design>{fragment}</design>"), encoding="utf-8")
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


def test_sqd_accepts_surrounding_comments_and_coordinate_whitespace(tmp_path: Path) -> None:
    """XML comments and numeric whitespace preserve the coordinate values."""
    document = EMPTY_SQD.replace(
        "<design/>", '<design><layer type="DB"><dbdot><latcoord n=" 1 " m=" 2 " l=" 1 "/></dbdot></layer></design>'
    )
    path = tmp_path / "valid.sqd"
    path.write_text(f'<?xml version="1.0"?><!-- before -->\r\n{document}\r\n<!-- after -->', encoding="utf-8")
    assert read_sqd_layout(str(path)).sidbs() == [lattice_site(1, 2, 1)]


@pytest.mark.parametrize(
    "geometry",
    [
        lattice("invalid", (float("nan"), 0), (0, 5), [(0, 0), (2, 2)]),
        lattice("invalid", (4, 0), (0, float("inf")), [(0, 0), (2, 2)]),
    ],
)
def test_sqd_invalid_geometry_preserves_file(tmp_path: Path, geometry: lattice) -> None:
    """Export rejects non-finite geometry before replacing an existing destination."""
    path = tmp_path / "preserved.sqd"
    path.write_bytes(b"original")
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- exception type is the contract
        write_sqd_layout(sidb_layout(geometry), str(path))
    assert path.read_bytes() == b"original"
    assert list(tmp_path.iterdir()) == [path]


@pytest.mark.parametrize(
    ("original", "replacement"),
    [
        ('<a1 x="4" y="1"/>', ""),
        ('<a2 x="-2" y="8"/>', ""),
        ('<b1 x="0" y="0"/>', ""),
        ('<b2 x="1.5" y="2.5"/>', ""),
        ("<N>2</N>", ""),
        ("<N>2</N>", "<N/>"),
        ("<N>2</N>", "<N>1</N>"),
        ("<N>2</N>", "<N>3</N>"),
        ("<N>2</N>", "<N>2.5</N>"),
        ("<N>2</N>", "<N>2junk</N>"),
        ('x="4"', ""),
        ('y="1"', ""),
        ('x="4"', 'x=""'),
        ('x="4"', 'x="nan"'),
        ('x="4"', 'x="inf"'),
        ('x="4"', 'x="1e999"'),
        ('x="4"', 'x="text"'),
        ('x="4"', 'x="4junk"'),
    ],
)
def test_sqd_explicit_lattice_geometry_validation(tmp_path: Path, original: str, replacement: str) -> None:
    """Explicit lattice vectors and basis sites require complete finite coordinates and exactly two basis sites."""
    geometry = '<name>custom</name><a1 x="4" y="1"/><a2 x="-2" y="8"/><N>2</N><b1 x="0" y="0"/><b2 x="1.5" y="2.5"/>'
    document = EMPTY_SQD.replace("<lat_vec/>", f"<lat_vec>{geometry.replace(original, replacement, 1)}</lat_vec>")
    path = tmp_path / "invalid_geometry.sqd"
    path.write_text(document, encoding="utf-8")
    with pytest.raises(sqd_parsing_error):
        read_sqd_layout(str(path))


def test_sqd_invalid_charged_defect_preserves_file(tmp_path: Path) -> None:
    """A charged defect needs valid material parameters before an existing destination is replaced."""
    layout = sidb_layout()
    layout.assign_defect(lattice_site(), sidb_defect(sidb_defect_type.SI_VACANCY, -1, float("inf"), 5.0))
    path = tmp_path / "preserved.sqd"
    path.write_bytes(b"original")
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- exception type is the contract
        write_sqd_layout(layout, str(path))
    assert path.read_bytes() == b"original"
    assert list(tmp_path.iterdir()) == [path]
