# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Public metadata supplied by the native build."""

from __future__ import annotations

from time import strptime

import pytest

from mnt import pyfiction
from mnt.pyfiction import utils


def test_version_and_repository() -> None:
    """The package and native module share the project version and repository."""
    assert pyfiction.__version__ == utils.__version__
    assert utils.__version__.startswith("fiction v")
    assert utils.__version__.removeprefix("fiction v")
    assert utils.__repo__ == "https://github.com/cda-tum/fiction"


@pytest.mark.parametrize(
    ("attribute", "date_format"),
    [("__compiled_date__", "%b %d %Y"), ("__compiled_time__", "%H:%M:%S")],
    ids=["date", "time"],
)
def test_build_timestamp(attribute: str, date_format: str) -> None:
    """Build metadata contains the compiler's date and time strings.

    Args:
        attribute: Exported build timestamp field.
        date_format: Format of the compiler macro supplying the field.
    """
    value = getattr(utils, attribute)
    assert isinstance(value, str)
    strptime(value, date_format)
