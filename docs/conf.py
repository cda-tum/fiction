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
import sys
from importlib.metadata import version as package_version
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent / "_ext"))

project = "fiction"
copyright = "2018-2023: Marcel Walter. 2023-present: Chair for Design Automation, Technical University of Munich (TUM)"
author = "Marcel Walter, Jan Drewniok, Simon Hofmann, Benjamin Hien, Willem Lambooy"
release = package_version("mnt-pyfiction").split("+")[0]
version = release
language = "en"

extensions = [
    "breathe",
    "myst_parser",
    "sphinx_copybutton",
    "sphinx_design",
    "sphinx.ext.autodoc",
    "sphinx.ext.intersphinx",
    "sphinx.ext.mathjax",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "nanobind_autodoc",
]
source_suffix = [".md"]
exclude_patterns = ["_build", "_tests", "doxyxml", "**/AGENTS.md", "AGENTS.md", "Thumbs.db", ".DS_Store"]
myst_enable_extensions = ["amsmath", "colon_fence", "deflist", "dollarmath", "substitution"]
myst_heading_anchors = 3
myst_substitutions = {"version": version, "release": release}

breathe_projects = {"fiction": "doxyxml/xml"}
breathe_default_project = "fiction"
autodoc_member_order = "bysource"
add_module_names = False
python_use_unqualified_type_names = True
toc_object_entries_show_parents = "hide"
modindex_common_prefix = ["mnt.pyfiction."]
napoleon_google_docstring = True
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
