.. _mincross:

Crossing Minimization (MinCross)
--------------------------------

Reimplementation of Graphviz's ``mincross`` algorithm for edge crossing minimization in leveled logic networks.
This algorithm iteratively reorders nodes within ranks to reduce the total number of edge crossings, using the **median**
and **transpose** heuristics originally described in Graphviz's ``dot`` layout engine.
It operates on networks with explicit rank information (e.g., ``mutable_rank_view``) and produces a layout-friendly node
ordering that improves readability and wire routing efficiency.

The algorithm consists of three main optimization passes:
  1. **Initialization pass** — explores simple reorderings to quickly lower crossings.
  2. **Refinement pass** — stabilizes ordering while checking convergence thresholds.
  3. **Full optimization pass** — performs iterative median and transposition steps until no further improvement is observed.

Each pass alternates between upward and downward traversal through the network ranks to refine node orderings
based on the median positions of their fanin and fanout connections.

.. tabs::
    .. tab:: C++
        **Header:** ``include/fiction/algorithms/graph/mincross.hpp``

        .. doxygenstruct:: fiction::mincross_params
           :members:

        .. doxygenstruct:: fiction::mincross_stats
           :members:

        .. doxygenfunction:: fiction::mincross(Ntk& ntk, const mincross_params& ps = {}, mincross_stats* pst = nullptr)
