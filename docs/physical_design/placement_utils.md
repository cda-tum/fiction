(placement-utils)=

# Placement Utilities

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/physical_design/placement_utils.hpp`

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n)

```

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n, const typename fiction::layouts::gate_layout<Geometry>::object_id& a)

```

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n, const typename fiction::layouts::gate_layout<Geometry>::object_id& a, const typename fiction::layouts::gate_layout<Geometry>::object_id& b, const std::optional<bool>& c = std::nullopt)

```

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n, const typename fiction::layouts::gate_layout<Geometry>::object_id& a, const typename fiction::layouts::gate_layout<Geometry>::object_id& b, const typename fiction::layouts::gate_layout<Geometry>::object_id& c)

```

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n, const mockturtle::node_map<typename fiction::layouts::gate_layout<Geometry>::object_id, tec_nt>& node2pos)

```

```{doxygenstruct} fiction::physical_design::branching_signal_container
:members:
```

```{doxygenfunction} fiction::physical_design::place(fiction::layouts::gate_layout<Geometry>& lyt, const fiction::layouts::layout_base::coordinate& t, const tec_nt& ntk, const tec_nt::node& n, const mockturtle::node_map<branching_signal_container<fanout_size>, tec_nt>& node2pos)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.physical_design.place
```

:::

::::
