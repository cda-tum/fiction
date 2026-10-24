# Energy Calculation

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/energy_distribution.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::energy_state
:members:
```

```{doxygenclass} fiction::sidb::simulation::analysis::energy_distribution
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_distribution(const std::vector<charge_distribution>& charge_distributions)

```

**Header:** `fiction/technology/sidb/simulation/analysis/minimum_energy.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::minimum_energy

```

```{doxygenfunction} fiction::sidb::simulation::analysis::minimum_energy_distribution

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.analysis.energy_state
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.analysis.energy_distribution
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.calculate_energy_distribution

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.minimum_energy
```

:::

::::
