.. _bfs_topo_view:

Breadth-First Topological View (bfs_topo_view)
----------------------------------------------

Provides a **breadth-first topological ordering** for all nodes reachable from the outputs of a logic network.
While Mockturtle's ``topo_view`` implements a depth-first traversal, this view constructs the node order level by level
using a queue-based breadth-first search (BFS).

All constant nodes and primary inputs are included in the traversal order, even if they are not directly reachable from
the outputs. Constants always precede primary inputs, and primary inputs retain their creation order.
This view is immutable and should only be used on static networks—modifications to the underlying network are not
reflected once the order is constructed.

Typical use cases include:
  * Building rank or level-based data structures for planarization or visualization.
  * Constructing balanced rank views (e.g., for crossing minimization).
  * Performing topological analyses in breadth-first order rather than depth-first.

.. tabs::
    .. tab:: C++
        **Header:** ``fiction/networks/views/bfs_topo_view.hpp``

        .. doxygenclass:: fiction::bfs_topo_view
           :members:
