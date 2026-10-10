# Bounding Box

The bounding box encloses occupied coordinates, including objects outside an editing layout's
frame. Empty layouts have no minimum or maximum coordinate: getters return optional values in
C++ and `None` in Python. Width and height are inclusive coordinate counts.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/layouts/bounding_box.hpp`

```{doxygenclass} fiction::layouts::bounding_box_2d
:members:
```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.layouts.cartesian_gate_layout.bounding_box_2d
   :no-index:

.. autofunction:: mnt.pyfiction.layouts.hexagonal_gate_layout.bounding_box_2d
   :no-index:

.. autofunction:: mnt.pyfiction.qca.qca_layout.bounding_box_2d
   :no-index:

.. autofunction:: mnt.pyfiction.mol_qca.mol_qca_layout.bounding_box_2d
   :no-index:

.. autofunction:: mnt.pyfiction.inml.inml_layout.bounding_box_2d
   :no-index:

```

:::

::::
