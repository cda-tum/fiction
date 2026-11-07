# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Public metadata supplied by the native build."""

from __future__ import annotations

from datetime import date
from time import strptime

from mnt import pyfiction
from mnt.pyfiction import utils


def test_version_and_repository() -> None:
    """The package and native module share the project version and repository."""
    assert pyfiction.__version__ == utils.__version__
    assert utils.__version__.startswith("fiction v")
    assert utils.__version__.removeprefix("fiction v")
    assert utils.__repo__ == "https://github.com/cda-tum/fiction"


def test_build_date() -> None:
    """The compiler date contains an English month and a valid calendar date."""
    month, day, year = utils.__compiled_date__.split()
    months = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]
    date(int(year), months.index(month) + 1, int(day))


def test_build_time() -> None:
    """The compiler time contains valid hours, minutes, and seconds."""
    strptime(utils.__compiled_time__, "%H:%M:%S")
