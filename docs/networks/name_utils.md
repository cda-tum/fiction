(name-utils)=

# Name Utilities

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
