# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical models and parameters of SiDB systems."""

import enum
from collections.abc import Sequence
from typing import overload

class sidb_defect_type(enum.Enum):
    """
    Specifies the types of fabrication defects that can occur on the
    H-Si(100) 2x1 surface according to \\"Atomic defect classification of
    the H–Si(100) surface through multi-mode scanning probe microscopy\\"
    by Jeremiah Croshaw, Thomas Dienel, Taleana Huff, and Robert Wolkow in
    Journal of Nanotechnology in 2020.
    """

    NONE = 0
    """Defect-free H-Si."""

    DB = 1
    """A stray dangling bond."""

    SI_VACANCY = 2
    """A missing silicon atom."""

    SINGLE_DIHYDRIDE = 3
    """Double hydrogen passivation."""

    DIHYDRIDE_PAIR = 4
    """
    A missing bond between dimers that leads to two double hydrogen
    passivations.
    """

    ONE_BY_ONE = 5
    """A collection of dihydride pairs."""

    THREE_BY_ONE = 6
    """A collection of 1 by 1's."""

    SILOXANE = 7
    """An oxidized dimer."""

    RAISED_SI = 8
    """A raised silicon dimer."""

    MISSING_DIMER = 9
    """The dimer is missing altogether."""

    ETCH_PIT = 10
    """A collection of missing dimers."""

    STEP_EDGE = 11
    """A step edge, which is a break in the surface reconstruction."""

    GUNK = 12
    """Residual material."""

    UNKNOWN = 14
    """Unknown defect."""

class sidb_defect:
    """
    In accordance with the paper mentioned above, the `defect` struct is
    used to represent a specific defect on the H-Si(100) 2x1 surface that
    has a charge as well as relative permittivity (`epsilon_r`) and
    Thomas-Fermi screening distance (`lambda_tf`) values associated to it.

    See \\"SiQAD: A Design and Simulation Tool for Atomic Silicon Quantum
    Dot Circuits\\" by S. S. H. Ng, J. Retallick, H. N. Chiu, R. Lupoiu, L.
    Livadaru, T. Huff, M. Rashidi, W. Vine, T. Dienel, R. A. Wolkow, and
    K. Walus in IEEE Transactions on Nanotechnology for more details on
    these values.
    """

    def __init__(
        self,
        defect_type: sidb_defect_type = ...,
        electric_charge: int = 0,
        relative_permittivity: float = 0.0,
        screening_distance: float = 0.0,
    ) -> None: ...
    @property
    def type(self) -> sidb_defect_type:
        """
        Specifies the types of fabrication defects that can occur on the
        H-Si(100) 2x1 surface according to \\"Atomic defect classification of
        the H–Si(100) surface through multi-mode scanning probe microscopy\\"
        by Jeremiah Croshaw, Thomas Dienel, Taleana Huff, and Robert Wolkow in
        Journal of Nanotechnology in 2020.
        """

    @property
    def charge(self) -> int:
        """
        Electrical charge in units of the elementary charge e (e.g., 1 ^= 1*e,
        -2 ^= -2*e).
        """

    @property
    def epsilon_r(self) -> float:
        """Electric permittivity (unitless)."""

    @property
    def lambda_tf(self) -> float:
        """Thomas-Fermi screening distance in nm."""

    def __eq__(self, rhs: sidb_defect) -> bool:
        """
        This operator compares two `defect` instances for equality. It checks
        if the `type`, `charge`, `epsilon_r`, and `lambda_tf` members of the
        two instances are equal.

        Args:
            rhs: `defect` instance to compare against.
        """

    def __ne__(self, rhs: sidb_defect) -> bool:
        """
        This operator compares two `sidb_defect` instances for inequality. It uses the `operator==` to check if the two instances are equal and returns the negation of the result.

        Parameter ``rhs``:
            `sidb_defect` instance to compare against.
        """

def is_charged_defect_type(defect: sidb_defect) -> bool:
    """
    Checks whether the given defect type is a charged one. `DB`,
    `SI_VACANCY` and `ARSENIC` types are charged. Those charged defects
    are to be avoided by a larger distance.

    Args:
        defect: Defect to check.

    Returns:
        `true` iff `defect` is of a charged type.
    """

def is_neutral_defect_type(defect: sidb_defect) -> bool:
    """
    Checks whether the given defect type is not a charged one. Neutral
    defects are to be avoided as well, but not by such a large distance.
    Even though the `NONE` defect type is technically neutral, it is not a
    defect per se which is why this function returns false on the `NONE`
    defect input.

    Args:
        defect: Defect to check.

    Returns:
        `true` iff `defect` is not of a charged type.
    """

def is_positively_charged_defect(defect: sidb_defect) -> bool:
    """
    Checks whether the given defect has a positive charge value assigned
    to it. This function is irrespective of the associated defect type.

    Args:
        defect: Defect to check.

    Returns:
        `true` iff `defect` has a positive charge value.
    """

def is_negatively_charged_defect(defect: sidb_defect) -> bool:
    """
    Checks whether the given defect has a negative charge value assigned
    to it. This function is irrespective of the associated defect type.

    Args:
        defect: Defect to check.

    Returns:
        `true` iff `defect` has a negative charge value.
    """

def is_neutrally_charged_defect(defect: sidb_defect) -> bool:
    """
    Checks whether the given defect has a neutral charge value, i.e., `0`,
    assigned to it. This function is irrespective of the associated defect
    type.

    Args:
        defect: Defect to check.

    Returns:
        `true` iff `defect` has a neutral charge value.
    """

def defect_extent(
    defect: sidb_defect,
    charged_defect_spacing_overwrite: tuple[int, int] | None,
    neutral_defect_spacing_overwrite: tuple[int, int] | None,
) -> tuple[int, int]:
    """
    Returns the extent of a defect as a pair of SiDB distances in the
    horizontal and vertical directions. If the defect type is `NONE`, `{0,
    0}` is returned.

    Args:
        defect: Defect type to evaluate.
        charged_defect_spacing_overwrite: Override the default influence
                                          distance of charged atomic
                                          defects on SiDBs with an
                                          optional pair of horizontal and
                                          vertical distances.
        neutral_defect_spacing_overwrite: Override the default influence
                                          distance of neutral atomic
                                          defects on SiDBs with an
                                          optional pair of horizontal and
                                          vertical distances.

    Returns:
        A pair of uint16_t values representing the number of horizontal
        and vertical SiDBs affected by the given defect type.
    """

class sidb_charge_state(enum.Enum):
    """Charge states of SiDBs."""

    NEGATIVE = -1

    NEUTRAL = 0

    POSITIVE = 1

    NONE = 127

SIDB_CHARGE_STATES_BASE_2: list = ...

SIDB_CHARGE_STATES_BASE_3: list = ...

def sidb_charge_states_for_base_number(base: int) -> list[sidb_charge_state]:
    """
    Charge states of SiDBs for a given simulation base number. The full
    base states are returned for an invalid simulation base.

    Args:
        base: The simulation base number to get the associated SiDB charge
              states for.

    Returns:
        NEG, NEUT, POS for base 3 (full base), and NEG, NEUT otherwise,
        associated with base 2 simulation.
    """

def charge_state_to_sign(cs: sidb_charge_state) -> int:
    """
    Converts the charge state into an integer (`-1`, `0`, `1`).

    Args:
        cs: SiDB charge state.

    Returns:
        Integer representing the SiDB's charge state.
    """

def sign_to_charge_state(sg: int) -> sidb_charge_state:
    """
    Converts an integer (`-1`, `0`, `1`) into a charge state.

    Args:
        sg: Integer (`-1`, `0`, `1`) representing a charge state.

    Returns:
        charge_state representation of `sg`.
    """

def charge_configuration_to_string(cc: Sequence[sidb_charge_state]) -> str:
    """
    Converts a vector of charge states to a string representation
    (`"-101..."`).

    Args:
        charge_distribution: A vector of SiDBs charge states.

    Returns:
        A string representation of the charge states.
    """

class sidb_simulation_parameters:
    """
    This struct collects all physical parameters for physical SiDB
    simulations. It can be useful to adjust them, especially when
    experiments create new insights. However, the default values are
    commonly used.
    """

    @overload
    def __init__(
        self,
        base_number: int = 3,
        mu_minus: float = -0.32,
        relative_permittivity: float = 5.6,
        screening_distance: float = 5.0,
    ) -> None: ...
    @overload
    def __init__(self) -> None:
        """
        Default constructor.

        Args:
            base_number: simulation can be conducted with 2 and 3 charge
                         states. 2 = (Negative, Neutral), 3 = (Negative,
                         Neutral, Positive).
            mu: (µ-) is the energy transition level (0/-) in eV.
            relative_permittivity: it describes the electric field reduction
                                   due to polarization.
            screening_distance: also known as "Thomas-Fermi screening" and it
                                describes the electric field screening due to
                                free charges in nm.
        """

    @property
    def epsilon_r(self) -> float:
        """
        `epsilon_r` is the electric permittivity. It is a material specific
        number (unit-less).
        """

    @epsilon_r.setter
    def epsilon_r(self, arg: float, /) -> None: ...
    @property
    def lambda_tf(self) -> float:
        """`lambda_tf` is the Thomas-Fermi screening distance (unit: nm)."""

    @lambda_tf.setter
    def lambda_tf(self, arg: float, /) -> None: ...
    @property
    def mu_minus(self) -> float:
        """`mu_minus` (µ-) is the energy transition level (0/-) (unit: eV)."""

    @mu_minus.setter
    def mu_minus(self, arg: float, /) -> None: ...
    @property
    def base(self) -> int:
        """
        `base` can be either 2 or 3 and describes the assumed number of charge
        states of one SiDB. It often makes sense to assume only negatively and
        neutrally charged SiDBs.
        """

    @base.setter
    def base(self, arg: int, /) -> None: ...
    def k(self) -> float:
        """
        `k` is the Coulomb constant `K_E` divided by `epsilon_r` (unit:
        :math:`N \\cdot m^{2} \\cdot C^{-2}`).
        """

    def mu_plus(self) -> float:
        """`mu_plus` (µ+) is the energy transition level (+/0) (unit: eV)."""

def potential_to_distance_conversion(potential: float, params: sidb_simulation_parameters, precision: int) -> float:
    """
    The electrostatic potential on hydrogen-passivated silicon is
    typically modeled using a screened Coulomb potential. This
    electrostatic potential is commonly employed to determine the
    electrostatic potential for a given distance (between SiDB and point
    under consideration) and given physical parameters. However, the
    function provided here serves the inverse purpose by calculating the
    distance for a given potential and given physical parameters.

    Args:
        params: The physical parameters for a given hydrogen-passivated
                silicon surface.
        potential: The electrostatic potential (unit: V) to be converted
                   to a distance.
        precision: The precision level for the conversion, specifying the
                   number of decimal places.

    Returns:
        The distance (unit: nm) corresponding to the given electrostatic
        potential.

    Note:
        Runtime depends exponentially on the provided precision.
    """
