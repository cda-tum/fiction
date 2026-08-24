Generate Edge Intersection Graph
--------------------------------

**Header:** ``fiction/physical_design/utils/generate_edge_intersection_graph.hpp``

.. doxygenstruct:: fiction::physical_design::utils::generate_edge_intersection_graph_params
   :members:
.. doxygenstruct:: fiction::physical_design::utils::generate_edge_intersection_graph_stats
   :members:
.. doxygenfunction:: fiction::physical_design::utils::generate_edge_intersection_graph


Graph Coloring
--------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/physical_design/utils/graph_coloring.hpp``

        .. doxygenenum:: fiction::physical_design::utils::graph_coloring_engine
        .. doxygenenum:: fiction::physical_design::utils::graph_coloring_sat_search_tactic
        .. doxygenstruct:: fiction::physical_design::utils::determine_vertex_coloring_sat_params
           :members:
        .. doxygenstruct:: fiction::physical_design::utils::determine_vertex_coloring_heuristic_params
           :members:
        .. doxygenstruct:: fiction::physical_design::utils::determine_vertex_coloring_params
           :members:
        .. doxygenstruct:: fiction::physical_design::utils::determine_vertex_coloring_stats
           :members:
        .. doxygenfunction:: fiction::physical_design::utils::determine_vertex_coloring

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.graph_coloring_engine
            :members:
