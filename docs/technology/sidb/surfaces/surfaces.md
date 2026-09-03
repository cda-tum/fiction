# SiDB Surfaces

```{toctree}
:maxdepth: 1

hsi_lattices
```

## SiDB Defect Surface

**Header:** `fiction/technology/sidb/surfaces/defect_surface.hpp`

A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
fabrication defects on the H-Si(100) 2x1 surface.

```{doxygenclass} fiction::sidb::surfaces::defect_surface
:members:
```

```{doxygenclass} fiction::sidb::surfaces::defect_surface< Lyt, true >
:members:
```

```{doxygenclass} fiction::sidb::surfaces::defect_surface< Lyt, false >
:members:
```

## SiDB Charge Distribution Surface

The `charge_distribution_surface` is superseded by {doc}`charge_distribution <../layout>` for charge states,
{doc}`potential_landscape <../simulation/simulation>` for electrostatics, and a simulation-private working
state for the engines. It remains available while the remaining consumers migrate.
