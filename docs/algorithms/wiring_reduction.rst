.. _wiring_reduction:

Reducing Wiring in 2DDWave-clocked Cartesian Layouts
----------------------------------------------------

**Header:** ``fiction/algorithms/physical_design/wiring_reduction.hpp``

This algorithm performs wiring reduction of a 2DDWave-clocked Cartesian gate-level layout by iteratively
finding tiles with excess wiring that can be deleted without changing the correctness of the underlying function.
The process continues until no further wires can be deleted.

.. doxygenstruct:: fiction::wiring_reduction_stats
   :members:
.. doxygenfunction:: fiction::wiring_reduction
