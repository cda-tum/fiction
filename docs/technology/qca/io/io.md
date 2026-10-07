# QCA I/O

## QCADesigner

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/qca/io/write_qca_layout.hpp`

```{doxygenstruct} fiction::qca::io::write_qca_layout_params
:members:
```

```{doxygenfunction} fiction::qca::io::write_qca_layout(const qca::layout& lyt, std::ostream& os, write_qca_layout_params ps = {})

```

```{doxygenfunction} fiction::qca::io::write_qca_layout(const qca::layout& lyt, const std::string_view& filename, write_qca_layout_params ps = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.qca.io.write_qca_layout_params
   :members:

.. autofunction:: mnt.pyfiction.qca.io.write_qca_layout
```

:::

::::

## SVG Images

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

```{doxygenclass} fiction::fcn::io::unsupported_cell_type_exception

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.qca.io.write_qca_layout_svg_params
   :members:

.. autofunction:: mnt.pyfiction.qca.io.write_qca_layout_svg
```

:::

::::
