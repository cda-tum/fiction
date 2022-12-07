Obstruction Layout
==================

**Header:** ``fiction/layouts/obstruction_layout.hpp``

The obstruction layout enables a unified interface for checking coordinates and connections for their obstruction that
can be layered on top of any coordinate layout. An obstruction could be due to a prior placement or due to a fabrication
defect. This interface can be added before construction of a new layout or to an already existing one without altering
it.

.. doxygenclass:: fiction::obstruction_layout
   :members:
