# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Sphinx configuration for the fiction documentation."""

from __future__ import annotations

import os
import re
import sys
from importlib.metadata import metadata
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from sphinx.application import Sphinx

sys.path.insert(0, str(Path(__file__).parent / "_ext"))

project = "fiction"
copyright = "2018-2023: Marcel Walter. 2023-present: Chair for Design Automation, Technical University of Munich (TUM)"
author = "Marcel Walter, Jan Drewniok, Simon Hofmann, Benjamin Hien, Willem Lambooy"
package_metadata = metadata("mnt-pyfiction")
release = package_metadata["Version"].split("+")[0]
version = release
llms_txt_description = package_metadata["Summary"]

extensions = [
    "breathe",
    "myst_parser",
    "sphinx_copybutton",
    "sphinx_design",
    "sphinx_llm.txt",
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinxcontrib.bibtex",
    "sphinxext.opengraph",
    "nanobind_autodoc",
]
source_suffix = [".md"]
exclude_patterns = ["_build", "doxyxml", "**/AGENTS.md", "AGENTS.md"]
myst_enable_extensions = ["amsmath", "colon_fence", "deflist", "dollarmath", "substitution"]
myst_heading_anchors = 3

bibtex_bibfiles = ["references.bib"]
bibtex_default_style = "unsrt"

ogp_site_url = os.environ.get("READTHEDOCS_CANONICAL_URL", "https://fiction.readthedocs.io/en/latest/")
ogp_image = "_static/mnt_social.png"
ogp_image_alt = "Munich Nanotech Toolkit logo"

breathe_projects = {"fiction": "doxyxml/xml"}
breathe_default_project = "fiction"
autodoc_member_order = "bysource"
add_module_names = False
python_use_unqualified_type_names = True
toc_object_entries_show_parents = "hide"
modindex_common_prefix = ["mnt.pyfiction."]
napoleon_numpy_docstring = False
napoleon_custom_sections = [("Template Args", "params_style")]
intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
}

copybutton_prompt_text = r"(?:\(\.?venv\) )?(?:\[.*\] )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

html_theme = "furo"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
html_js_files = ["tabs.js"]
pygments_style = "colorful"
pygments_dark_style = "monokai"
html_theme_options = {
    "light_logo": "mnt_dark.svg",
    "dark_logo": "mnt_light.svg",
    "source_repository": "https://github.com/cda-tum/fiction/",
    "source_branch": os.environ.get("READTHEDOCS_GIT_COMMIT_HASH", "main"),
    "source_directory": "docs/",
    "navigation_with_keys": True,
    "footer_icons": [
        {"name": "GitHub", "url": "https://github.com/cda-tum/fiction", "html": "GitHub", "class": ""},
        {"name": "PyPI", "url": "https://pypi.org/project/mnt-pyfiction/", "html": "PyPI", "class": ""},
    ],
}


def normalize_llms_sitemap(app: Sphinx, exception: Exception | None) -> None:
    """Keep portable page links without truncated Markdown descriptions."""
    sitemap = Path(app.outdir) / "llms.txt"
    if exception or not sitemap.is_file():
        return
    content = sitemap.read_text(encoding="utf8")
    content = re.sub(
        r"^(- \[.*?\]\()([^)\n]+)\):.*$",
        lambda match: match[1] + match[2].replace("\\", "/") + ")",
        content,
        flags=re.MULTILINE,
    )
    sitemap.write_text(content, encoding="utf8")


def setup(app: Sphinx) -> None:
    """Normalize the sitemap after sphinx-llm finishes exporting pages."""
    app.connect("build-finished", normalize_llms_sitemap, priority=900)
