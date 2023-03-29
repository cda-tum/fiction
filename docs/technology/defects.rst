Fabrication Defects
===================

A collection of tools to model defects that can occur during the fabrication process of FCN technologies.

SiDB Defects
------------

**Header:** ``fiction/technology/sidb_defects.hpp``

.. doxygenenum:: fiction::sidb_defect_type

.. doxygenstruct:: fiction::sidb_defect
   :members:

.. doxygenfunction:: fiction::is_charged_defect
.. doxygenfunction:: fiction::is_neutral_defect

.. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_HORIZONTAL_SPACING
.. doxygenvariable:: fiction::SIDB_CHARGED_DEFECT_VERTICAL_SPACING
.. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_HORIZONTAL_SPACING
.. doxygenvariable:: fiction::SIDB_NEUTRAL_DEFECT_VERTICAL_SPACING

.. doxygenfunction:: fiction::defect_extent


**Header:** ``fiction/technology/sidb_surface.hpp``

.. doxygenclass:: fiction::sidb_surface
   :members:
.. doxygenclass:: fiction::sidb_surface< Lyt, true >
   :members:
.. doxygenclass:: fiction::sidb_surface< Lyt, false >
   :members:


**Header:** ``fiction/technology/sidb_surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb_surface_analysis
