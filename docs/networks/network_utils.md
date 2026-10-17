(network-utils)=

# Network Utilities

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/network_utils.hpp`

```{doxygenstruct} mockturtle::edge

```

```{doxygentypedef} fiction::networks::edge_path
```

```{doxygenfunction} fiction::networks::foreach_edge

```

```{doxygenfunction} fiction::networks::foreach_outgoing_edge

```

```{doxygenfunction} fiction::networks::foreach_incoming_edge

```

```{doxygenfunction} fiction::networks::fanouts

```

```{doxygenstruct} fiction::networks::fanin_container
:members:
```

```{doxygenfunction} fiction::networks::fanins

```

```{doxygenfunction} fiction::networks::num_constant_fanins

```

```{doxygenclass} fiction::networks::high_degree_fanin_exception
:members:
:undoc-members:
```

```{doxygenfunction} fiction::networks::has_high_degree_fanin_nodes

```

```{doxygenstruct} fiction::networks::fanin_edge_container
:members:
```

```{doxygenfunction} fiction::networks::fanin_edges

```

```{doxygenfunction} fiction::networks::has_incoming_primary_input

```

```{doxygenfunction} fiction::networks::all_incoming_edge_paths

```

```{doxygenfunction} fiction::networks::inverse_levels

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.networks.high_degree_fanin_exception
   :members:

.. autofunction:: mnt.pyfiction.networks.has_high_degree_fanin_nodes
```

:::

::::
