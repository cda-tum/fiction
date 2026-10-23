# Operational Checks

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/simulation/logic/is_operational.hpp`

```{doxygenenum} fiction::sidb::simulation::logic::operational_status

```

```{doxygenstruct} fiction::sidb::simulation::logic::is_operational_params
:members:
```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_operational(const std::vector<layout>& input_pattern_layouts, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::is_kink_induced_non_operational(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params = {})

```

```{doxygenfunction} fiction::sidb::simulation::logic::kink_induced_non_operational_input_patterns(const layout& lyt, const std::vector<kitty::dynamic_truth_table>& spec, const is_operational_params& params, const std::vector<bdl_wire>& input_bdl_wire, const std::vector<bdl_wire>& output_bdl_wire, const std::optional<layout>& canvas_lyt = std::nullopt)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_status
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_condition
   :members:

.. autoclass:: mnt.pyfiction.sidb.simulation.logic.operational_analysis_strategy
   :members:

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.is_operational

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.operational_input_patterns

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.is_kink_induced_non_operational

.. autofunction:: mnt.pyfiction.sidb.simulation.logic.kink_induced_non_operational_input_patterns
```

:::

::::
