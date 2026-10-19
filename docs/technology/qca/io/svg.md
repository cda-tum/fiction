# SVG Images

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/write_qca_layout_svg.hpp`

```{doxygenstruct} fiction::qca::io::write_qca_layout_svg_params
:members:
```

```{doxygenfunction} fiction::qca::io::write_qca_layout_svg(const qca::layout& lyt, std::ostream& os, const write_qca_layout_svg_params& ps = {})

```

```{doxygenfunction} fiction::qca::io::write_qca_layout_svg(const qca::layout& lyt, const std::string_view& filename, const write_qca_layout_svg_params& ps = {})

```

Unsupported cell types raise {cpp:class}`fiction::fcn::io::unsupported_cell_type_exception`.

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.qca.io.write_qca_layout_svg
```

:::

::::
