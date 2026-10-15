# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Render nanobind methods and generated Markdown docstrings with autodoc."""

from __future__ import annotations

import re
from typing import TYPE_CHECKING

from sphinx import addnodes
from sphinx.ext.autodoc import AttributeDocumenter, ClassDocumenter, MethodDocumenter, ModuleDocumenter
from sphinx.util.nodes import find_pending_xref_condition
from typing_extensions import override

if TYPE_CHECKING:
    from docutils import nodes
    from sphinx.application import Sphinx
    from sphinx.config import Config
    from sphinx.ext.autodoc import Documenter, Options
    from sphinx.util.typing import ExtensionMetadata


class NanobindMethodDocumenter(MethodDocumenter):
    """Prefer callable documentation over nanobind's attribute descriptor."""

    objtype = "nanobind_method"
    priority = AttributeDocumenter.priority + 1

    @classmethod
    @override
    def can_document_member(cls, member: object, membername: str, isattr: bool, parent: Documenter) -> bool:
        """Identify bound methods.

        Returns:
            Whether the member is a nanobind method on a class.
        """
        return type(member).__name__ == "nb_method" and not isinstance(parent, ModuleDocumenter)

    @override
    def format_signature(self, **kwargs: object) -> str:
        """Return bound-method overloads without the implicit receiver."""
        return re.sub(r"(?m)^\(self(?:,\s*|(?=\)))", "(", super().format_signature(**kwargs))


class NanobindClassDocumenter(ClassDocumenter):
    """Format constructor overloads exposed through nanobind docstrings."""

    @override
    def format_signature(self, **kwargs: object) -> str:
        """Return constructor overloads without the implicit receiver."""
        if type(self.object).__name__ == "nb_type":
            initializer = self.get_attr(self.object, "__init__")
            docstring = self.get_attr(initializer, "__doc__", "") or ""
            signatures = [
                line.removeprefix("__init__").removesuffix(" -> None")
                for line in docstring.split("\n\n", 1)[0].splitlines()
                if line.startswith("__init__(")
            ]
            if signatures:
                return re.sub(r"(?m)^\(self(?:,\s*|(?=\)))", "(", "\n".join(signatures))
        return super().format_signature(**kwargs)


def _inline_markup(match: re.Match[str]) -> str:
    """Translate inline code while preserving roles and fenced examples.

    Returns:
        RST markup with inline continuations kept out of block-list parsing.
    """
    if protected := match[1]:
        return protected if protected.startswith("```") else re.sub(r"\n\s*", " ", protected)
    literal = re.sub(r"\n\s*", " ", match[2])
    return f"``{literal}``" + ("\\ " + match[3] if match[3] else "")


def prepare_docstring(_app: Sphinx, _what: str, name: str, _obj: object, _options: Options, lines: list[str]) -> None:
    """Translate generated code markup before Napoleon parses Google sections."""
    if not name.startswith("mnt.pyfiction."):
        return
    result = []
    overloaded = "Overloaded function." in lines
    callable_name = "__init__" if type(_obj).__name__ == "nb_type" else name.rsplit(".", 1)[-1]
    in_code = False
    bullet_indent = None
    text = re.sub(
        r"(```[\s\S]*?```|:[\w:]+:`[^`]+`|``[^`]*``)|`([^`]+)`(\w?)",
        _inline_markup,
        "\n".join(lines),
    )
    for line in text.splitlines():
        if line.lstrip().startswith("```"):
            if in_code:
                result.append("")
            else:
                result.extend(["", f".. code-block:: {line.strip()[3:] or 'text'}", ""])
            in_code = not in_code
        elif in_code:
            result.append(f"    {line}")
        else:
            formatted = (
                re.sub(r"^(\d+)\. (?:``)?" + re.escape(callable_name) + r"\(.*$", r"**Overload \1**", line)
                if overloaded
                else line
            )
            bullet = re.match(r"\s*(?:[-*]|\d+[.)]) ", formatted)
            if bullet:
                bullet_indent = len(bullet[0])
                if result and result[-1]:
                    result.append("")
            elif not formatted.strip():
                bullet_indent = None
            elif bullet_indent is not None and len(formatted) - len(formatted.lstrip()) < bullet_indent:
                formatted = " " * bullet_indent + formatted.lstrip()
            result.append(formatted)
    lines[:] = result


def qualify_type_references(_app: Sphinx, doctree: nodes.document) -> None:
    """Keep Python's built-in type distinct from binding properties named type."""
    for signature in doctree.findall(addnodes.desc_signature):
        if not (signature.get("module") or "").startswith("mnt.pyfiction"):
            continue
        for reference in signature.findall(addnodes.pending_xref):
            if (
                reference.get("refdomain") == "py"
                and reference.get("reftype") == "class"
                and reference.get("reftarget") == "type"
            ):
                if content := find_pending_xref_condition(reference, "resolved"):
                    reference[:] = content.children
                reference["intersphinx"] = True
                reference["inventory"] = "python"


def remove_unavailable_sources(app: Sphinx) -> tuple[()]:
    """Keep the source index limited to modules with available source pages.

    Returns:
        No additional HTML pages.
    """
    modules = getattr(app.env, "_viewcode_modules", {})
    for name, entry in list(modules.items()):
        if entry is False:
            del modules[name]
    return ()


def register_documenters(app: Sphinx, _config: Config) -> None:
    """Register nanobind renderers after Sphinx installs its default renderers."""
    app.add_autodocumenter(NanobindMethodDocumenter)
    app.add_autodocumenter(NanobindClassDocumenter, override=True)


def setup(app: Sphinx) -> ExtensionMetadata:
    """Register the bound-method documenter and generated-docstring adapter.

    Returns:
        Parallel-build support for the stateless extension.
    """
    app.connect("config-inited", register_documenters, priority=800)
    app.connect("autodoc-process-docstring", prepare_docstring, priority=400)
    app.connect("doctree-read", qualify_type_references)
    app.connect("html-collect-pages", remove_unavailable_sources, priority=400)
    return {"parallel_read_safe": True, "parallel_write_safe": True}
