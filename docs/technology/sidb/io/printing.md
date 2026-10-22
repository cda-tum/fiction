# Layout Printing

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/io/print_sidb_layout.hpp`

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution* cd, const bool lat_color, const bool crop_layout, const bool draw_lattice)
```

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```

```{doxygenfunction} fiction::sidb::io::print_sidb_layout(std::ostream& os, const layout& lyt, const charge_distribution& cd, const bool lat_color = true, const bool crop_layout = false, const bool draw_lattice = true)
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.io.print_sidb_layout
```

:::

::::
