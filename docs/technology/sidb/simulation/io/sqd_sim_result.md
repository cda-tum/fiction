# SiQAD Simulation Results

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/io/write_sqd_sim_result.hpp`

```{doxygenfunction} fiction::sidb::simulation::io::write_sqd_sim_result(const sidb::simulation::result& sim_result, std::ostream& os)

```

```{doxygenfunction} fiction::sidb::simulation::io::write_sqd_sim_result(const sidb::simulation::result& sim_result, const std::string_view& filename)

```

**Header:** `fiction/technology/sidb/simulation/io/write_location_and_ground_state.hpp`

```{doxygenfunction} fiction::sidb::simulation::io::write_location_and_ground_state(const sidb::simulation::result& sim_result, std::ostream& os)

```

```{doxygenfunction} fiction::sidb::simulation::io::write_location_and_ground_state(const sidb::simulation::result& sim_result, const std::string_view& filename)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb.simulation.io.write_sqd_sim_result

.. autofunction:: mnt.pyfiction.sidb.simulation.io.write_location_and_ground_state
```

:::

::::
