.. _a_star_pr:

A* Physical Design
------------------

Generates FCN gate-level layouts from logic network specifications by spanning a search space graph where each placement
event can be represented as a search space vertex characterized by a partial layout at that instance. Edges between a
partial layout a and b exist iff a can be transformed into b via a single placement event. Similar to navigating through
a maze, A*-search can be employed to discover a path from the starting vertex (the empty layout) to the exit of the
maze (a layout with all gates placed). This approach is scalable but requires that the input network is restricted
to a 3-graph. At the same time, the output layout will always be 2DDWave-clocked and is not always optimal.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/algorithms/physical_design/a_star_pr.hpp``

        .. doxygenstruct:: fiction::a_star_pr_params
           :members:
        .. doxygenstruct:: fiction::a_star_pr_stats
           :members:
        .. doxygenfunction:: fiction::a_star_pr

    .. tab:: Python
        .. autoclass:: mnt.pyfiction.a_star_pr_params
            :members:
        .. autoclass:: mnt.pyfiction.a_star_pr_stats
            :members:
        .. autofunction:: mnt.pyfiction.a_star_pr
