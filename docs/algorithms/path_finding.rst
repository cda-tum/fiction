Distance Functions
------------------

Distance functions compute (an approximation for) the distance between two coordinates.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/distance.hpp``

        .. doxygenfunction:: fiction::manhattan_distance
        .. doxygenfunction:: fiction::euclidean_distance

        .. doxygenclass:: fiction::distance_functor
           :members:
        .. doxygenclass:: fiction::manhattan_distance_functor
        .. doxygenclass:: fiction::euclidean_distance_functor

    .. tab:: Python
        .. autofunction:: fiction.pyfiction.manhattan_distance
        .. autofunction:: fiction.pyfiction.euclidean_distance

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
        .. autoclass:: fiction.pyfiction.a_star_params
            :members:
        .. autofunction:: fiction.pyfiction.a_star
        .. autofunction:: fiction.pyfiction.a_star_distance

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
        .. autoclass:: fiction.pyfiction.yen_k_shortest_paths_params
            :members:
        .. autofunction:: fiction.pyfiction.yen_k_shortest_paths

Enumerate All Paths
-------------------

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/path_finding/enumerate_all_paths.hpp``

        .. doxygenstruct:: fiction::enumerate_all_clocking_paths_params
           :members:
        .. doxygenfunction:: fiction::enumerate_all_clocking_paths
    .. tab:: Python
        .. autoclass:: fiction.pyfiction.enumerate_all_clocking_paths_params
            :members:
        .. autofunction:: fiction.pyfiction.enumerate_all_clocking_paths
