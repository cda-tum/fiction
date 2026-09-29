# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical simulation engines for SiDB layouts."""

from __future__ import annotations

from mnt.pyfiction._native.sidb.simulation.engines import (
    automatic_base_number_detection,
    exhaustive_ground_state_simulation,
    quickexact,
    quickexact_params,
    quicksim,
    quicksim_params,
)

__all__ = [
    "automatic_base_number_detection",
    "exhaustive_ground_state_simulation",
    "quickexact",
    "quickexact_params",
    "quicksim",
    "quicksim_params",
]

try:
    from mnt.pyfiction._native.sidb.simulation.engines import (
        clustercomplete,
        clustercomplete_params,
        ground_state_space_reporting,
    )
except ImportError:
    pass
else:
    # CodeQL does not follow `__all__ +=` into this branch and reports the imports as unused. Reading
    # the names here marks them as used; ClusterComplete needs ALGLIB, so the names exist only in builds with it.
    _clustercomplete_exports = (clustercomplete, clustercomplete_params, ground_state_space_reporting)
    __all__ += [
        "clustercomplete",
        "clustercomplete_params",
        "ground_state_space_reporting",
    ]
