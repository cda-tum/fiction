import glob
import os
import sys
from os.path import basename, dirname, isfile, join
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

modules = glob.glob(join(dirname(__file__), "*.py"))
__all__ = [basename(f)[:-3] for f in modules if isfile(f) and not f.endswith("__init__.py")]
