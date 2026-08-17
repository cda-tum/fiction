"""Namespace package for the MNT tools.

Windows resolves the DLLs a native extension links against through an explicit search path, so
the directories holding the Z3 libraries are registered here, before anything imports
``mnt.pyfiction``.
"""

from __future__ import annotations

import os
import sys
from pathlib import Path

if sys.platform == "win32":
    if "Z3_ROOT" in os.environ:
        lib_path = Path(os.environ["Z3_ROOT"]) / "lib"
        if lib_path.exists():
            os.add_dll_directory(str(lib_path))
        bin_path = Path(os.environ["Z3_ROOT"]) / "bin"
        if bin_path.exists():
            os.add_dll_directory(str(bin_path))
    if "ALGLIB_DIR" in os.environ:
        os.add_dll_directory(str(os.environ["ALGLIB_DIR"]))
