# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Silicon Dangling Bond (SiDB) lattices, layouts, charge distributions, and simulation."""

from __future__ import annotations

from mnt.pyfiction._area import sidb_area as area
from mnt.pyfiction._native.sidb import (
    ChargeDistribution,
    DotTag,
    Lattice,
    LatticeSite,
    SiDBLayout,
    row_of,
    site_at_row,
    sites_in_area,
)
from mnt.pyfiction._native.sidb.model import (
    SIDB_CHARGE_STATES_BASE_2,
    SIDB_CHARGE_STATES_BASE_3,
    ChargeState,
    Defect,
    DefectType,
    SimulationParams,
    charge_configuration_to_string,
    charge_state_to_sign,
    defect_extent,
    is_charged_defect_type,
    is_negatively_charged_defect,
    is_neutral_defect_type,
    is_neutrally_charged_defect,
    is_positively_charged_defect,
    potential_to_distance_conversion,
    sidb_charge_states_for_base_number,
    sign_to_charge_state,
)

from . import analysis, design, io, simulation

__all__ = [
    "SIDB_CHARGE_STATES_BASE_2",
    "SIDB_CHARGE_STATES_BASE_3",
    "ChargeDistribution",
    "ChargeState",
    "Defect",
    "DefectType",
    "DotTag",
    "Lattice",
    "LatticeSite",
    "SiDBLayout",
    "SimulationParams",
    "analysis",
    "area",
    "charge_configuration_to_string",
    "charge_state_to_sign",
    "defect_extent",
    "design",
    "io",
    "is_charged_defect_type",
    "is_negatively_charged_defect",
    "is_neutral_defect_type",
    "is_neutrally_charged_defect",
    "is_positively_charged_defect",
    "potential_to_distance_conversion",
    "row_of",
    "sidb_charge_states_for_base_number",
    "sign_to_charge_state",
    "simulation",
    "site_at_row",
    "sites_in_area",
]
