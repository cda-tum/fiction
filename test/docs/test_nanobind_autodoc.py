# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Exercise documentation adapters through Sphinx's HTML and Markdown writers."""

from __future__ import annotations

import html
import re
import subprocess
import sys
from pathlib import Path

import pytest


@pytest.mark.parametrize(("builder", "literal_signatures"), [("html", False), ("llms-markdown", True)])
def test_nanobind_rendering(tmp_path: Path, builder: str, *, literal_signatures: bool) -> None:
    """Keep overload descriptions, markup, defaults, and exporter node contents."""
    source = tmp_path / "source"
    source.mkdir()
    extension = Path(__file__).resolve().parents[2] / "docs" / "_ext"
    (source / "conf.py").write_text(
        f"import sys\nsys.path[:0] = [{str(extension)!r}, {str(source)!r}]\n"
        "import importlib.util\n"
        f"for name, path in [('mnt', {str(source / 'mnt' / '__init__.py')!r}), "
        f"('mnt.pyfiction', {str(source / 'mnt' / 'pyfiction' / '__init__.py')!r})]:\n"
        "    spec = importlib.util.spec_from_file_location(name, path)\n"
        "    module = importlib.util.module_from_spec(spec)\n"
        "    sys.modules[name] = module\n"
        "    spec.loader.exec_module(module)\n"
        "extensions = ['sphinx.ext.autodoc', 'sphinx.ext.napoleon', 'sphinx_llm.txt', "
        "'nanobind_autodoc', 'llm_markdown']\n"
        "autodoc_use_legacy_class_based = True\n"
        "llms_txt_enabled = False\n",
        encoding="utf-8",
    )
    package = source / "mnt" / "pyfiction"
    package.mkdir(parents=True)
    (package.parent / "__init__.py").write_text("", encoding="utf-8")
    (package / "__init__.py").write_text(
        'def route():\n    """route(layout: int, *, limit: int = 42, kind: type[int] = int) -> int\n'
        "route(layout: str, *, limit: int = 42, kind: type[str] = str) -> str\n\n"
        "Overloaded function.\n\n"
        "1. ``route(layout: int, *, limit: int = 42, kind: type[int] = int) -> int``\n\n"
        "Integer overload prose.\n\n"
        "- Keep `first` item.\n"
        "- Keep second item.\n\n"
        "```python\nroute(1, limit=42)\n```\n\n"
        "2. ``route(layout: str, *, limit: int = 42, kind: type[str] = str) -> str``\n\n"
        "String overload prose with :class:`sidb_defect`.\n"
        '    """\n',
        encoding="utf-8",
    )
    if not literal_signatures:
        docstring = package / "__init__.py"
        docstring.write_text(
            re.sub(r"(?m)^(\d+\. )``(.*)``$", r"\1\2", docstring.read_text(encoding="utf-8")), encoding="utf-8"
        )
    # The adapter only handles the public binding namespace.
    (source / "index.rst").write_text(
        "Rendering\n=========\n\n"
        ".. py:class:: sidb_defect\n\n   .. py:property:: type\n\n"
        ".. py:class:: bdl_pair\n\n   .. py:property:: type\n\n"
        ".. automodule:: mnt.pyfiction\n\n"
        ".. autofunction:: mnt.pyfiction.route\n\n"
        ".. py:function:: keyword(*, value: int = 42)\n\n"
        ".. py:attribute:: setting\n   :type: int\n\n"
        "   Setting prose.\n\n"
        "Inline expression :cpp:expr:`limit + 1`.\n\n"
        "    Quoted prose.\n\n    -- Attribution survives\n\n"
        "Citation [source]_.\n\n.. [source] Citation body survives.\n",
        encoding="utf-8",
    )
    command = [
        sys.executable,
        "-m",
        "sphinx",
        "-b",
        builder,
        "-t",
        "sphinx_llm_markdown",
        "-W",
        str(source),
        str(tmp_path / "out"),
    ]
    result = subprocess.run(command, capture_output=True, text=True, check=False)
    assert result.returncode == 0, result.stdout + result.stderr
    output = (tmp_path / "out" / ("index.html" if builder == "html" else "index.md")).read_text(encoding="utf-8")
    text = html.unescape(re.sub(r"<[^>]+>", "", output)) if builder == "html" else output
    assert "Integer overload prose" in text
    assert "String overload prose" in text
    assert "first" in text
    assert "second item" in text
    assert "route(1, limit=42)" in text
    assert "limit: int = 42" in text
    assert "type[int] = int" in text
    assert "type[str] = str" in text
    assert "limit + 1" in text
    assert "Attribution survives" in text
    assert "Citation body survives" in text
    assert "Overload 1" in text
    assert "Overload 2" in text
    assert "1. `route(" not in text
    assert "2. `route(" not in text
    assert text.count("layout:") == 2
    if builder == "html":
        assert 'href="#sidb_defect"' in output
        assert "<ul>" in output
        assert 'class="highlight-python' in output
        assert "*," in text
    else:
        assert "[`sidb_defect`](sphinx-llm:" in output
        assert "- Keep `first` item." in output
        assert "```python\nroute(1, limit=42)\n```" in output
        assert r"\*, limit" in output
