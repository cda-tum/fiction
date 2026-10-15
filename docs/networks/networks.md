# Networks

```{toctree}
:maxdepth: 1

technology_network
virtual_pi_network
io/io
views/views
```

(network-utils)=

## Network Utilities

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

(name-utils)=

## Name Utilities

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

```{doxygenfunction} fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<Signal, NtkSrc>& old2new)

```

```{doxygenfunction} fiction::networks::restore_signal_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, const mockturtle::node_map<physical_design::branching_signal_container<NtkSrc, fanout_size>, NtkSrc>& old2new) noexcept

```

```{doxygenfunction} fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest) noexcept

```

```{doxygenfunction} fiction::networks::restore_names(const NtkSrc& ntk_src, NtkDest& ntk_dest, mockturtle::node_map<T, NtkSrc>& old2new) noexcept

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.networks.get_name

.. autofunction:: mnt.pyfiction.networks.set_name
```

:::

::::

## Simulation and Random Generation

```{eval-rst}
.. autofunction:: mnt.pyfiction.networks.simulate

.. autofunction:: mnt.pyfiction.networks.simulate_outputs

.. autofunction:: mnt.pyfiction.networks.random_network

```

## Interface Matching

**Header:** `fiction/networks/interface_matching.hpp`

```{doxygenstruct} fiction::networks::interface_matching
:members:
```

```{doxygenfunction} fiction::networks::match_interfaces
```

## Layout Network Extraction

**Header:** `fiction/networks/extract_layout_network.hpp`

```{doxygenfunction} fiction::networks::extract_layout_network
```
