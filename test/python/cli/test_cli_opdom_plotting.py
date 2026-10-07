# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Static and interactive operational domain plots."""

from __future__ import annotations

import importlib
from typing import TYPE_CHECKING

import numpy as np
import pytest
from mpl_toolkits.mplot3d import Axes3D

from mnt.fiction.cli.registry import REGISTRY
from mnt.pyfiction.sidb.simulation.logic import operational_domain, operational_status, parameter_point, sweep_parameter

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


@pytest.fixture(params=[2, 3])
def domain(request: pytest.FixtureRequest) -> operational_domain:
    """Two classified points in a domain whose axis order differs from the defaults.

    Returns:
        The sample domain.
    """
    dimensions = [sweep_parameter.LAMBDA_TF, sweep_parameter.EPSILON_R, sweep_parameter.MU_MINUS][: request.param]
    result = operational_domain(dimensions)
    result[parameter_point([5.0, 5.6, -0.32][: request.param])] = operational_status.OPERATIONAL
    result[parameter_point([5.1, 5.7, -0.3][: request.param])] = operational_status.NON_OPERATIONAL
    return result


def options(*flags: str) -> argparse.Namespace:
    """Parse plot options through the public shell command.

    Args:
        flags: Extra command arguments.

    Returns:
        Parsed options.
    """
    return REGISTRY["opdom"].parser.parse_args(["domain.csv", "--plot", "domain.png", *flags])


def test_static_sample_coordinates_and_axes(domain: operational_domain) -> None:
    """Static plots preserve point coordinates and selected axis order."""
    plotting = importlib.import_module("mnt.fiction.cli.opdom_plotting")
    figure = plotting.matplotlib_figure(domain, options())
    axis = figure.axes[0]
    assert "lambda" in axis.get_xlabel().lower() or "lambda" in axis.get_xlabel()
    assert "epsilon" in axis.get_ylabel().lower()
    assert len(axis.collections) == 2
    legend = axis.get_legend()
    assert legend is not None
    assert [text.get_text() for text in legend.get_texts()] == ["Operational", "Non-operational"]
    if domain.get_number_of_dimensions() == 2:
        assert np.asarray(axis.collections[0].get_offsets()).tolist() == [[5.0, 5.6]]
        assert np.asarray(axis.collections[1].get_offsets()).tolist() == [[5.1, 5.7]]
    else:
        assert isinstance(axis, Axes3D)
        assert "mu" in axis.get_zlabel().lower()


def test_interactive_samples_and_visibility(domain: operational_domain) -> None:
    """HTML plots omit hidden samples, label Sketch positives, and hide legends on request."""
    plotting = importlib.import_module("mnt.fiction.cli.opdom_plotting")
    figure = plotting.plotly_figure(
        domain,
        options(
            "--sketch",
            "--no-legend",
            "--no-non-operational",
            "--title",
            "Domain",
            "--operational-color",
            "#123456",
            "--operational-size",
            "6",
        ),
    )
    assert len(figure.data) == 1
    assert list(figure.data[0].x) == [5.0]
    assert list(figure.data[0].y) == [5.6]
    assert figure.data[0].name == "Potentially operational"
    assert figure.data[0].marker.color == "#123456"
    assert figure.data[0].marker.size == 6
    assert figure.layout.showlegend is False
    assert figure.layout.title.text == "Domain"
    if domain.get_number_of_dimensions() == 3:
        assert list(figure.data[0].z) == [-0.32]
        assert figure.data[0].type == "scatter3d"


def test_static_visibility_and_size(domain: operational_domain) -> None:
    """Static plots honor the same visibility and figure controls as HTML."""
    plotting = importlib.import_module("mnt.fiction.cli.opdom_plotting")
    figure = plotting.matplotlib_figure(
        domain,
        options("--no-legend", "--no-non-operational", "--no-title", "--width", "8", "--height", "4", "--dpi", "150"),
    )
    assert len(figure.axes[0].collections) == 1
    assert figure.axes[0].get_legend() is None
    assert not figure.axes[0].get_title()
    assert figure.get_size_inches().tolist() == [8, 4]
    assert figure.dpi == 150


@pytest.mark.parametrize("suffix", ["png", "svg", "pdf", "html"])
def test_opdom_plot_files(xor_gate: Shell, tmp_path: Path, suffix: str) -> None:
    """A shell computation can export every supported plot format."""
    path = tmp_path / f"domain.{suffix}"
    xor_gate.ok(f'opdom "{tmp_path / "domain.csv"}" --plot "{path}" --x-min 5.6 --x-max 5.6 --y-min 5 --y-max 5')
    data = path.read_bytes()
    assert data
    if suffix == "png":
        assert data.startswith(b"\x89PNG")
    elif suffix == "pdf":
        assert data.startswith(b"%PDF")
    elif suffix == "svg":
        assert b"<svg" in data
    else:
        assert b"Plotly.newPlot" in data
        assert b"<script src=" not in data


def test_default_png(resource: Callable[[str], str], tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """The dedicated command defaults to a PNG in the working directory."""
    main = importlib.import_module("mnt.fiction.opdom").main
    monkeypatch.chdir(tmp_path)
    assert (
        main([
            resource("hex_21_inputsdbp_xor_v1.sqd"),
            "--gate",
            "xor",
            "--x-min",
            "5.6",
            "--x-max",
            "5.6",
            "--y-min",
            "5",
            "--y-max",
            "5",
        ])
        == 0
    )
    assert (tmp_path / "hex_21_inputsdbp_xor_v1_opdom.png").read_bytes().startswith(b"\x89PNG")


@pytest.mark.parametrize(
    "flags",
    [
        "--plot bad.txt",
        "--plot out.png --width 0",
        "--plot out.html --dpi 0",
        "--plot out.png --operational-color invalid",
        "--plot out.html --non-operational-color invalid",
    ],
)
def test_plot_preflight_errors(xor_gate: Shell, tmp_path: Path, flags: str) -> None:
    """Invalid plot options fail before writing the CSV or starting computation."""
    path = tmp_path / "domain.csv"
    xor_gate.fails(f'opdom "{path}" {flags} --x-min 5.6 --x-max 5.6 --y-min 5 --y-max 5')
    assert not path.exists()
