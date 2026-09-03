# Network I/O

## Logic Networks

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/io/network_reader.hpp`

```{doxygenclass} fiction::networks::io::network_reader
:members:
```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.read_technology_network

```

:::

::::

## Graphviz (DOT) Drawers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/io/dot_drawers.hpp`

```{doxygenclass} fiction::networks::io::technology_dot_drawer

```

```{doxygenclass} fiction::networks::io::color_view_drawer

```

```{doxygenclass} fiction::networks::io::edge_color_view_drawer

```

```{doxygenfunction} fiction::networks::io::write_dot_network(const Ntk& ntk, std::ostream& os, const Drawer& drawer = {})

```

```{doxygenfunction} fiction::networks::io::write_dot_network(const Ntk& ntk, const std::string_view& filename, const Drawer& drawer = {})

```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.write_dot_network

```

:::

::::
