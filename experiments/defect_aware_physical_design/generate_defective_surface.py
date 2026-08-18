#!/usr/bin/env -S uv run --script --quiet
# /// script
# requires-python = ">=3.10"
# dependencies = ["matplotlib", "numpy"]
# ///

"""Generate a randomly defective H-Si surface and write it out as a lattice of array values.

Each H-Si atom is one array value. The surface is a 2D array of dimer rows, where one dimer
is made of two H-Si atoms, and every lattice point carries the array value of the defect
occupying it.

Run it with no arguments to reproduce the published surface, or pass ``--help`` for the
parameters it accepts.
"""

from __future__ import annotations

import argparse
import logging
import random
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

np.set_printoptions(threshold=sys.maxsize)
logging.getLogger("matplotlib.axes._axes").setLevel(logging.ERROR)

DEFECT_NAMES: list[str] = [
    "H-Si",
    "DB",
    "si_vacancy",
    "dihydride_pair",
    "single_dihydride",
    "onebyone",
    "threebyone",
    "siloxane",
    "raised_Si",
    "etch_pit",
    "missing_dimer",
]

# one entry reads (array_value, width, height, ratio). `array_value` is the number assigned to each
# lattice point the defect occupies, `width` and `height` count H-Si atoms rather than dimer rows,
# and `ratio` is a fractional percent, so 0.05 is 5%
DEFECT_PARAMS: list[tuple[int, int, int, float]] = [
    (0, 1, 1, 1.0),  # HSi
    (1, 1, 1, 0.0),  # DB
    (2, 1, 2, 0.0),  # si_vacancy 0.05
    (3, 1, 2, 0.2),  # dihydride_pair
    (4, 1, 2, 0.05),  # single_dihydride
    (5, 4, 2, 0.05),  # onebyone
    (6, 4, 4, 0.05),  # threebyone
    (7, 1, 2, 0.1),  # siloxane
    (8, 1, 2, 0.1),  # raised_si
    (9, 3, 2, 0.05),  # etch pit
    (10, 1, 2, 0.05),  # missing_dimer
]

# one entry reads (colour, marker shape), indexed by the same array value as `DEFECT_PARAMS`
DEFECT_PLOTTING: list[tuple[tuple[float, float, float], str]] = [
    ((255 / 255, 0, 0), "o"),  # HSi
    ((0, 0, 255 / 255), "o"),  # DB
    ((0, 0, 0), "s"),  # si_vacancy
    ((58 / 255, 0, 83 / 255), "s"),  # dihydride_pair
    ((72 / 255, 53 / 255, 4 / 255), "s"),  # single_dihydride
    ((42 / 255, 56 / 255, 14 / 255), "s"),  # onebyone
    ((173 / 255, 255 / 255, 47 / 255), "s"),  # threebyone
    ((64 / 255, 224 / 255, 208 / 255), "s"),  # siloxane
    ((119 / 255, 136 / 255, 153 / 255), "s"),  # raised_si
    ((255 / 255, 255 / 255, 0), "s"),  # etch_pit
    ((227 / 255, 28 / 255, 121 / 255), "s"),  # missing_dimer
]


class DefectSurface:
    """A 2D lattice of H-Si atoms, each holding the array value of the defect that occupies it."""

    def __init__(self, surface_width: int = 100, surface_height: int = 100) -> None:
        """Initialize every lattice point of the surface to the H-Si array value.

        Args:
            surface_width: The number of dimers within a row.
            surface_height: The number of dimer rows. One dimer is made of two H-Si atoms.
        """
        self.total_defect_lattice_points = 0

        self.surface_width = surface_width
        self.surface_height = surface_height

        self.a1 = 3.84  # (width) in nm
        self.a2 = 7.68  # (height)
        self.b2 = 2.25  # (distance within dimer)

        self.surface_lattice = np.full((self.surface_height, self.surface_width), 0)

    def add_defects(self, coverage: float = 0.05) -> None:
        """Scatter defects across the surface until the requested coverage is accounted for.

        ``DEFECT_PARAMS`` configures the ratio and size of each defect.

        Args:
            coverage: The total coverage of defects. A fully defected surface is ``1.0``.
        """
        self.total_defect_lattice_points = int(self.surface_width * self.surface_height * coverage)
        for defect in DEFECT_PARAMS:
            num_of_defects = int(defect[3] * self.total_defect_lattice_points)
            # a placement that lands out of bounds or on an already occupied region is skipped, not
            # retried: the original `i = i - 1` reassigned the loop variable, which `range` discards
            # on the next iteration, so it never retried either. Retrying would change the surface
            # this experiment produces
            for _ in range(1, num_of_defects):
                random_width = random.randint(0, self.surface_width - 1)
                random_height = random.randint(0, self.surface_height - 1)

                # a defect whose footprint runs off the right or bottom edge is rejected. numpy
                # truncates such a slice instead of raising, so before this check the experiment
                # placed a clipped defect there
                if (
                    random_width + defect[1] > self.surface_width
                    or random_height + defect[2] > self.surface_height
                    or (random_height % 2 == 1 and defect[2] % 2 == 0)
                ):
                    continue

                if np.all(
                    self.surface_lattice[
                        random_height : random_height + defect[2],
                        random_width : random_width + defect[1],
                    ]
                    == 0,
                ):
                    self.surface_lattice[
                        random_height : random_height + defect[2],
                        random_width : random_width + defect[1],
                    ] = defect[0]

    def draw_panels(self) -> None:
        """Plot the surface, one marker per lattice point, coloured and shaped by defect."""
        width_nm = self.a1 * self.surface_width
        height_nm = self.a2 * self.surface_height

        fig = plt.figure(figsize=((width_nm + 1) / 10, (height_nm + 1) / 10), dpi=100)
        fig.add_subplot(1, 1, 1)
        plt.gca().invert_yaxis()

        label_list = np.zeros(len(DEFECT_PARAMS), dtype=np.bool_)

        for y in range(self.surface_height):
            for x in range(self.surface_width):
                if not label_list[self.surface_lattice[y][x]]:
                    lab = DEFECT_NAMES[self.surface_lattice[y][x]]
                    label_list[self.surface_lattice[y][x]] = True
                else:
                    lab = ""

                if y % 2 == 0:
                    plt.scatter(
                        x * self.a1,
                        y * self.a2 + self.b2 / 2,
                        c=DEFECT_PLOTTING[self.surface_lattice[y][x]][0],
                        label=lab,
                        marker=DEFECT_PLOTTING[self.surface_lattice[y][x]][1],
                    )
                elif y % 2 == 1:
                    plt.scatter(
                        x * self.a1,
                        y * self.a2 - self.b2 / 2,
                        c=DEFECT_PLOTTING[self.surface_lattice[y][x]][0],
                        label=lab,
                        marker=DEFECT_PLOTTING[self.surface_lattice[y][x]][1],
                    )

        plt.axis("equal")
        plt.xticks([])
        plt.yticks([])
        plt.legend()
        plt.show()

    def save_to_file(self, filename: str | Path = "defective_surface.csv") -> None:
        """Write the surface lattice out as comma-separated array values, one row per line.

        Args:
            filename: The path to write the lattice to.
        """
        np.savetxt(filename, self.surface_lattice, fmt="%d", delimiter=",")


def main() -> None:
    """Generate a defective surface and write it out."""
    parser = argparse.ArgumentParser(description="Generate a randomly defective H-Si surface.")
    parser.add_argument("--width", type=int, default=740, help="number of dimers within a row")
    parser.add_argument("--height", type=int, default=1090, help="number of dimer rows")
    parser.add_argument(
        "--coverage",
        type=float,
        default=0.005,
        help="total coverage of defects, where a fully defected surface is 1.0",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=Path("defective_surface.csv"),
        help="path to write the lattice to",
    )
    parser.add_argument("--plot", action="store_true", help="also plot the surface")
    args = parser.parse_args()

    surface = DefectSurface(surface_width=args.width, surface_height=args.height)
    surface.add_defects(coverage=args.coverage)
    surface.save_to_file(args.output)
    print(f"wrote {args.output}")

    if args.plot:
        surface.draw_panels()


if __name__ == "__main__":
    main()
