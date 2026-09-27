# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import Defect, DefectType


def test_default_arguments():
    defect = Defect(DefectType.DB)

    assert defect.type == DefectType.DB
    assert defect.charge == 0
    assert defect.epsilon_r == 0.0
    assert defect.lambda_tf == 0.0
