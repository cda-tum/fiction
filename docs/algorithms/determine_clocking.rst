.. _determine_clocking:

Determine Clocking
------------------

**Header:** ``fiction/algorithms/physical_design/determine_clocking.hpp``

Determines a new clock number assignment for the given gate-level layout. Whether the layout has useful/valid clocking
information or not, this function will override each gate and wire segment with new clocking information if a valid
clock number assignment exists.

.. doxygenstruct:: fiction::determine_clocking_params
   :members:
.. doxygenstruct:: fiction::determine_clocking_stats
   :members:
.. doxygenfunction:: fiction::determine_clocking
