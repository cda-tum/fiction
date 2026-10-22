# SVG Images

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/write_sidb_layout_svg.hpp`

```{doxygenstruct} fiction::sidb::io::write_sidb_layout_svg_params
:members:
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, std::ostream& os, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const std::string_view& filename, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const charge_distribution& cd, std::ostream& os, const write_sidb_layout_svg_params& ps = {})
```

```{doxygenfunction} fiction::sidb::io::write_sidb_layout_svg(const layout& lyt, const charge_distribution& cd, const std::string_view& filename, const write_sidb_layout_svg_params& ps = {})
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}

.. autoclass:: mnt.pyfiction.sidb.io.sidb_lattice_mode
   :members:

.. autoclass:: mnt.pyfiction.sidb.io.color_mode
   :members:

.. autofunction:: mnt.pyfiction.sidb.io.write_sidb_layout_svg

.. autofunction:: mnt.pyfiction.sidb.io.write_sidb_layout_svg_to_string

```

:::

::::
