Distance Functions
------------------

Distance functions compute (an approximation for) the distance between two coordinates.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/distance.hpp``

        .. doxygenfunction:: fiction::manhattan_distance
        .. doxygenfunction:: fiction::euclidean_distance
        .. doxygenfunction:: fiction::twoddwave_distance

        .. doxygenclass:: fiction::distance_functor
           :members:
        .. doxygenclass:: fiction::manhattan_distance_functor
        .. doxygenclass:: fiction::euclidean_distance_functor
        .. doxygenclass:: fiction::twoddwave_distance_functor

    .. tab:: Python
        .. autofunction:: mnt.pyfiction.manhattan_distance
        .. autofunction:: mnt.pyfiction.euclidean_distance
        .. autofunction:: mnt.pyfiction.twoddwave_distance

Distance Maps
-------------

Distance maps can store the distance from a coordinate to all other coordinates. They are particularly useful when
repeatedly calling complex distance functions that are expensive to evaluate. The distance maps can serve as a
lookup-table for these cases.

**Header:** ``fiction/algorithms/path_finding/distance_map.hpp``

.. doxygentypedef:: fiction::distance_map
.. doxygentypedef:: fiction::sparse_distance_map

.. doxygenfunction:: fiction::initialize_distance_map
.. doxygenfunction:: fiction::initialize_sparse_distance_map

.. doxygenclass:: fiction::distance_map_functor
   :members:
.. doxygenclass:: fiction::sparse_distance_map_functor
   :members:
.. doxygenclass:: fiction::smart_distance_cache_functor
   :members:

Cost Functions
--------------

Cost functions compute the cost to move from one coordinate to another (adjacent) one.

**Header:** ``fiction/algorithms/path_finding/cost.hpp``

.. doxygenfunction:: fiction::unit_cost
.. doxygenfunction:: fiction::random_cost

.. doxygenclass:: fiction::cost_functor
   :members:
.. doxygenclass:: fiction::unit_cost_functor
.. doxygenclass:: fiction::random_cost_functor

A* Shortest Path
----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/a_star.hpp``

        .. doxygenstruct:: fiction::a_star_params
           :members:
        .. doxygenfunction:: fiction::a_star
        .. doxygenfunction:: fiction::a_star_distance
        .. doxygenclass:: fiction::a_star_distance_functor

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.a_star_params
            :members:
        .. autofunction:: mnt.pyfiction.a_star
        .. autofunction:: mnt.pyfiction.a_star_distance

Jump Point Search Shortest Path in a Cartesian Grid
---------------------------------------------------

**Header:** ``fiction/algorithms/path_finding/jump_point_search.hpp``

.. doxygenfunction:: fiction::jump_point_search

k Shortest Paths
----------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/k_shortest_paths.hpp``

        .. doxygenstruct:: fiction::yen_k_shortest_paths_params
           :members:
        .. doxygenfunction:: fiction::yen_k_shortest_paths

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.yen_k_shortest_paths_params
            :members:
        .. autofunction:: mnt.pyfiction.yen_k_shortest_paths

Enumerate All Paths
-------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/enumerate_all_paths.hpp``

        .. doxygenstruct:: fiction::enumerate_all_paths_params
           :members:
        .. doxygenfunction:: fiction::enumerate_all_paths

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.enumerate_all_paths_params
            :members:
        .. autofunction:: mnt.pyfiction.enumerate_all_paths
