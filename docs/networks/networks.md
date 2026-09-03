# Networks

```{toctree}
:maxdepth: 1

technology_network
virtual_pi_network
io/io
views/views
```

## Network Utils

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/network_utils.hpp`

```{doxygenstruct} mockturtle::edge

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

```{autoclass} mnt.pyfiction.high_degree_fanin_exception
:members:
```

```{autofunction} mnt.pyfiction.has_high_degree_fanin_nodes

```

:::

::::

## Name Utils

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/name_utils.hpp`

```{doxygenfunction} fiction::networks::get_name

```

```{doxygenfunction} fiction::networks::set_name

```

```{doxygenfunction} fiction::networks::restore_network_name

```

```{doxygenfunction} fiction::networks::restore_input_names

```

```{doxygenfunction} fiction::networks::restore_output_names

```

```{doxygenfunction} fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<mockturtle::signal<NtkDest>, NtkSrc>& old2new) noexcept

```

```{doxygenfunction} fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<physical_design::branching_signal_container<NtkDest, NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept

```

```{doxygenfunction} fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept

```

```{doxygenfunction} fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept

```

:::

:::{tab-item} Python
:sync: python

```{autofunction} mnt.pyfiction.get_name

```

```{autofunction} mnt.pyfiction.set_name

```

:::

::::
