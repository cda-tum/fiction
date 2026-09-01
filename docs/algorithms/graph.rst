Generate Edge Intersection Graph
--------------------------------

**Header:** ``fiction/physical_design/generate_edge_intersection_graph.hpp``

.. doxygenstruct:: fiction::physical_design::generate_edge_intersection_graph_params
   :members:
.. doxygenstruct:: fiction::physical_design::generate_edge_intersection_graph_stats
   :members:
.. doxygenfunction:: fiction::physical_design::generate_edge_intersection_graph


Graph Coloring
--------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/utils/graph/graph_coloring.hpp``

        .. doxygenenum:: fiction::utils::graph::graph_coloring_engine
        .. doxygenenum:: fiction::utils::graph::graph_coloring_sat_search_tactic
        .. doxygenstruct:: fiction::utils::graph::determine_vertex_coloring_sat_params
           :members:
        .. doxygenstruct:: fiction::utils::graph::determine_vertex_coloring_heuristic_params
           :members:
        .. doxygenstruct:: fiction::utils::graph::determine_vertex_coloring_params
           :members:
        .. doxygenstruct:: fiction::utils::graph::determine_vertex_coloring_stats
           :members:
        .. doxygenfunction:: fiction::utils::graph::determine_vertex_coloring

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.graph_coloring_engine
            :members:
