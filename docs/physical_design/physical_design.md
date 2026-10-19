# Physical Design

Physical design maps logic networks to FCN layouts through placement, routing,
and clock assignment. Gate libraries map gate-level layouts to technology cells;
layout transformations and optimization refine the resulting designs.

Physical design consumes `fiction::tec_nt`, a named `technology_network`. Convert other network types before
placement; the conversion preserves network, primary-input, and primary-output names.

```cpp
const auto specification = fiction::synthesis::convert_network<fiction::tec_nt>(source);
const auto layout = fiction::physical_design::orthogonal<fiction::layouts::cartesian_gate_layout>(specification);
```

`orthogonal` and `exact` normalize fanout for the selected layout and clocking scheme. GOLD removes dangling gates
while preparing its search networks.

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
surface_analysis
wiring_reduction
path_finding/path_finding
generate_edge_intersection_graph
aspect_ratio_iterator
placement_utils
routing_utils
```
