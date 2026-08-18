"""Generate a randomly defective H-Si surface and write it out as a lattice of array values.

Each H-Si atom is one array value. The surface is a 2D array of dimer rows, where one dimer
is made of two H-Si atoms, and every lattice point carries the array value of the defect
occupying it.
"""

from __future__ import annotations

import logging
import random
import sys

import matplotlib.pyplot as plt
import numpy as np

np.set_printoptions(threshold=sys.maxsize)
logging.getLogger("matplotlib.axes._axes").setLevel(logging.ERROR)


class DefectSurface:
    """A 2D lattice of H-Si atoms, each holding the array value of the defect that occupies it."""

    def __init__(self, surface_width: int = 100, surface_height: int = 100) -> None:
        """Initialize every lattice point of the surface to the H-Si array value.

        Args:
            surface_width: The number of dimers within a row.
            surface_height: The number of dimer rows. One dimer is made of two H-Si atoms.
        """
        self.defects_name = None
        self.defect_params = None
        self.defect_plotting = None
        self.total_defect_lattice_points = None

        self.surface_width = surface_width
        self.surface_height = surface_height

        self.a1 = 3.84  # (width) in nm
        self.a2 = 7.68  # (height)
        self.b2 = 2.25  # (distance within dimer)

        self.surface_lattice = np.full((self.surface_height, self.surface_width), 0)

    def add_defects(self, coverage: float = 0.05) -> None:
        """Scatter defects across the surface until the requested coverage is accounted for.

        ``self.defect_params`` configures the ratio and size of each defect. One entry reads
        ``[array_value, width, height, ratio]``, where ``array_value`` is the number assigned to
        each lattice point in ``self.surface_lattice``, ``width`` and ``height`` count H-Si atoms
        rather than dimer rows, and ``ratio`` is a fractional percent, so ``0.05`` is 5%.

        Args:
            coverage: The total coverage of defects. A fully defected surface is ``1.0``.
        """
        self.defects_name = [
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
        self.defect_params = [
            [0, 1, 1, 1.0],  # HSi
            [1, 1, 1, 0.0],  # [array_value,width,height,ratio] DB
            [2, 1, 2, 0.0],  # si_vacancy 0.05
            [3, 1, 2, 0.2],  # dihydride_pair
            [4, 1, 2, 0.05],  # single_dihydride
            [5, 4, 2, 0.05],  # onebyone
            [6, 4, 4, 0.05],  # threebyone
            [7, 1, 2, 0.1],  # siloxane
            [8, 1, 2, 0.1],  # raised_si
            [9, 3, 2, 0.05],  # etch pit
            [10, 1, 2, 0.05],  # missing_dimer
        ]

        self.defect_plotting = [
            [(255 / 255, 0, 0), "o"],  # HSi
            [(0, 0, 255 / 255), "o"],  # [colour,shape] DB
            [(0, 0, 0), "s"],  # si_vacancy
            [(58 / 255, 0, 83 / 255), "s"],  # dihydride_pair
            [(72 / 255, 53 / 255, 4 / 255), "s"],  # single_dihydride
            [(42 / 255, 56 / 255, 14 / 255), "s"],  # onebyone
            [(173 / 255, 255 / 255, 47 / 255), "s"],  # threebyone
            [(64 / 255, 224 / 255, 208 / 255), "s"],  # siloxane
            [(119 / 255, 136 / 255, 153 / 255), "s"],  # raised_si
            [(255 / 255, 255 / 255, 0), "s"],  # etch_pit
            [(227 / 255, 28 / 255, 121 / 255), "s"],  # missing_dimer
        ]

        self.total_defect_lattice_points = int(self.surface_width * self.surface_height * coverage)
        for defect in self.defect_params:
            num_of_defects = int(defect[3] * self.total_defect_lattice_points)
            # a placement that lands out of bounds or on an already occupied region is skipped, not
            # retried: the original `i = i - 1` reassigned the loop variable, which `range` discards
            # on the next iteration, so it never retried either. Retrying would change the surface
            # this experiment produces
            for _ in range(1, num_of_defects):
                random_width = random.randint(0, self.surface_width - 1)
                random_height = random.randint(0, self.surface_height - 1)

                if (random_width > random_width + defect[1] and random_height > random_height + defect[2]) or (
                    random_height % 2 == 1 and defect[2] % 2 == 0
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

        label_list = np.zeros(np.shape(self.defect_params)[0], dtype=bool)

        for y in range(self.surface_height):
            for x in range(self.surface_width):
                if not label_list[self.surface_lattice[y][x]]:
                    lab = self.defects_name[self.surface_lattice[y][x]]
                    label_list[self.surface_lattice[y][x]] = True
                else:
                    lab = ""

                if y % 2 == 0:
                    plt.scatter(
                        x * self.a1,
                        y * self.a2 + self.b2 / 2,
                        c=self.defect_plotting[self.surface_lattice[y][x]][0],
                        label=lab,
                        marker=self.defect_plotting[self.surface_lattice[y][x]][1],
                    )
                elif y % 2 == 1:
                    plt.scatter(
                        x * self.a1,
                        y * self.a2 - self.b2 / 2,
                        c=self.defect_plotting[self.surface_lattice[y][x]][0],
                        label=lab,
                        marker=self.defect_plotting[self.surface_lattice[y][x]][1],
                    )

        plt.axis("equal")
        plt.xticks([])
        plt.yticks([])
        plt.legend()
        plt.show()

    def save_to_file(self, filename: str = "test.txt") -> None:
        """Write the surface lattice out as text.

        Args:
            filename: The path to write the lattice to.
        """
        np.savetxt(filename, self.surface_lattice)


surface_width = 740
surface_height = 1090
coverage = 0.005
surface = DefectSurface(surface_width=surface_width, surface_height=surface_height)
surface.add_defects(coverage=coverage)
# `surface.draw_panels()` plots the result and `surface.save_to_file(...)` writes it to disk
