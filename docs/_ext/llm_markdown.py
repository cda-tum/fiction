# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Preserve documentation nodes unsupported by the Markdown exporter."""

from __future__ import annotations

from typing import TYPE_CHECKING

from sphinx_llm.markdown_builder import SphinxLlmMarkdownTranslator

if TYPE_CHECKING:
    from docutils import nodes
    from sphinx.application import Sphinx
    from sphinx.util.typing import ExtensionMetadata


class FictionMarkdownTranslator(SphinxLlmMarkdownTranslator):
    """Render inline syntax, quote authors, and bibliography entries."""

    def visit_abbreviation(self, _node: nodes.Element) -> None:
        """Keep abbreviation text, including Python's keyword-only marker."""

    def visit_desc_inline(self, _node: nodes.Element) -> None:
        """Keep inline C++ expressions and their child links."""

    visit_attribution = SphinxLlmMarkdownTranslator.visit_paragraph
    visit_citation = SphinxLlmMarkdownTranslator.visit_footnote


def setup(app: Sphinx) -> ExtensionMetadata:
    """Install the Markdown writer without changing HTML rendering.

    Returns:
        Parallel-build support for the stateless translator.
    """
    app.set_translator("llms-markdown", FictionMarkdownTranslator, override=True)
    return {"parallel_read_safe": True, "parallel_write_safe": True}
