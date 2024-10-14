import os
import sys
from pathlib import Path

if sys.platform == "win32" and sys.version_info > (3, 8, 0):
    if "Z3_ROOT" in os.environ:
        lib_path = Path(os.environ["Z3_ROOT"]) / "lib"
        if lib_path.exists():
            os.add_dll_directory(str(lib_path))
        bin_path = Path(os.environ["Z3_ROOT"]) / "bin"
        if bin_path.exists():
            os.add_dll_directory(str(bin_path))
    if "ALGLIB_DIR" in os.environ:
        os.add_dll_directory(str(os.environ["ALGLIB_DIR"]))
