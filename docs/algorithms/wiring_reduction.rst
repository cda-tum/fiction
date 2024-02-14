.. _wiring_reduction:

Reducing Wiring in 2DDWave-clocked Cartesian Layouts
----------------------------------------------------

**Header:** ``fiction/algorithms/physical_design/wiring_reduction.hpp``

This algorithm aims to minimize the number of wire segments, the area, and the length of the critical path in
2DDWave-clocked Cartesian gate-level layouts.

Initially, it constructs an equivalent layout where non-wire tiles are obstructed, and wire-tiles are obstructed
selectively based on the search direction. Subsequently, it employs A* path-finding to identify cuts through the layout
that are eligible for deletion.

The removal of these wire tiles creates gaps, which are then filled by shifting all gates located beneath the emptied
spaces upward and subsequently reconnecting them. This iterative process continues until convergence is achieved.

.. doxygenstruct:: fiction::wiring_reduction_stats
   :members:
.. doxygenfunction:: fiction::wiring_reduction
