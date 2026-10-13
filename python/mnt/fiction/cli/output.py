# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Atomic staging for CLI output files."""

from __future__ import annotations

import tempfile
from contextlib import contextmanager
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Generator


@contextmanager
def atomic_output(path: Path) -> Generator[Path, None, None]:
    """Stage an output in a sibling temporary file, then replace the destination.

    The destination changes only when the body completes. Existing symbolic links retain their link
    and receive the output through their target.

    Args:
        path: The destination.

    Yields:
        The temporary path to write.
    """
    destination = path.resolve(strict=True) if path.is_symlink() else path
    with tempfile.TemporaryDirectory(prefix=".fiction-", dir=destination.parent) as directory:
        temporary = Path(directory) / destination.name
        yield temporary  # ruff: ignore[fallible-context-manager] -- a failed body skips the replacement
        if destination.exists():
            temporary.chmod(destination.stat().st_mode)
        temporary.replace(destination)
