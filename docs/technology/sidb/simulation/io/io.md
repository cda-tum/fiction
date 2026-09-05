# Simulation I/O

## SiDB Operational Domain

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/io/write_operational_domain.hpp`

```{doxygenstruct} fiction::sidb::simulation::io::write_operational_domain_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::io::write_operational_domain(const OpDomain&, std::ostream&, const write_operational_domain_params&)

```

```{doxygenfunction} fiction::sidb::simulation::io::write_operational_domain(const OpDomain&, const std::string_view&, const write_operational_domain_params&)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.write_operational_domain_params
   :members:

.. autofunction:: mnt.pyfiction.write_operational_domain

.. autofunction:: mnt.pyfiction.write_critical_temperature_domain
```

:::

::::

## SiQAD Simulation Results

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
.. autofunction:: mnt.pyfiction.write_sqd_sim_result

.. autofunction:: mnt.pyfiction.write_location_and_ground_state
```

:::

::::

## Defect Influence Domain

**Header:** `fiction/technology/sidb/simulation/io/write_defect_influence_domain.hpp`

```{doxygenstruct} fiction::sidb::simulation::io::write_defect_influence_domain_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::io::write_defect_influence_domain(const defects::defect_influence_domain& defect_infdom, std::ostream& os, const write_defect_influence_domain_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::io::write_defect_influence_domain(const defects::defect_influence_domain& defect_infdom, const std::string_view& filename, const write_defect_influence_domain_params& params = {})

```
