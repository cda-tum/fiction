"""Test package for the ``pyfiction`` bindings.

Windows resolves the DLLs a native extension links against through an explicit search path,
so the directories holding the Z3 and ALGLIB libraries have to be registered before anything
imports ``mnt.pyfiction``. Importing any test module imports this package first, which makes
this the one place that runs early enough.
"""

from __future__ import annotations

import os
import sys
from pathlib import Path

if sys.platform == "win32":
    if "Z3_ROOT" in os.environ:
        for subdirectory in ("lib", "bin"):
            z3_path = Path(os.environ["Z3_ROOT"]) / subdirectory
            if z3_path.exists():
                os.add_dll_directory(str(z3_path))
    if "ALGLIB_DIR" in os.environ:
        os.add_dll_directory(os.environ["ALGLIB_DIR"])
