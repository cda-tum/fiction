Fabrication Defects
===================

A collection of tools to model defects that can occur during the fabrication process of FCN technologies.

SiDB Defect Types
-----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/technology/sidb/model/defect.hpp``

        .. doxygenenum:: fiction::sidb::model::defect_type

        .. doxygenstruct:: fiction::sidb::model::defect
           :members:

        .. doxygenfunction:: fiction::sidb::model::is_charged_defect_type
        .. doxygenfunction:: fiction::sidb::model::is_neutral_defect_type

        .. doxygenfunction:: fiction::sidb::model::is_positively_charged_defect
        .. doxygenfunction:: fiction::sidb::model::is_negatively_charged_defect
        .. doxygenfunction:: fiction::sidb::model::is_neutrally_charged_defect

        .. doxygenvariable:: fiction::sidb::model::CHARGED_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::CHARGED_DEFECT_VERTICAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::NEUTRAL_DEFECT_HORIZONTAL_SPACING
        .. doxygenvariable:: fiction::sidb::model::NEUTRAL_DEFECT_VERTICAL_SPACING

        .. doxygenfunction:: fiction::sidb::model::defect_extent

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

**Header:** ``fiction/technology/sidb/surfaces/defect_surface.hpp``

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

.. doxygenclass:: fiction::sidb::surfaces::defect_surface
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb::surfaces::defect_surface< Lyt, false >
   :members:


SiDB Defect Analysis
--------------------

**Header:** ``fiction/technology/sidb/surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb::surface_analysis
