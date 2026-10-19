# BDL Input Iterator

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp`

```{doxygenstruct} fiction::sidb::simulation::logic::bdl_input_iterator_params
:members:
```

```{doxygenclass} fiction::sidb::simulation::logic::bdl_input_iterator
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const layout& lyt, const bdl_input_iterator_params& ps = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::generate_bdl_input_pattern_layouts(const layout& lyt, const bdl_input_iterator_params& ps, const std::vector<bdl_wire>& input_wires)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.input_bdl_configuration
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.bdl_input_iterator
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.generate_bdl_input_pattern_layouts
```

:::

::::
