# Physical Parameters

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/simulation_parameters.hpp`

```{doxygenstruct} fiction::sidb::model::simulation_parameters
:members:
```

:::

:::{tab-item} Python
:sync: python

`sidb_simulation_parameters` holds physical model data, including the model returned by
`sidb_simulation_result.simulation_parameters` and `potential_landscape.params()`.
Construct it with keyword-only `base_number`, `mu_minus`, `relative_permittivity`, and `screening_distance`.
Computational functions accept scalar `base`, `mu_minus`, `epsilon_r`, and `lambda_tf` keywords instead.

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.model.sidb_simulation_parameters
   :members:
```

:::

::::
