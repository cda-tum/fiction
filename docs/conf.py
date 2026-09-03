# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Sphinx configuration for the fiction documentation."""

from __future__ import annotations

import xml.etree.ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] - parses the docs build's own Doxygen output
from typing import TYPE_CHECKING, ClassVar

from docutils import nodes
from docutils.parsers.rst import Directive

if TYPE_CHECKING:
    from sphinx.application import Sphinx

project = "fiction"
copyright = "2018-2023: Marcel Walter. 2023-present: Chair for Design Automation, Technical University of Munich (TUM)"
author = "Marcel Walter, Jan Drewniok, Simon Hofmann, Benjamin Hien, Willem Lambooy"
version = "v0.8.0"
release = version
language = "en"

extensions = [
    "breathe",
    "myst_parser",
    "sphinx_copybutton",
    "sphinx_design",
    "sphinx.ext.autodoc",
    "sphinx.ext.mathjax",
    "sphinx.ext.viewcode",
]
source_suffix = [".md"]
exclude_patterns = ["_build", "doxyxml", "**/AGENTS.md", "AGENTS.md", "Thumbs.db", ".DS_Store"]
myst_enable_extensions = ["amsmath", "colon_fence", "deflist", "dollarmath", "substitution"]
myst_heading_anchors = 3
myst_substitutions = {"version": version, "release": release}

breathe_projects = {"fiction": "doxyxml/xml"}
breathe_default_project = "fiction"
autodoc_member_order = "bysource"

copybutton_prompt_text = r"(?:\(\.?venv\) )?(?:\[.*\] )?\$ "
copybutton_prompt_is_regexp = True
copybutton_line_continuation_character = "\\"

html_theme = "furo"
html_static_path = ["_static"]
pygments_style = "colorful"
html_theme_options = {
    "light_logo": "mnt_dark.svg",
    "dark_logo": "mnt_light.svg",
    "source_repository": "https://github.com/cda-tum/fiction/",
    "source_branch": "main",
    "source_directory": "docs/",
    "navigation_with_keys": True,
    "footer_icons": [
        {"name": "GitHub", "url": "https://github.com/cda-tum/fiction", "html": "GitHub", "class": ""},
        {"name": "PyPI", "url": "https://pypi.org/project/mnt-pyfiction/", "html": "PyPI", "class": ""},
    ],
}


class DocOverviewTableDirective(Directive):
    """Renders a two-column table of the functions a Doxygen compound documents."""

    has_content = True
    required_arguments = 1
    option_spec: ClassVar = {"column": str}

    def run(self) -> list[nodes.Node]:
        """Build the table from the Doxygen XML the docs build produced.

        Returns:
            The table node, as the single element of the directive's node list.
        """
        # The XML comes from the documentation build's Doxygen run.
        doc = ET.parse(f"doxyxml/xml/{self.arguments[0]}.xml")  # ruff: ignore[suspicious-xml-element-tree-usage]

        table = nodes.table()
        tgroup = nodes.tgroup(cols=2)

        tgroup += nodes.colspec(colwidth=50)
        tgroup += nodes.colspec(colwidth=50)

        # header
        colname = self.options.get("column", "Function")
        tgroup += nodes.thead(
            "",
            nodes.row("", *[nodes.entry("", nodes.line(text=c)) for c in [colname, "Description"]]),
        )

        # rows
        tbody = nodes.tbody()
        for target in self.content:
            for elem in doc.findall(f"./compounddef/sectiondef/memberdef/[name='{target}']"):
                ref = nodes.reference("", target, internal=True)
                ref["refuri"] = "#{}".format(elem.attrib["id"])

                reft = nodes.paragraph()
                reft.extend([ref])

                func = nodes.entry("", reft)
                desc = nodes.entry("", nodes.line(text=elem.findtext("./briefdescription/para", default="")))

                tbody += nodes.row("", func, desc)

        tgroup += tbody
        table += tgroup
        return [table]


def setup(app: Sphinx) -> None:
    """Register the custom directives with Sphinx.

    Args:
        app: The Sphinx application to register with.
    """
    app.add_directive("doc_overview_table", DocOverviewTableDirective)
