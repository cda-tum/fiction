Tile-based Layout
=================

**Header:** ``fiction/layouts/tile_based_layout.hpp``

The tile-based layout implements a naming convention that can be layered on top of any coordinate layout like
``cartesian_layout`` or ``hexagonal_layout`` at compile time. It does not implement any additional functionality but
conveys the intention that each coordinate is in actuality referring to a group of smaller coordinates in another
abstraction down the hierarchy. In the literature, these are called tiles.

.. doxygenclass:: fiction::tile_based_layout
   :members:
