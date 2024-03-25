Generate Edge Intersection Graph
--------------------------------

**Header:** ``fiction/algorithms/graph/generate_edge_intersection_graph.hpp``

.. doxygenstruct:: fiction::generate_edge_intersection_graph_params
   :members:
.. doxygenstruct:: fiction::generate_edge_intersection_graph_stats
   :members:
.. doxygenfunction:: fiction::generate_edge_intersection_graph


Graph Coloring
--------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/graph/graph_coloring.hpp``

        .. doxygenenum:: fiction::graph_coloring_engine
        .. doxygenenum:: fiction::graph_coloring_sat_search_tactic
        .. doxygenstruct:: fiction::determine_vertex_coloring_sat_params
           :members:
        .. doxygenstruct:: fiction::determine_vertex_coloring_heuristic_params
           :members:
        .. doxygenstruct:: fiction::determine_vertex_coloring_params
           :members:
        .. doxygenstruct:: fiction::determine_vertex_coloring_stats
           :members:
        .. doxygenfunction:: fiction::determine_vertex_coloring

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.graph_coloring_engine
            :members:
