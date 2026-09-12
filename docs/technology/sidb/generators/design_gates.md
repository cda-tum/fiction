(gate_design)=

# SiDB Gate Designer

Set `timeout` to a millisecond budget to limit a gate search. The default, `2**64 - 1`,
leaves the search unlimited; `0` expires immediately. Deadlines are cooperative, so an
operation can finish after the deadline before the next check stops the search.
Finite timeouts support QuickExact, ExGS, and QuickSim; ClusterComplete rejects them.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/design_gates.hpp`

An expired deadline throws `fiction::utils::timeout_error` without returning partial results.

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

The call copies the skeleton, specification, and parameters before releasing the GIL.
An expired deadline raises the built-in `TimeoutError`; the input skeleton remains unchanged.
Statistics are published only after a successful search.

```python
params = design_sidb_gates_params()
params.timeout = 5_000  # milliseconds
try:
    gates = design_sidb_gates(skeleton, specification, params)
except TimeoutError:
    print("Gate design exceeded its time budget")
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
