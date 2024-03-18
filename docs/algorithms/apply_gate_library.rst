.. _apply-gate-library:

Apply Gate Library
------------------

Applies an FCN :ref:`gate library <fcn-gate-libraries>` to a ``gate_level_layout`` to obtain a ``cell_level_layout`` implemented
in the same :ref:`technology <fcn-cell-technologies>` as the provided gate library. Thereby, this function creates cell-accurate
implementations for each gate present in the passed ``gate_level_layout``.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/apply_gate_library.hpp``

        .. doxygenfunction:: fiction::apply_gate_library(const GateLyt& lyt)

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.apply_qca_one_library
        .. autofunction:: mnt.pyfiction.apply_topolinano_library
        .. autofunction:: mnt.pyfiction.apply_bestagon_library
