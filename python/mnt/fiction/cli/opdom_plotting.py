# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Optional static and interactive plots of operational domain samples."""

from __future__ import annotations

import tempfile
from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction.sidb.simulation.logic import operational_status, sweep_parameter

from .drawing import open_viewer
from .errors import CommandError
from .parsing import positive_float, positive_int

if TYPE_CHECKING:
    import argparse

    from matplotlib.figure import Figure
    from plotly.graph_objects import Figure as PlotlyFigure

    from mnt.pyfiction.sidb.simulation.logic import operational_domain

    from .parsing import Parser

LABELS = {
    sweep_parameter.EPSILON_R: (r"$\epsilon_r$", "epsilon_r"),
    sweep_parameter.LAMBDA_TF: (r"$\lambda_{\mathrm{TF}}$ [nm]", "lambda_tf [nm]"),
    sweep_parameter.MU_MINUS: (r"$\mu^{-}$ [eV]", "mu_minus [eV]"),
}
"""Matplotlib and HTML axis labels, including physical units."""

DIMENSIONS_3D = 3
"""Number of axes for a three-dimensional plot."""


def plot_arguments(parser: Parser) -> None:
    """Add options shared by both operational domain entry points.

    Args:
        parser: The command parser.
    """
    plot = parser.add_argument_group("plotting (requires mnt-pyfiction[plot])")
    plot.add_argument(
        "--plot",
        type=Path,
        action="append",
        default=[],
        metavar="FILE",
        help="PNG, SVG, PDF, or HTML; repeat for several outputs",
    )
    plot.add_argument("--show", action="store_true", help="open saved plots in the platform viewer")
    plot.add_argument("--no-legend", action="store_true", help="hide plot legends")
    plot.add_argument("--no-non-operational", action="store_true", help="hide non-operational samples in plots")
    title = plot.add_mutually_exclusive_group()
    title.add_argument("--no-title", action="store_true", help="hide plot titles")
    title.add_argument("--title", help="custom plot title")
    plot.add_argument("--operational-color", help="operational marker color; purple, or yellow for Sketch")
    plot.add_argument("--non-operational-color", default="#BFBFBF", help="non-operational marker color")
    plot.add_argument("--operational-size", type=positive_float, default=4.0, help="operational marker diameter")
    plot.add_argument(
        "--non-operational-size", type=positive_float, default=2.0, help="non-operational marker diameter"
    )
    plot.add_argument(
        "--width", type=positive_float, default=7.0, help="figure width in inches (96 pixels/inch for HTML)"
    )
    plot.add_argument(
        "--height", type=positive_float, default=6.0, help="figure height in inches (96 pixels/inch for HTML)"
    )
    plot.add_argument("--dpi", type=positive_int, default=300, help="static-image resolution")


def validate_outputs(args: argparse.Namespace, *, log_path: Path | None = None) -> None:
    """Validate destinations, dependencies, and colors before computation.

    Args:
        args: CSV and plot paths and appearance options.
        log_path: An optional statistics log destination.

    Raises:
        CommandError: An output or plotting option is invalid or a dependency is missing.
    """
    if args.show and not args.plot:
        msg = "--show requires a --plot output"
        raise CommandError(msg)
    seen: list[Path] = []
    paths = ([args.file] if args.file is not None else []) + args.plot + ([log_path] if log_path is not None else [])
    for path in paths:
        _validate_path(path)
        if any(_same_destination(path, other) for other in seen):
            msg = "output paths must be distinct"
            raise CommandError(msg)
        if getattr(args, "input", None) is not None and _same_destination(path, args.input):
            msg = "an output path would overwrite the input SQD file"
            raise CommandError(msg)
        seen.append(path)
    for path in args.plot:
        if path.suffix.lower() not in {".png", ".svg", ".pdf", ".html"}:
            msg = "plot outputs require .png, .svg, .pdf, or .html"
            raise CommandError(msg)
        try:
            _validate_colors(args, html=path.suffix.lower() == ".html")
        except ImportError as error:
            msg = "plotting requires 'pip install mnt-pyfiction[plot]'"
            raise CommandError(msg) from error
        except ValueError as error:
            msg = f"invalid plot color: {error}"
            raise CommandError(msg) from error


def _same_destination(left: Path, right: Path) -> bool:
    """Compare paths, including symbolic links and existing hard links.

    Args:
        left: The first path.
        right: The second path.

    Returns:
        Whether the paths name the same destination.
    """
    return left.resolve() == right.resolve() or (left.exists() and right.exists() and left.samefile(right))


def _validate_path(path: Path) -> None:
    """Reject unsupported destinations without opening them.

    Args:
        path: A requested output path.

    Raises:
        CommandError: The path is dangling, non-regular, or has no parent directory.
    """
    if path.is_symlink() and not path.exists():
        msg = f"'{path}' is a dangling output link"
        raise CommandError(msg)
    if path.exists() and not path.is_file():
        msg = f"'{path}' is not a regular output file"
        raise CommandError(msg)
    if not path.parent.is_dir():
        msg = f"output directory does not exist: '{path.parent}'"
        raise CommandError(msg)


def _validate_colors(args: argparse.Namespace, *, html: bool) -> None:
    """Validate colors using the requested backend's parser.

    Args:
        args: Marker colors and analysis mode.
        html: Use Plotly's color parser.

    Raises:
        ValueError: A color is invalid for the backend.
    """
    if html:
        from plotly.graph_objects import Scatter  # ruff: ignore[import-outside-top-level] -- optional backend

        for color in _colors(args):
            Scatter(marker={"color": color})
    else:
        from matplotlib.colors import is_color_like  # ruff: ignore[import-outside-top-level] -- optional backend

        if not all(is_color_like(color) for color in _colors(args)):
            msg = "invalid marker color"
            raise ValueError(msg)


def _colors(args: argparse.Namespace) -> tuple[str, str]:
    """Return the selected colors, including the Sketch default.

    Args:
        args: Color overrides and Sketch selection.

    Returns:
        Operational and non-operational colors.
    """
    return args.operational_color or ("#FBBF24" if args.sketch else "#801A99"), args.non_operational_color


def _series(domain: operational_domain, args: argparse.Namespace) -> list[tuple[str, str, float, list[list[float]]]]:
    """Group returned samples by status without interpolating missing points.

    Args:
        domain: The computed domain.
        args: Status visibility and marker options.

    Returns:
        Label, color, diameter, and coordinates for each visible status.
    """
    colors = _colors(args)
    operational: list[list[float]] = []
    non_operational: list[list[float]] = []
    for point, status in domain.items():
        (operational if status == operational_status.OPERATIONAL else non_operational).append(point.get_parameters())
    result = [
        ("Potentially operational" if args.sketch else "Operational", colors[0], args.operational_size, operational)
    ]
    if not args.no_non_operational:
        result.append(("Non-operational", colors[1], args.non_operational_size, non_operational))
    return result


def _title(args: argparse.Namespace) -> str:
    """Choose the plot title.

    Args:
        args: Title options and analysis mode.

    Returns:
        The title, or an empty string when hidden.
    """
    return "" if args.no_title else args.title or ("Operational domain sketch" if args.sketch else "Operational domain")


def matplotlib_figure(domain: operational_domain, args: argparse.Namespace) -> Figure:
    """Build a static 2D or 3D sample plot without pyplot's global figure state.

    Args:
        domain: The computed samples and ordered axes.
        args: Plot appearance options.

    Returns:
        A figure ready for PNG, SVG, or PDF export.
    """
    from matplotlib.figure import Figure  # ruff: ignore[import-outside-top-level] -- optional backend

    dimensions = domain.get_number_of_dimensions()
    figure = Figure(figsize=(args.width, args.height), dpi=args.dpi, layout="constrained")
    axis = figure.add_subplot(projection="3d" if dimensions == DIMENSIONS_3D else None)
    for label, color, size, points in _series(domain, args):
        coordinates = [[point[index] for point in points] for index in range(dimensions)]
        axis.scatter(*coordinates, color=color, s=size**2, label=label)
    for index, name in enumerate(("x", "y", "z")[:dimensions]):
        getattr(axis, f"set_{name}label")(LABELS[domain.get_dimension(index)][0])
        low, high, step = (getattr(args, f"{name}_{key}") for key in ("min", "max", "step"))
        getattr(axis, f"set_{name}lim")(low - step / 2, high + step / 2)
    axis.set_title(_title(args))
    axis.grid(visible=True, color="#E2E8F0", linestyle="--", alpha=0.7)
    if not args.no_legend:
        axis.legend()
    return figure


def plotly_figure(domain: operational_domain, args: argparse.Namespace) -> PlotlyFigure:
    """Build an interactive 2D or 3D sample plot.

    Args:
        domain: The computed samples and ordered axes.
        args: Plot appearance options.

    Returns:
        A figure with parameter and status hover information and standard Plotly controls.
    """
    import plotly.graph_objects as go  # ruff: ignore[import-outside-top-level] -- optional backend

    dimensions = domain.get_number_of_dimensions()
    labels = [LABELS[domain.get_dimension(index)][1] for index in range(dimensions)]
    figure = go.Figure()
    for label, color, size, points in _series(domain, args):
        coordinates = {
            name: [point[index] for point in points] for index, name in enumerate(("x", "y", "z")[:dimensions])
        }
        trace = go.Scatter3d if dimensions == DIMENSIONS_3D else go.Scatter
        hover = "<br>".join(f"{labels[index]}: %{{{name}}}" for index, name in enumerate(coordinates))
        figure.add_trace(
            trace(
                **coordinates,
                mode="markers",
                name=label,
                marker={"color": color, "size": size},
                hovertemplate=hover + "<extra>" + label + "</extra>",
            )
        )
    axes = {
        f"{name}axis": {
            "title": labels[index],
            "range": [
                getattr(args, f"{name}_min") - getattr(args, f"{name}_step") / 2,
                getattr(args, f"{name}_max") + getattr(args, f"{name}_step") / 2,
            ],
        }
        for index, name in enumerate(("x", "y", "z")[:dimensions])
    }
    figure.update_layout(
        title=_title(args),
        showlegend=not args.no_legend,
        template="none",
        paper_bgcolor="white",
        plot_bgcolor="white",
        width=max(10, round(args.width * 96)),
        height=max(10, round(args.height * 96)),
        **({"scene": axes} if dimensions == DIMENSIONS_3D else axes),
    )
    return figure


def write_plot(domain: operational_domain, path: Path, args: argparse.Namespace) -> None:
    """Serialize a plot to a sibling temporary file, then replace the destination.

    Existing symbolic links retain their link and receive output through their target.

    Args:
        domain: The computed domain.
        path: PNG, SVG, PDF, or HTML destination.
        args: Appearance and viewer options.
    """
    destination = path.resolve(strict=True) if path.is_symlink() else path
    with tempfile.TemporaryDirectory(prefix=".fiction-", dir=destination.parent) as directory:
        temporary = Path(directory) / destination.name
        if path.suffix.lower() == ".html":
            plotly_figure(domain, args).write_html(
                str(temporary), include_plotlyjs=True, full_html=True, auto_open=False
            )
        else:
            figure = matplotlib_figure(domain, args)
            try:
                figure.savefig(temporary, format=path.suffix[1:].lower(), dpi=args.dpi)
            finally:
                figure.clear()
        if destination.exists():
            temporary.chmod(destination.stat().st_mode)
        temporary.replace(destination)
    if args.show:
        open_viewer(path)
