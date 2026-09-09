(apply-gate-library)=

# Apply Gate Library

Applies an FCN {ref}`gate library <fcn-gate-libraries>` to a `gate_level_layout` to obtain a `cell_level_layout` implemented
in the same {ref}`technology <fcn-cell-technologies>` as the provided gate library. Thereby, this function creates cell-accurate
implementations for each gate present in the passed `gate_level_layout`. The variants that place gates on a defective
surface take the surface as a `sidb::layout` and return one, carrying the surface's defects.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/apply_gate_library.hpp`

```{doxygenfunction} fiction::physical_design::apply_gate_library(const GateLyt& lyt)

```

```{doxygenfunction} fiction::physical_design::apply_gate_library_to_defective_surface(const GateLyt& lyt, const sidb::layout& defect_surface)

```

```{doxygenfunction} fiction::physical_design::apply_parameterized_gate_library(const GateLyt& lyt, const Params& params)

```

```{doxygenfunction} fiction::physical_design::apply_parameterized_gate_library_to_defective_surface(const GateLyt& lyt, const Params& params, const sidb::layout& defect_surface)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.apply_qca_one_library

.. autofunction:: mnt.pyfiction.apply_topolinano_library

.. autofunction:: mnt.pyfiction.apply_bestagon_library

.. autofunction:: mnt.pyfiction.apply_sim7_mol_library
```

:::

::::
