Placement Layout
================

**Header:** ``fiction/layouts/placement_layout.hpp``

The placement layout enables a quick, easy, and performant way to assign layout positions to logic network nodes. While
a gate-level layout creates new nodes for each gate, a placement layout uses the same nodes as the logic network. This
layout type is useful for quickly creating, evaluating, and adjusting different placements of the same set of nodes.
This is a common task in many optimization algorithms, such as simulated annealing, tabu search, and genetic algorithms.
When settled for a satisfying placement, this layout type can be dumped into a real gate-level layout that is then
independent of the assigned logic network.

.. doxygenclass:: fiction::placement_layout
   :members:
