# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import sidb_defect, sidb_defect_type


def test_default_arguments():
    defect = sidb_defect(sidb_defect_type.DB)

    assert defect.type == sidb_defect_type.DB
    assert defect.charge == 0
    assert defect.epsilon_r == 0.0
    assert defect.lambda_tf == 0.0
