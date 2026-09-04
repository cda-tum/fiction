# Physical Design

```{toctree}
:maxdepth: 1

apply_gate_library
color_routing
determine_clocking
exact
graph_oriented_layout_design
hexagonalization
orthogonal
post_layout_optimization
wiring_reduction
path_finding/path_finding
```

## Generate Edge Intersection Graph

**Header:** `fiction/physical_design/generate_edge_intersection_graph.hpp`

```{doxygenstruct} fiction::physical_design::generate_edge_intersection_graph_params
:members:
```

```{doxygenstruct} fiction::physical_design::generate_edge_intersection_graph_stats
:members:
```

```{doxygenfunction} fiction::physical_design::generate_edge_intersection_graph

```

## Aspect Ratio Iterator

**Header:** `fiction/physical_design/aspect_ratio_iterator.hpp`

```{doxygenclass} fiction::physical_design::aspect_ratio_iterator
:members:
```

## Placement Utils

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/placement_utils.hpp`

```{doxygenfunction} fiction::physical_design::reserve_input_nodes

```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n) noexcept

```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a) noexcept

```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const std::optional<bool>& c = std::nullopt) noexcept

```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::signal<Lyt>& a, const mockturtle::signal<Lyt>& b, const mockturtle::signal<Lyt>& c) noexcept

```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<mockturtle::signal<Lyt>, Ntk>& node2pos) noexcept

```

```{doxygenstruct} fiction::physical_design::branching_signal_container
:members:
```

```{doxygenfunction} fiction::physical_design::place(Lyt& lyt, const tile<Lyt>& t, const Ntk& ntk, const mockturtle::node<Ntk>& n, const mockturtle::node_map<branching_signal_container<Lyt, Ntk, fanout_size>, Ntk>& node2pos) noexcept

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.reserve_input_nodes

.. autofunction:: mnt.pyfiction.place
```

:::

::::

## Routing Utils

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/routing_utils.hpp`

```{doxygenstruct} fiction::physical_design::routing_objective
:members:
```

```{doxygenclass} fiction::physical_design::layout_coordinate_path
:members:
```

```{doxygenclass} fiction::physical_design::path_collection
:members:
```

```{doxygenclass} fiction::physical_design::path_set
:members:
```

```{doxygenfunction} fiction::physical_design::is_crossable_wire

```

```{doxygenfunction} fiction::physical_design::route_path

```

```{doxygenfunction} fiction::physical_design::extract_routing_objectives

```

```{doxygenfunction} fiction::physical_design::clear_routing

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.is_crossable_wire

.. autofunction:: mnt.pyfiction.route_path

.. autofunction:: mnt.pyfiction.extract_routing_objectives

.. autofunction:: mnt.pyfiction.clear_routing
```

:::

::::
