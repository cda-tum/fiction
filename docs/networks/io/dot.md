# Graphviz (DOT) Drawers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/io/dot_drawers.hpp`

```{doxygenclass} fiction::networks::io::technology_dot_drawer
:members:
```

```{doxygenclass} fiction::networks::io::color_view_drawer
:members:
```

```{doxygenclass} fiction::networks::io::edge_color_view_drawer
:members:
```

```{doxygenfunction} fiction::networks::io::write_dot_network(const Ntk& ntk, std::ostream& os, const Drawer& drawer = {})

```

```{doxygenfunction} fiction::networks::io::write_dot_network(const Ntk& ntk, const std::string_view& filename, const Drawer& drawer = {})

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.networks.io.write_dot_network
```

:::

::::

Python network readers and random generation accept a keyword-only `network_type` class.
Both default to `technology_network`; `aig_network`, `xag_network`, and `mig_network` select
their native representations. Pass the exact registered class; subclasses are not supported.
BLIF input supports `technology_network` only.

```python
from mnt.pyfiction.networks import aig_network, random_network
from mnt.pyfiction.networks.io import read_network

network = read_network("network.v", network_type=aig_network)
random = random_network(3, 4, 42, network_type=aig_network)
```
