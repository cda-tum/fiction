# SiDB Generators

```{toctree}
:maxdepth: 1

design_gates
on_the_fly_circuit_design
```

## Random SiDB Layout Generator

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/generators/random_layout_generator.hpp`

```{doxygenstruct} fiction::sidb::generators::generate_random_layout_params
:members:
```

```{doxygenfunction} fiction::sidb::generators::generate_random_layout(const generate_random_layout_params& params, const std::optional<layout>& skeleton = std::nullopt)

```

```{doxygenfunction} fiction::sidb::generators::generate_multiple_random_layouts(const generate_random_layout_params& params, const std::optional<layout>& skeleton = std::nullopt)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.generate_random_sidb_layout_params
   :members:

.. autofunction:: mnt.pyfiction.generate_random_sidb_layout

.. autofunction:: mnt.pyfiction.generate_multiple_random_sidb_layouts
```

:::

::::

## Is SiDB gate design deemed impossible

**Header:** `fiction/technology/sidb/generators/is_gate_design_impossible.hpp`

```{doxygenstruct} fiction::sidb::generators::is_gate_design_impossible_params
:members:
```

```{doxygenfunction} fiction::sidb::generators::is_gate_design_impossible(const layout& skeleton_with_defects, const std::vector<TT>& spec, const is_gate_design_impossible_params& params = {})

```
