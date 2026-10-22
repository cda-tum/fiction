(critical_temperature)=

# Temperature Behavior

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/analysis/critical_temperature.hpp`

```{doxygenstruct} fiction::sidb::simulation::analysis::critical_temperature_stats
:members:
```

```{doxygenstruct} fiction::sidb::simulation::analysis::critical_temperature_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_gate_based(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const critical_temperature_params& params = {}, critical_temperature_stats* pst = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_gate_based(const std::vector<layout>& input_pattern_layouts, const std::vector<kitty::dynamic_truth_table>& spec, const critical_temperature_params& params, const std::vector<logic::bdl_pair>& output_bdl_pairs, const std::vector<logic::bdl_wire>& input_bdl_wires, const std::vector<logic::bdl_wire>& output_bdl_wires, critical_temperature_stats* pst = nullptr)

```

```{doxygenfunction} fiction::sidb::simulation::analysis::critical_temperature_non_gate_based(const layout& lyt, const critical_temperature_params& params = {}, critical_temperature_stats* pst = nullptr)

```

**Header:** `fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp`

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_boltzmann_factor

```

```{doxygenfunction} fiction::sidb::simulation::analysis::occupation_probability_gate_based

```

```{doxygenfunction} fiction::sidb::simulation::analysis::occupation_probability_non_gate_based

```

**Header:** `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`

```{doxygenenum} fiction::sidb::simulation::analysis::state_type

```

```{doxygentypedef} fiction::sidb::simulation::analysis::energy_and_state_type

```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_accepted

```

```{doxygenfunction} fiction::sidb::simulation::analysis::calculate_energy_and_state_type_with_kinks_rejected

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.analysis.critical_temperature_stats
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.critical_temperature_gate_based

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.critical_temperature_non_gate_based

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.occupation_probability_gate_based

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.occupation_probability_non_gate_based

.. autoclass:: mnt.pyfiction.sidb.simulation.analysis.state_type
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.calculate_energy_and_state_type_with_kinks_accepted

.. autofunction:: mnt.pyfiction.sidb.simulation.analysis.calculate_energy_and_state_type_with_kinks_rejected
```

:::

::::
