Fabrication Defects
===================

A collection of tools to model defects that can occur during the fabrication process of FCN technologies.

SiDB Defect Types
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb_defects.hpp``

        .. doxygenenum:: fiction::sidb_defect_type

        .. doxygenstruct:: fiction::sidb_defect
           :members:

        .. doxygenfunction:: fiction::is_charged_defect_type
        .. doxygenfunction:: fiction::is_neutral_defect_type

        .. doxygenfunction:: fiction::is_positively_charged_defect
        .. doxygenfunction:: fiction::is_negatively_charged_defect
        .. doxygenfunction:: fiction::is_neutrally_charged_defect

        .. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_VERTICAL_SPACING
        .. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING

        .. doxygenfunction:: fiction::defect_extent

    .. tab:: Python
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


SiDB Defect Surface
-------------------

**Header:** ``fiction/technology/sidb_defect_surface.hpp``

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

.. doxygenclass:: fiction::sidb_defect_surface
   :members:
.. doxygenclass:: fiction::sidb_defect_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb_defect_surface< Lyt, false >
   :members:


SiDB Defect Analysis
--------------------

**Header:** ``fiction/technology/sidb_surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb_surface_analysis
