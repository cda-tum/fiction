(surface-analysis)=

# Surface Analysis

Determines which gate-tile pairs of a gate-level layout cannot be realized with a given SiDB gate library on a defective
H-Si surface, because the SiDBs of the respective gate implementations would coincide with (or be affected by) atomic
defects. The resulting black list can be handed to {ref}`exact <exact>` to steer placement and routing around the defects.

**Header:** `fiction/physical_design/surface_analysis.hpp`

```{doxygentypedef} fiction::physical_design::surface_black_list

```

```{doxygenfunction} fiction::physical_design::surface_analysis

```
