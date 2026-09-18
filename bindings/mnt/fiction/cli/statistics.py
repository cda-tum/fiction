# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Convert native statistics and command results into JSON values."""

from __future__ import annotations

import contextlib
import datetime
import json
import math
from enum import Enum
from pathlib import Path


def stats_to_dict(stats: object) -> dict[str, object]:
    """Convert a statistics object of the bindings into JSON-ready values.

    Every public, non-callable attribute becomes a key. Durations become float seconds under the
    attribute name with an ``_s`` suffix, enumerators become their names, and nested statistics
    objects recurse.

    Args:
        stats: A ``*_stats`` object of ``mnt.pyfiction``.

    Returns:
        The attributes as a dictionary; members whose C++ type has no caster are named under
        ``_unsupported`` instead of being dropped silently.
    """
    result: dict[str, object] = {}
    unsupported: list[str] = []
    for name in dir(stats):
        if name.startswith("_"):
            continue
        try:
            value = getattr(stats, name)
        except TypeError:
            # a member whose C++ type has no caster; naming it here makes the missing one visible
            unsupported.append(name)
            continue
        if callable(value):
            continue
        if isinstance(value, datetime.timedelta):
            result[f"{name}_s"] = value.total_seconds()
        else:
            if name == "report" and isinstance(value, str):
                with contextlib.suppress(ValueError):
                    value = json.loads(value)
            result[name] = json_value(value)
    if unsupported:
        result["_unsupported"] = unsupported
    return result


def json_value(value: object) -> object:
    """Convert one binding value into a JSON-ready value.

    Args:
        value: A scalar, an enumerator, a list, a path, or a statistics object.

    Returns:
        The value as JSON accepts it.
    """
    if isinstance(value, dict):
        return {str(key): json_value(item) for key, item in value.items()}
    if isinstance(value, float) and not math.isfinite(value):
        return None
    if isinstance(value, bool | int | float | str) or value is None:
        return value
    if isinstance(value, Enum):
        return value.name
    if isinstance(value, datetime.timedelta):
        return value.total_seconds()
    if isinstance(value, Path):
        return str(value)
    if isinstance(value, list | tuple):
        return [json_value(item) for item in value]
    if type(value).__module__.startswith("mnt.pyfiction"):
        return stats_to_dict(value)
    return str(value)
