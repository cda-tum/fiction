# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check rendered signatures and markup using the installed Python bindings."""

from __future__ import annotations

import io
import tempfile
import unittest
from pathlib import Path

from sphinx.application import Sphinx


class AutodocTests(unittest.TestCase):
    """Exercise the generated documentation through Sphinx's HTML builder."""

    def test_binding_documentation(self) -> None:
        """Methods, overloads, code and parameter descriptions remain readable."""
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            extension = Path(__file__).parents[1] / "_ext"
            (root / "conf.py").write_text(
                f"import sys\nsys.path.insert(0, {str(extension)!r})\n"
                "extensions = ['sphinx.ext.autodoc', 'sphinx.ext.napoleon', 'nanobind_autodoc']\n"
                "napoleon_custom_sections = [('Template Args', 'params_style')]\n",
                encoding="utf8",
            )
            (root / "index.rst").write_text(
                "Bindings\n========\n\n"
                ".. autoclass:: mnt.pyfiction.cartesian_layout\n   :members: above\n\n"
                ".. autofunction:: mnt.pyfiction.to_siqad_coord\n\n"
                ".. autoexception:: mnt.pyfiction.missing_required_gates_exception\n\n"
                ".. autoclass:: mnt.pyfiction.cartesian_gate_layout\n\n"
                ".. autofunction:: mnt.pyfiction.clustercomplete\n\n"
                ".. autofunction:: mnt.pyfiction.write_operational_domain\n\n"
                ".. autofunction:: mnt.pyfiction.critical_path_length_and_throughput\n\n"
                ".. autofunction:: mnt.pyfiction.critical_temperature_non_gate_based\n\n"
                ".. autofunction:: mnt.pyfiction.design_sidb_gates\n",
                encoding="utf8",
            )
            warnings = io.StringIO()
            app = Sphinx(root, root, root / "html", root / "doctrees", "html", status=io.StringIO(), warning=warnings)
            app.build()
            output = (root / "html/index.html").read_text(encoding="utf8")
            self.assertEqual(warnings.getvalue(), "")
            self.assertIn('class="py method"', output)
            self.assertIn('id="mnt.pyfiction.cartesian_layout.above"', output)
            self.assertIn('id="mnt.pyfiction.to_siqad_coord"', output)
            self.assertIn('id="mnt.pyfiction.missing_required_gates_exception"', output)
            self.assertIn('class="sig-param"', output)
            self.assertNotIn("**kwargs", output)
            self.assertNotIn('<span class="pre">self</span>', output)
            self.assertNotIn('class="problematic"', output)
            self.assertIn('<span class="pre">mockturtle</span>', output)


if __name__ == "__main__":
    unittest.main()
