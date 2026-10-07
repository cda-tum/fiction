# Network Conversion

**Header:** `fiction/synthesis/network_conversion.hpp`

```{doxygenfunction} fiction::synthesis::convert_network

```

## Network Balancing

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/network_balancing.hpp`

```{doxygenstruct} fiction::synthesis::network_balancing_params
:members:
```

```{doxygenfunction} fiction::synthesis::network_balancing

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.network_balancing_params
   :members:

.. autofunction:: mnt.pyfiction.synthesis.network_balancing

.. autofunction:: mnt.pyfiction.synthesis.is_balanced
```

:::

::::

## Fanout Substitution

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/fanout_substitution.hpp`

```{doxygenstruct} fiction::synthesis::fanout_substitution_params
:members:
```

```{doxygenfunction} fiction::synthesis::fanout_substitution

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.fanout_substitution_params
   :members:

.. autoclass:: mnt.pyfiction.synthesis.substitution_strategy
   :members:

.. autofunction:: mnt.pyfiction.synthesis.fanout_substitution

.. autofunction:: mnt.pyfiction.synthesis.is_fanout_substituted
```

:::

::::

## Technology Mapping

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/technology_mapping.hpp`

```{doxygenclass} fiction::synthesis::missing_required_gates_exception
:members:
```

```{doxygenstruct} fiction::synthesis::technology_mapping_params
:members:
```

```{doxygenfunction} fiction::synthesis::and_or_not

```

```{doxygenfunction} fiction::synthesis::and_or_not_maj

```

```{doxygenfunction} fiction::synthesis::all_standard_2_input_functions

```

```{doxygenfunction} fiction::synthesis::all_standard_3_input_functions

```

```{doxygenfunction} fiction::synthesis::all_supported_standard_functions

```

```{doxygenstruct} fiction::synthesis::technology_mapping_stats
:members:
```

```{doxygenfunction} fiction::synthesis::technology_mapping

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autoclass:: mnt.pyfiction.synthesis.missing_required_gates_exception
   :members:

.. autoclass:: mnt.pyfiction.synthesis.technology_mapping_params
   :members:

.. autofunction:: mnt.pyfiction.synthesis.and_or_not

.. autofunction:: mnt.pyfiction.synthesis.and_or_not_maj

.. autofunction:: mnt.pyfiction.synthesis.all_standard_2_input_functions

.. autofunction:: mnt.pyfiction.synthesis.all_standard_3_input_functions

.. autofunction:: mnt.pyfiction.synthesis.all_supported_standard_functions

.. autoclass:: mnt.pyfiction.synthesis.technology_mapping_stats
   :members:

.. autofunction:: mnt.pyfiction.synthesis.technology_mapping
```

:::

::::

## Delete Virtual PIs

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/delete_virtual_pis.hpp`

```{doxygenfunction} fiction::synthesis::delete_virtual_pis

```

:::

::::

## Truth Tables

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/synthesis/truth_tables.hpp`

```{doxygenfunction} fiction::synthesis::create_id_tt

```

```{doxygenfunction} fiction::synthesis::create_not_tt

```

```{doxygenfunction} fiction::synthesis::create_and_tt

```

```{doxygenfunction} fiction::synthesis::create_or_tt

```

```{doxygenfunction} fiction::synthesis::create_nand_tt

```

```{doxygenfunction} fiction::synthesis::create_nor_tt

```

```{doxygenfunction} fiction::synthesis::create_xor_tt

```

```{doxygenfunction} fiction::synthesis::create_xnor_tt

```

```{doxygenfunction} fiction::synthesis::create_lt_tt

```

```{doxygenfunction} fiction::synthesis::create_gt_tt

```

```{doxygenfunction} fiction::synthesis::create_le_tt

```

```{doxygenfunction} fiction::synthesis::create_ge_tt

```

```{doxygenfunction} fiction::synthesis::create_and3_tt

```

```{doxygenfunction} fiction::synthesis::create_xor_and_tt

```

```{doxygenfunction} fiction::synthesis::create_or_and_tt

```

```{doxygenfunction} fiction::synthesis::create_onehot_tt

```

```{doxygenfunction} fiction::synthesis::create_maj_tt

```

```{doxygenfunction} fiction::synthesis::create_gamble_tt

```

```{doxygenfunction} fiction::synthesis::create_dot_tt

```

```{doxygenfunction} fiction::synthesis::create_ite_tt

```

```{doxygenfunction} fiction::synthesis::create_and_xor_tt

```

```{doxygenfunction} fiction::synthesis::create_xor3_tt

```

```{doxygenfunction} fiction::synthesis::create_double_wire_tt

```

```{doxygenfunction} fiction::synthesis::create_crossing_wire_tt

```

```{doxygenfunction} fiction::synthesis::create_fan_out_tt

```

```{doxygenfunction} fiction::synthesis::create_half_adder_tt

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.synthesis.create_id_tt

.. autofunction:: mnt.pyfiction.synthesis.create_not_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_or_tt

.. autofunction:: mnt.pyfiction.synthesis.create_nand_tt

.. autofunction:: mnt.pyfiction.synthesis.create_nor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xnor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_lt_tt

.. autofunction:: mnt.pyfiction.synthesis.create_gt_tt

.. autofunction:: mnt.pyfiction.synthesis.create_le_tt

.. autofunction:: mnt.pyfiction.synthesis.create_ge_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and3_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_or_and_tt

.. autofunction:: mnt.pyfiction.synthesis.create_onehot_tt

.. autofunction:: mnt.pyfiction.synthesis.create_maj_tt

.. autofunction:: mnt.pyfiction.synthesis.create_gamble_tt

.. autofunction:: mnt.pyfiction.synthesis.create_dot_tt

.. autofunction:: mnt.pyfiction.synthesis.create_ite_tt

.. autofunction:: mnt.pyfiction.synthesis.create_and_xor_tt

.. autofunction:: mnt.pyfiction.synthesis.create_xor3_tt

.. autofunction:: mnt.pyfiction.synthesis.create_double_wire_tt

.. autofunction:: mnt.pyfiction.synthesis.create_crossing_wire_tt

.. autofunction:: mnt.pyfiction.synthesis.create_fan_out_tt

.. autofunction:: mnt.pyfiction.synthesis.create_half_adder_tt
```

:::

::::
