Cell-level Layout
=================

The cell-level layout can be layered on top of any clocked layout type at compile time to extend its functionality by
a notion of FCN cells that can be assigned to its clock zones. In contrast to ``gate_level_layout``, this layout type
does not abstract from a technology implementation but embraces it. The cell-level layout requires a
:ref:`cell technology <fcn-cell-technologies>` as a template parameter that specifies the types and properties of cells
available to it. These could for instance be QCA, iNML, or SiDB.

Cell-level layouts merely represent structural information of the FCN circuit but do not inherently possess any
knowledge of their implemented functionality.

A cell-level layout can be obtained from a ``gate_level_layout`` by the :ref:`application <apply-gate-library>` of a
:ref:`gate library <fcn-gate-libraries>`.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/layouts/cell_level_layout.hpp``

        .. doxygenclass:: fiction::cell_level_layout
           :members:

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.qca_layout
            :members:
        .. autoclass:: mnt.pyfiction.inml_layout
            :members:
        .. autoclass:: mnt.pyfiction.sidb_layout
            :members:
