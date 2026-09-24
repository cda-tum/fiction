# molQCA I/O

## SVG Images

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/mol_qca/io/write_mol_qca_layout_svg.hpp`

```{doxygenstruct} fiction::mol_qca::io::write_mol_qca_layout_svg_params
:members:
```

```{doxygenfunction} fiction::mol_qca::io::write_mol_qca_layout_svg(const mol_qca::layout& lyt, std::ostream& os, const write_mol_qca_layout_svg_params& ps = {})

```

```{doxygenfunction} fiction::mol_qca::io::write_mol_qca_layout_svg(const mol_qca::layout& lyt, const std::string_view& filename, const write_mol_qca_layout_svg_params& ps = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.write_mol_qca_layout_svg_params
   :members:

.. autofunction:: mnt.pyfiction.write_mol_qca_layout_svg
```

:::

::::
