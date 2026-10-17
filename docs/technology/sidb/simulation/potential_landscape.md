# Potential Landscape

The potential landscape stores the static electrostatics of one layout and parameter set: SiDB pair distances and
potentials, defect and external potentials, and charge transition thresholds. Simulations share the immutable
landscape across worker threads; it evaluates local potentials, energy, and physical validity for a
`charge_distribution`.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/potential_landscape.hpp`

```{doxygenenum} fiction::sidb::simulation::charge_transition_threshold_bounds
```

```{doxygenclass} fiction::sidb::simulation::potential_landscape
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.charge_transition_threshold_bounds
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.potential_landscape
   :members:
```

:::

::::
