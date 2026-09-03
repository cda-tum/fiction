# SiDB Model

## Physical Parameters

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

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_simulation_parameters
   :members:
```

:::

::::

## Convert Potential to Distance

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/potential_to_distance_conversion.hpp`

```{doxygenfunction} fiction::sidb::model::potential_to_distance_conversion

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.potential_to_distance_conversion
```

:::

::::

## SiDB Defect Types

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/defect.hpp`

```{doxygenenum} fiction::sidb::model::defect_type

```

```{doxygenstruct} fiction::sidb::model::defect
:members:
```

```{doxygenfunction} fiction::sidb::model::is_charged_defect_type

```

```{doxygenfunction} fiction::sidb::model::is_neutral_defect_type

```

```{doxygenfunction} fiction::sidb::model::is_positively_charged_defect

```

```{doxygenfunction} fiction::sidb::model::is_negatively_charged_defect

```

```{doxygenfunction} fiction::sidb::model::is_neutrally_charged_defect

```

```{doxygenvariable} fiction::sidb::model::CHARGED_DEFECT_HORIZONTAL_SPACING

```

```{doxygenvariable} fiction::sidb::model::CHARGED_DEFECT_VERTICAL_SPACING

```

```{doxygenvariable} fiction::sidb::model::NEUTRAL_DEFECT_HORIZONTAL_SPACING

```

```{doxygenvariable} fiction::sidb::model::NEUTRAL_DEFECT_VERTICAL_SPACING

```

```{doxygenfunction} fiction::sidb::model::defect_extent

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_defect_type
   :members:

.. autoclass:: mnt.pyfiction.sidb_defect
   :members:

.. autofunction:: mnt.pyfiction.is_charged_defect_type

.. autofunction:: mnt.pyfiction.is_neutral_defect_type

.. autofunction:: mnt.pyfiction.is_positively_charged_defect

.. autofunction:: mnt.pyfiction.is_negatively_charged_defect

.. autofunction:: mnt.pyfiction.is_neutrally_charged_defect

.. autofunction:: mnt.pyfiction.defect_extent
```

:::

::::

## SiDB Charge States

Possible charge states of SiDBs.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/charge_state.hpp`

```{doxygenenum} fiction::sidb::model::charge_state

```

```{doxygenvariable} fiction::sidb::model::CHARGE_STATES_BASE_2

```

```{doxygenvariable} fiction::sidb::model::CHARGE_STATES_BASE_3

```

```{doxygenfunction} fiction::sidb::model::charge_states_for_base_number

```

```{doxygenfunction} fiction::sidb::model::charge_state_to_sign

```

```{doxygenfunction} fiction::sidb::model::sign_to_charge_state

```

```{doxygenfunction} fiction::sidb::model::charge_configuration_to_string

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_charge_state
   :members:

.. autofunction:: mnt.pyfiction.sidb_charge_states_for_base_number

.. autofunction:: mnt.pyfiction.charge_state_to_sign

.. autofunction:: mnt.pyfiction.sign_to_charge_state

.. autofunction:: mnt.pyfiction.charge_configuration_to_string
```

:::

::::

## Physical Constants

**Header:** `fiction/technology/sidb/model/physical_constants.hpp`

```{doxygenvariable} fiction::sidb::model::BOLTZMANN_CONSTANT

```

```{doxygenvariable} fiction::sidb::model::EV_TO_JOULE

```

```{doxygenvariable} fiction::sidb::model::ELEMENTARY_CHARGE

```

```{doxygenvariable} fiction::sidb::model::K_E

```

## Euclidean distance between two SiDBs in nanometers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/nm_distance.hpp`

```{doxygenfunction} fiction::sidb::model::nm_distance

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.sidb_nm_distance_100

.. autoclass:: mnt.pyfiction.sidb_nm_distance_111
```

:::

::::

## SiDB Position in Nanometers

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/sidb/model/nm_position.hpp`

```{doxygenfunction} fiction::sidb::model::nm_position

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.sidb_nm_position
```

:::

::::
