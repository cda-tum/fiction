(gate_design)=

# SiDB Gate Designer

Set `timeout` in milliseconds to limit a gate search. See {ref}`sidb_timeouts`
for defaults, exceptions, and cancellation limits.

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

```{doxygenfunction} fiction::sidb::generators::design_gates(const layout& skeleton, const std::vector<kitty::dynamic_truth_table>& spec, const design_gates_params& params = {}, design_gates_stats* stats = nullptr)

```

:::

:::{tab-item} Python
:sync: python

The Python call releases the GIL and leaves the input skeleton unchanged.

```python
params = design_sidb_gates_params()
params.timeout = 5_000  # milliseconds
gates = design_sidb_gates(skeleton, specification, params)
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.design_sidb_gates_stats
   :members:

.. autoclass:: mnt.pyfiction.design_sidb_gates_params
   :members:

.. autofunction:: mnt.pyfiction.design_sidb_gates
```

:::

::::
