(bfs_topo_view)=

# Breadth-First Topological View (bfs_topo_view)

`bfs_topo_view<Ntk, false>` computes a queue-based topological order from constant nodes
and all combinational inputs. The traversal follows fanouts and visits a gate when all
of its fanins have been visited. It includes input-fed gates that do not drive an output;
the traversal does not restrict the order to output dependency cones.

Constants precede combinational inputs, which retain their declared order. This
specialization wraps `mockturtle::immutable_view` and computes its order at construction.
Use it while the underlying network remains unchanged.

`bfs_topo_view<Ntk, true>` inherits `Ntk` and preserves its traversal order. The default
`sorted` argument selects this specialization when `Ntk` is already topologically sorted,
so an existing depth-first order remains depth-first.

Typical use cases include:

- Building rank or level-based data structures for planarization or visualization.
- Constructing balanced rank views (e.g., for crossing minimization).
- Performing topological analyses in breadth-first order rather than depth-first.

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/networks/views/bfs_topo_view.hpp`

```{doxygenclass} fiction::networks::views::bfs_topo_view
:members:
```

```{doxygenclass} fiction::networks::views::bfs_topo_view< Ntk, true >
:members:
```

```{doxygenclass} fiction::networks::views::bfs_topo_view< Ntk, false >
:members:
```

:::

::::
