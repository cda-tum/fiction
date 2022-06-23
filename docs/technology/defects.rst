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

.. doxygenvariable:: fiction::sidb_charged_defect_horizontal_spacing
.. doxygenvariable:: fiction::sidb_charged_defect_vertical_spacing
.. doxygenvariable:: fiction::sidb_neutral_defect_horizontal_spacing
.. doxygenvariable:: fiction::sidb_neutral_defect_vertical_spacing

.. doxygenfunction:: fiction::defect_extent


**Header:** ``fiction/technology/sidb_surface.hpp``

.. doxygenclass:: fiction::sidb_surface
   :members:


**Header:** ``fiction/technology/sidb_surface_analysis.hpp``

.. doxygenfunction:: fiction::sidb_surface_analysis
