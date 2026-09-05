(gate_design)=

# SiDB Gate Designer

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/design_gates.hpp`

```{doxygenstruct} fiction::sidb::generators::design_gates_stats
:members:
```

```{doxygenstruct} fiction::sidb::generators::design_gates_params
:members:
```

```{doxygenfunction} fiction::sidb::generators::design_gates(const layout& skeleton, const std::vector<TT>& spec, const design_gates_params& params = {}, design_gates_stats* stats = nullptr)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.design_sidb_gates_stats
   :members:

.. autoclass:: mnt.pyfiction.design_sidb_gates_params
   :members:

.. autofunction:: mnt.pyfiction.design_sidb_gates
```

:::

::::
