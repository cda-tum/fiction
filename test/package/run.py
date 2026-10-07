# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Build and run a consumer of a relocated fiction installation."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import tempfile
from pathlib import Path


def main() -> None:
    """Install fiction, remove its producer tree, and check SAT equivalence."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cmake-option", action="append", default=[])
    args = parser.parse_args()
    source = Path(__file__).resolve().parents[2]
    scratch = source / ".ai"
    scratch.mkdir(exist_ok=True)

    def run(*command: str | Path, cwd: Path | None = None) -> None:
        """Run a command and stop on failure.

        Args:
            command: Program and arguments.
            cwd: Working directory.
        """
        subprocess.run(list(map(str, command)), cwd=cwd, check=True)

    with tempfile.TemporaryDirectory(prefix="package-", dir=scratch) as temporary:
        work = Path(temporary)
        producer_workspace = tempfile.TemporaryDirectory(prefix="producer-", dir=work)
        producer = Path(producer_workspace.name)
        for directory in ("include", "cmake", "vendors"):
            shutil.copytree(source / directory, producer / directory)
        for name in ("CMakeLists.txt", "CMakePresets.json"):
            shutil.copy2(source / name, producer / name)
        shutil.copytree(source / "test/package", work / "consumer-source")
        prefix = work / "prefix"
        run(
            "cmake",
            "--preset",
            "tests-slim",
            "-DFICTION_TEST=OFF",
            "-DFICTION_ENABLE_CACHE=OFF",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DCMAKE_INSTALL_PREFIX={prefix.as_posix()}",
            *args.cmake_option,
            cwd=producer,
        )
        run("cmake", "--build", "--preset", "tests-slim", "--parallel", "2", cwd=producer)
        run("cmake", "--install", producer / "build-tests-slim")
        relocated = work / "relocated"
        prefix.rename(relocated)
        # TemporaryDirectory handles Git's read-only pack files on Windows.
        producer_workspace.cleanup()

        # Default package locations must not override the co-installed mockturtle.
        shadow = work / "shadow"
        shadow.mkdir()
        (shadow / "mockturtleConfig.cmake").write_text(
            'message(FATAL_ERROR "Selected unrelated mockturtle")\n',
            encoding="utf-8",
        )
        build = work / "consumer-build"
        run(
            "cmake",
            "-S",
            work / "consumer-source",
            "-B",
            build,
            "-G",
            "Ninja",
            "-DCMAKE_BUILD_TYPE=Release",
            f"-Dfiction_DIR={relocated.as_posix()}/lib/cmake/fiction",
            f"-DCMAKE_PREFIX_PATH={shadow.as_posix()}",
            f"-Dmockturtle_DIR={shadow.as_posix()}",
        )
        run("cmake", "--build", build, "--parallel", "2")
        run("ctest", "--test-dir", build, "--output-on-failure")
        config_only = work / "missing-mockturtle/lib/cmake/fiction"
        config_only.mkdir(parents=True)
        shutil.copy2(relocated / "lib/cmake/fiction/fictionConfig.cmake", config_only)
        run(
            "cmake",
            "-S",
            work / "consumer-source",
            "-B",
            work / "missing-dependency-build",
            "-G",
            "Ninja",
            "-DEXPECT_MISSING_MOCKTURTLE=ON",
            f"-Dfiction_DIR={config_only.as_posix()}",
        )


if __name__ == "__main__":
    main()
