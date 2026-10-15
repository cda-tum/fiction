(gate_design)=

# SiDB Gate Designer

Set `operational_params.timeout` in C++ or the `timeout` keyword in Python to limit a gate search in milliseconds. See {ref}`sidb_timeouts`
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
gates = design_sidb_gates(skeleton, specification, timeout=5_000)
```

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.generators.design_sidb_gates_stats
   :members:

.. autoclass:: mnt.pyfiction.sidb.generators.design_sidb_gates_mode
   :members:

.. autoclass:: mnt.pyfiction.sidb.generators.termination_condition
   :members:

.. autofunction:: mnt.pyfiction.sidb.generators.design_sidb_gates
```

:::

::::
