.. _apply gate library

Apply Gate Library
------------------

**Header:** ``fiction/layouts/apply_gate_library.hpp``

Applies an FCN `gate library <gate libraries>`_ to a ``gate_level_layout`` to obtain a ``cell_level_layout`` implemented
in the same `technology <cell technologies>`_ as the provided gate library. Thereby, this function creates cell-accurate
implementations for each gate present in the passed ``gate_level_layout``.

.. doxygenfunction:: fiction::apply_gate_library(const GateLyt& lyt)
