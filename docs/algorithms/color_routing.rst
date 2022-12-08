.. _color_routing:

Multi-Path Routing (Color Routing)
----------------------------------

**Header:** ``fiction/algorithms/physical_design/color_routing.hpp``

Utilizes graph coloring to determine non-conflicting paths between multiple given routing objectives in an FCN
gate-level layout. This approach can be parameterized to tradeoff completeness (full path enumeration and SAT-based
coloring) and runtime (limited path exploration and/or heuristic coloring). This algorithm is suitable for all clocking
schemes and layout topologies and will apply all determined paths directly to the given layout. In case heuristic
settings have been specified, the ``conduct_partial_routing`` parameter must be set to apply a non-complete set of paths
to the layout.

.. doxygenfunction:: fiction::color_routing(Lyt& lyt, const std::vector<routing_objective<Lyt>>& objectives, color_routing_params ps = {}, color_routing_stats* pst = nullptr)
