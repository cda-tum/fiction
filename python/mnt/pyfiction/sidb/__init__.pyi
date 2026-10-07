# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""
Silicon Dangling Bond (SiDB) lattices, layouts, charge distributions, and simulation.
"""

import enum
from collections.abc import Sequence
from typing import overload

from mnt.pyfiction.sidb import generators as generators
from mnt.pyfiction.sidb import io as io
from mnt.pyfiction.sidb import model as model
from mnt.pyfiction.sidb import simulation as simulation

class lattice_site:
    """
    A site of the H-Si surface lattice. Site `(x, y, z)` is the `z`-th
    basis site of the unit cell reached by `x` steps along the first and
    `y` steps along the second lattice vector of a `lattice`. This is the
    `(n, m, l)` lattice coordinate of SiQAD's SQD files. Both supported
    reconstructions have a two-site basis, so `z` is either 0 or 1.

    Sites order in raster order: by `y`, then by `z`, then by `x`.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor. Creates the site `(0, 0, 0)`."""

    @overload
    def __init__(self, x: int, y: int, z: int) -> None:
        """
        Creates the site `(x, y, z)`. Coordinates must be representable by the
        parameter types before the call.

        Args:
            x_coord: Steps along the first lattice vector.
            y_coord: Steps along the second lattice vector.
            basis_site: Basis site, 0 or 1.

        Raises:
            std::out_of_range: if the basis index is not 0 or 1.
        """

    @overload
    def __init__(self, x: int, y: int) -> None:
        """
        Creates the site `(x, y, 0)`.

        Args:
            x_coord: Steps along the first lattice vector.
            y_coord: Steps along the second lattice vector.
        """

    @property
    def x(self) -> int:
        """Steps along the first lattice vector."""

    @x.setter
    def x(self, arg: int, /) -> None: ...
    @property
    def y(self) -> int:
        """Steps along the second lattice vector."""

    @y.setter
    def y(self, arg: int, /) -> None: ...
    @property
    def z(self) -> int:
        """Basis site within the unit cell, 0 or 1."""

    @z.setter
    def z(self, arg: int, /) -> None: ...
    def __eq__(self, arg: lattice_site, /) -> bool:
        """
        Compares two sites for equality.

        Args:
            other: Right-hand side site.

        Returns:
            `true` iff both sites are identical.
        """

    def __ne__(self, arg: lattice_site, /) -> bool: ...
    def __lt__(self, arg: lattice_site, /) -> bool:
        """
        Orders two sites in raster order: by `y`, then by `z`, then by `x`.

        Args:
            other: Right-hand side site.

        Returns:
            The ordering of this site relative to `other`.
        """

    def __le__(self, arg: lattice_site, /) -> bool: ...
    def __gt__(self, arg: lattice_site, /) -> bool: ...
    def __ge__(self, arg: lattice_site, /) -> bool: ...
    def __add__(self, arg: lattice_site, /) -> lattice_site:
        """
        Adds another site to this one, carrying an overflowing basis index
        into the next unit cell along the second lattice vector. Does not
        modify this site.

        Args:
            other: Site to add.

        Returns:
            Sum of both sites.

        Raises:
            std::out_of_range: if a basis index is invalid or a result
                               coordinate exceeds the lattice-site range.
        """

    def __sub__(self, arg: lattice_site, /) -> lattice_site:
        """
        Subtracts another site from this one, borrowing from the previous unit
        cell along the second lattice vector when the basis index underflows.
        Does not modify this site.

        Args:
            other: Site to subtract.

        Returns:
            Difference of both sites.

        Raises:
            std::out_of_range: if a basis index is invalid or a result
                               coordinate exceeds the lattice-site range.
        """

    def __hash__(self) -> int: ...

def row_of(site: lattice_site) -> int:
    """
    The row of a site counted in single SiDB rows: `2 * y + z`. SiDB gate
    libraries describe their gates on a grid of such rows.

    Args:
        s: Site.

    Returns:
        Row of `s`.
    """

def site_at_row(x: int, row: int) -> lattice_site:
    """
    The site at a given column and single-SiDB row, the inverse of
    `row_of`. Negative rows map to the unit cell below them, so `(x, -1)`
    is `(x, -1, 1)`.

    Args:
        x: Column.
        row: Row counted in single SiDB rows.

    Returns:
        The site `(x, floor(row / 2), row mod 2)`.

    Raises:
        std::out_of_range: if the row exceeds the range of lattice sites.
    """

def sites_in_area(first_corner: lattice_site, second_corner: lattice_site) -> list[lattice_site]:
    """
    All sites in the rectangle spanned by two corner sites, in raster
    order (top to bottom, left to right), both corners included. The
    corners may be given in any order.

    Args:
        first_corner: One corner of the rectangle.
        second_corner: The opposite corner.

    Returns:
        The sites in the rectangle in raster order.

    Raises:
        std::length_error: if the rectangle exceeds the maximum vector
                           size.
    """

class lattice:
    """
    A hydrogen-passivated silicon surface lattice: a Bravais lattice given
    by two lattice vectors plus a two-site basis. Site `(x, y, z)` lies at
    `x * a1 + y * a2 + basis[z]`. The vectors are stored in ångström, as
    SiQAD stores them; positions are reported in nanometers. The two
    reconstructions SiQAD defines are available as `si_100_2x1()` and
    `si_111_1x1()`; any other reconstruction with a two-site basis can be
    described by setting the vectors directly.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor."""

    @overload
    def __init__(
        self, name: str, a1: tuple[float, float], a2: tuple[float, float], basis: Sequence[tuple[float, float]]
    ) -> None:
        """
        A hydrogen-passivated silicon surface lattice: a Bravais lattice given
        by two lattice vectors plus a two-site basis. Site `(x, y, z)` lies at
        `x * a1 + y * a2 + basis[z]`. The vectors are stored in ångström, as
        SiQAD stores them; positions are reported in nanometers. The two
        reconstructions SiQAD defines are available as `si_100_2x1()` and
        `si_111_1x1()`; any other reconstruction with a two-site basis can be
        described by setting the vectors directly.
        """

    @staticmethod
    def si_100_2x1() -> lattice:
        """
        The H-Si(100)-2x1 surface: dimer rows 7.68 Å apart, dimer atoms 3.84 Å
        apart along the row and 2.25 Å apart across it.

        Returns:
            The H-Si(100)-2x1 lattice.
        """

    @staticmethod
    def si_111_1x1() -> lattice:
        """
        The H-Si(111)-1x1 surface: a hexagonal arrangement with 3.84 Å between
        neighboring sites, described by a rectangular 6.65 Å × 3.84 Å cell
        with a centered second site.

        Returns:
            The H-Si(111)-1x1 lattice.
        """

    @property
    def name(self) -> str:
        """
        Name of the reconstruction as SiQAD spells it in SQD files, e.g.,
        `"Si(100) 2x1"`.
        """

    @name.setter
    def name(self, arg: str, /) -> None: ...
    @property
    def a1(self) -> tuple[float, float]:
        """First lattice vector (unit: Å)."""

    @a1.setter
    def a1(self, arg: tuple[float, float], /) -> None: ...
    @property
    def a2(self) -> tuple[float, float]:
        """Second lattice vector (unit: Å)."""

    @a2.setter
    def a2(self, arg: tuple[float, float], /) -> None: ...
    @property
    def basis(self) -> list[tuple[float, float]]:
        """
        The two basis sites of the unit cell (unit: Å). The first one is the
        origin.
        """

    @basis.setter
    def basis(self, arg: Sequence[tuple[float, float]], /) -> None: ...
    def nm_position(self, site: lattice_site) -> tuple[float, float]:
        """
        The position of a site relative to the site `(0, 0, 0)`.

        Args:
            s: Site.

        Returns:
            The `(x, y)` position of `s` (unit: nm).

        Raises:
            std::out_of_range: if the basis index is not 0 or 1.
        """

    def nm_distance(self, source: lattice_site, target: lattice_site) -> float:
        """
        The Euclidean distance between two sites.

        Args:
            source: First site.
            target: Second site.

        Returns:
            Distance between `source` and `target` (unit: nm).

        Raises:
            std::out_of_range: if either basis index is not 0 or 1.
        """

    def __eq__(self, arg: lattice, /) -> bool:
        """
        Compares two lattices for equality.

        Args:
            other: Right-hand side lattice.

        Returns:
            `true` iff name, vectors, and basis are identical.
        """

    def __ne__(self, arg: lattice, /) -> bool: ...

class sidb_dot_tag(enum.Enum):
    """
    Tags describing the role of a silicon dangling bond. EMPTY denotes an
    unoccupied site.
    """

    EMPTY = 32
    """Unoccupied lattice site."""

    NORMAL = 120
    """Untagged dot."""

    INPUT = 105
    """Primary input dot."""

    OUTPUT = 111
    """Primary output dot."""

    LOGIC = 108
    """Logic dot (e.g., a canvas SiDB)."""

class sidb_layout:
    """
    An SiDB layout: tagged SiDBs and surface defects placed on lattice
    sites of one H-Si lattice. The layout has value semantics; copies are
    independent.

    Dots and defects are kept sorted in raster order, so `sidbs()` is the
    canonical SiDB order that index-based structures such as charge
    distributions refer to, and every `foreach_*` traversal is
    deterministic.
    """

    @overload
    def __init__(self) -> None:
        """Creates an empty layout on the H-Si(100)-2x1 lattice."""

    @overload
    def __init__(self, lattice: lattice, name: str = "") -> None:
        """
        Creates an empty layout on the given lattice.

        Args:
            lat: Lattice of the surface.
            name: Layout name.
        """

    def get_lattice(self) -> lattice:
        """
        The lattice of the surface.

        Returns:
            The lattice.
        """

    def set_lattice(self, lattice: lattice) -> None:
        """
        Reassigns the lattice. Sites are kept as they are; only their physical
        positions change.

        Args:
            lat: New lattice.
        """

    def get_layout_name(self) -> str:
        """
        The layout name.

        Returns:
            Layout name.
        """

    def set_layout_name(self, name: str) -> None:
        """
        Sets the layout name.

        Args:
            name: New layout name.
        """

    def assign_sidb(self, site: lattice_site, dot_tag: sidb_dot_tag = ...) -> None:
        """
        Assigns an SiDB to a lattice site with the given tag, or
        `dot_tag::NORMAL` by default. Reassigning an occupied site updates its
        tag. Assigning `dot_tag::EMPTY` removes the SiDB. Allocation failure
        leaves the dots unchanged.

        Args:
            s: Site.
            tag: Dot tag to assign; defaults to `dot_tag::NORMAL`.
        """

    def get_dot_tag(self, site: lattice_site) -> sidb_dot_tag:
        """
        The dot tag at a site.

        Args:
            s: Site.

        Returns:
            Dot tag at `s`, `dot_tag::EMPTY` if no SiDB is there.
        """

    def is_empty_site(self, site: lattice_site) -> bool:
        """
        Whether no SiDB sits at a site.

        Args:
            s: Site.

        Returns:
            `true` iff `s` holds no SiDB.
        """

    def is_empty(self) -> bool:
        """
        Whether the layout holds no SiDB. Defects do not count.

        Returns:
            `true` iff there is no SiDB.
        """

    def num_dots(self) -> int:
        """
        Number of SiDBs.

        Returns:
            Number of SiDBs.
        """

    def num_dots_with_tag(self, dot_tag: sidb_dot_tag) -> int:
        """
        Number of tagged SiDBs.

        Args:
            tag: Dot tag.

        Returns:
            Number of SiDBs with tag `tag`.
        """

    def dots_with_tag(self, dot_tag: sidb_dot_tag) -> list[lattice_site]:
        """
        All sites holding an SiDB of a given dot tag, in raster order.

        Args:
            tag: Dot tag.

        Returns:
            Sites with tag `tag`.
        """

    def sidbs(self) -> list[lattice_site]:
        """
        All sites holding an SiDB, in raster order. This is the canonical SiDB
        order that index-based structures refer to.

        Returns:
            Sorted SiDB sites.
        """

    def index_of(self, site: lattice_site) -> int | None:
        """
        The index of a site in `sidbs()`.

        Args:
            s: Site.

        Returns:
            Index of `s`, or `std::nullopt` if `s` holds no SiDB.
        """

    def num_pis(self) -> int:
        """
        Number of input SiDBs.

        Returns:
            Number of input SiDBs.
        """

    def num_pos(self) -> int:
        """
        Number of output SiDBs.

        Returns:
            Number of output SiDBs.
        """

    def is_pi(self, site: lattice_site) -> bool:
        """
        Whether a site holds an input SiDB.

        Args:
            s: Site.

        Returns:
            `true` iff `s` holds an input SiDB.
        """

    def is_po(self, site: lattice_site) -> bool:
        """
        Whether a site holds an output SiDB.

        Args:
            s: Site.

        Returns:
            `true` iff `s` holds an output SiDB.
        """

    def pis(self) -> list[lattice_site]:
        """Returns the sites of all input SiDBs."""

    def pos(self) -> list[lattice_site]:
        """Returns the sites of all output SiDBs."""

    def assign_defect(self, site: lattice_site, defect: model.sidb_defect) -> None:
        """
        Places a surface defect at a site. A defect of type
        `defect_type::NONE` removes the defect from the site.

        Args:
            s: Site.
            d: Defect to place.
        """

    def move_defect(self, source: lattice_site, target: lattice_site) -> None:
        """
        Moves the defect at one site to another, replacing whatever defect the
        target site held. An empty source or identical source and target
        leaves the defects unchanged.

        Args:
            source: Site holding the defect.
            target: Site to move the defect to.
        """

    def get_defect(self, site: lattice_site) -> model.sidb_defect:
        """
        The defect at a site.

        Args:
            s: Site.

        Returns:
            Defect at `s`, or a defect of type `defect_type::NONE` if there is
            none.
        """

    def num_defects(self) -> int:
        """
        Number of surface defects.

        Returns:
            Number of defects.
        """

    def num_positively_charged_defects(self) -> int:
        """
        Number of positively charged defects.

        Returns:
            Number of defects with positive charge.
        """

    def num_negatively_charged_defects(self) -> int:
        """
        Number of negatively charged defects.

        Returns:
            Number of defects with negative charge.
        """

    def num_charged_defects(self) -> int:
        """
        Number of charged defects.

        Returns:
            Number of defects with non-zero charge.
        """

    def num_neutral_defects(self) -> int:
        """
        Number of neutral defects.

        Returns:
            Number of defects with zero charge.
        """

    def defects(self) -> list[tuple[lattice_site, model.sidb_defect]]:
        """
        All defects with their sites, in raster order.

        Returns:
            Site-defect pairs.
        """

    def affected_sidbs(
        self,
        site: lattice_site,
        charged_defect_spacing_overwrite: tuple[int, int] | None = None,
        neutral_defect_spacing_overwrite: tuple[int, int] | None = None,
    ) -> set[lattice_site]:
        """
        The sites whose SiDBs the defect at a given site would influence: the
        rectangle around the defect spanned by `model::defect_extent`, counted
        in unit cells along the lattice vectors and keeping the defect's basis
        site. The rectangle is clipped to the representable lattice-site
        coordinates.

        Args:
            s: Site of the defect.
            charged_defect_spacing_overwrite: Horizontal and vertical extent
                                              to use for charged defects
                                              instead of the default.
            neutral_defect_spacing_overwrite: Horizontal and vertical extent
                                              to use for neutral defects
                                              instead of the default.

        Returns:
            Sites influenced by the defect at `s`; empty if `s` holds no
            defect.
        """

    def all_affected_sidbs(
        self,
        charged_defect_spacing_overwrite: tuple[int, int] | None = None,
        neutral_defect_spacing_overwrite: tuple[int, int] | None = None,
    ) -> set[lattice_site]:
        """
        The union of `affected_sidbs` over every defect.

        Args:
            charged_defect_spacing_overwrite: Horizontal and vertical extent
                                              to use for charged defects
                                              instead of the default.
            neutral_defect_spacing_overwrite: Horizontal and vertical extent
                                              to use for neutral defects
                                              instead of the default.

        Returns:
            Sites influenced by any defect.
        """

    def bounding_box(self) -> tuple[lattice_site, lattice_site]:
        """
        The smallest rectangle of sites containing every SiDB and every
        defect. The corners are the north-western and south-eastern site; an
        empty layout yields `{(0,0,0), (0,0,0)}`.

        Returns:
            North-western and south-eastern corner.
        """

    def __eq__(self, arg: sidb_layout, /) -> bool:
        """
        Compares two layouts for equality: same lattice, name, SiDBs, and
        defects.

        Args:
            other: Right-hand side layout.

        Returns:
            `true` iff both layouts are identical.
        """

    def __ne__(self, arg: sidb_layout, /) -> bool: ...
    def __hash__(self) -> int: ...

class charge_distribution:
    """
    A charge distribution assigns one charge state to every SiDB of a
    layout, in the layout's canonical raster order (`layout::sidbs()`),
    and carries the electrostatic potential energy of that assignment. The
    site list is shared by every distribution of one simulation result, so
    a distribution costs one byte per SiDB.
    """

    @overload
    def __init__(self) -> None:
        """Creates a distribution over no SiDBs."""

    @overload
    def __init__(self, layout: sidb_layout, charge_state: model.sidb_charge_state = ...) -> None:
        """
        Creates a distribution over the SiDBs of a layout with every SiDB in
        one charge state.

        Args:
            lyt: Layout whose SiDBs the distribution covers.
            cs: Charge state of every SiDB.
        """

    def sites(self) -> list[lattice_site]:
        """
        The sites the distribution covers, in raster order.

        Returns:
            The sites.
        """

    def size(self) -> int:
        """
        Number of SiDBs.

        Returns:
            Number of SiDBs.
        """

    def empty(self) -> bool:
        """
        Whether the distribution covers no SiDB.

        Returns:
            `true` iff there is no SiDB.
        """

    def index_of(self, site: lattice_site) -> int | None:
        """
        The index of a site in the distribution.

        Args:
            s: Site.

        Returns:
            Index of `s`, or `std::nullopt` if the distribution covers no SiDB
            at `s`.
        """

    def get_charge_state(self, site: lattice_site) -> model.sidb_charge_state:
        """
        The charge state of the SiDB at a site.

        Args:
            s: Site.

        Returns:
            Charge state at `s`, `charge_state::NONE` if the distribution
            covers no SiDB at `s`.
        """

    def get_charge_state_by_index(self, index: int) -> model.sidb_charge_state:
        """
        The charge state of the SiDB at an index.

        Args:
            index: Index in raster order.

        Returns:
            Charge state at `index`, `charge_state::NONE` if the index is out
            of range.
        """

    def assign_charge_state(self, site: lattice_site, charge_state: model.sidb_charge_state) -> None:
        """
        Assigns the charge state of the SiDB at a site. A site the
        distribution does not cover is ignored.

        Args:
            s: Site.
            cs: Charge state to assign.
        """

    def assign_charge_state_by_index(self, index: int, charge_state: model.sidb_charge_state) -> None:
        """
        Assigns the charge state of the SiDB at an index.

        Args:
            index: Index in raster order.
            cs: Charge state to assign.

        Raises:
            std::out_of_range: if the index is out of range.
        """

    def assign_all_charge_states(self, charge_state: model.sidb_charge_state) -> None:
        """
        Assigns one charge state to every SiDB.

        Args:
            cs: Charge state to assign.
        """

    def charge_states(self) -> list[model.sidb_charge_state]:
        """
        All charge states in raster order.

        Returns:
            The charge states.
        """

    def energy(self) -> float:
        """
        The electrostatic potential energy of the distribution.

        Returns:
            Energy (unit: eV).
        """

    def assign_energy(self, energy: float) -> None:
        """
        Sets the electrostatic potential energy of the distribution.

        Args:
            e: Energy (unit: eV).
        """

    def charge_exists(self, charge_state: model.sidb_charge_state) -> bool:
        """
        Whether any SiDB has a given charge state.

        Args:
            cs: Charge state.

        Returns:
            `true` iff at least one SiDB is in state `cs`.
        """

    def num_negative_sidbs(self) -> int:
        """
        Number of negatively charged SiDBs.

        Returns:
            Number of negatively charged SiDBs.
        """

    def num_neutral_sidbs(self) -> int:
        """
        Number of neutrally charged SiDBs.

        Returns:
            Number of neutrally charged SiDBs.
        """

    def num_positive_sidbs(self) -> int:
        """
        Number of positively charged SiDBs.

        Returns:
            Number of positively charged SiDBs.
        """

    def charge_index(self, base: int) -> int:
        """
        The charge index: the distribution read as a number in the given base
        with the first SiDB as the most significant digit and digit `sign + 1`
        per SiDB. The index wraps around past 64 base-2 digits, so it
        identifies distributions uniquely only for layouts of at most 64 (base
        2) or 40 (base 3) SiDBs.

        Args:
            base: Base of the index, 2 or 3.

        Returns:
            The charge index.
        """

    def same_charge_states(self, other: charge_distribution) -> bool:
        """
        Compares two distributions for equal charge states, ignoring the
        energy.

        Args:
            other: Right-hand side distribution.

        Returns:
            `true` iff both assign the same charge states to the same number
            of SiDBs.
        """

    def __eq__(self, arg: charge_distribution, /) -> bool:
        """
        Compares two distributions: same charge states and same energy.

        Args:
            other: Right-hand side distribution.

        Returns:
            `true` iff charge states and energy are equal.
        """

    def __ne__(self, arg: charge_distribution, /) -> bool: ...
    def __len__(self) -> int: ...

    __hash__: None = None
