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

from sphinx.ext.autodoc import AttributeDocumenter, ClassDocumenter, MethodDocumenter, ModuleDocumenter
from typing_extensions import override

if TYPE_CHECKING:
    from sphinx.application import Sphinx
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


def prepare_docstring(_app: Sphinx, _what: str, name: str, _obj: object, _options: Options, lines: list[str]) -> None:
    """Translate generated code markup before Napoleon parses Google sections."""
    if not name.startswith("mnt.pyfiction."):
        return
    result = []
    in_code = False
    bullet_indent = None
    text = re.sub(
        r"(```[\s\S]*?```|:[\w:]+:`[^`]+`|``[^`]*``)|`([^`]+)`(\w?)",
        lambda match: match[1] or f"``{match[2]}``" + ("\\ " + match[3] if match[3] else ""),
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
            formatted = line
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


def setup(app: Sphinx) -> ExtensionMetadata:
    """Register the bound-method documenter and generated-docstring adapter.

    Returns:
        Parallel-build support for the stateless extension.
    """
    app.add_autodocumenter(NanobindMethodDocumenter)
    app.add_autodocumenter(NanobindClassDocumenter, override=True)
    app.connect("autodoc-process-docstring", prepare_docstring, priority=400)
    return {"parallel_read_safe": True, "parallel_write_safe": True}
