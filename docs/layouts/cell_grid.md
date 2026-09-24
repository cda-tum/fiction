# Cell Grids and Tile Clocking

The QCA, molQCA, and iNML layouts are each their own type, see {ref}`the technologies <fcn-cell-technologies>`. They
share two building blocks: `cell_grid`, the storage of their cells, and `tile_clocking`, the clock zones of the
technologies that clock tiles of cells. Neither is a layout of its own. The SiDB layout places dots on lattice sites
instead and uses neither.

A cell-level layout can be obtained from a `gate_level_layout` by the {ref}`application <apply-gate-library>` of a
{ref}`gate library <fcn-gate-libraries>`. Cell-level layouts merely represent structural information of the FCN
circuit but do not inherently possess any knowledge of their implemented functionality.

## Cell Grid

A cell grid is a Cartesian layout whose positions hold cells of one technology's cell type. It stores only occupied
positions: assigning the `EMPTY` type removes a cell together with its name. Primary inputs and outputs are the cells
of type `INPUT` and `OUTPUT`, and they carry names. Copies of a cell grid are independent.

**Header:** `fiction/layouts/cell_grid.hpp`

```{doxygenclass} fiction::layouts::cell_grid
:members:
```

## Tile Clocking

A clock zone is a tile: a region of `tile_size_x` by `tile_size_y` cells that one clock signal governs on every layer.
Usually, one gate or one wire fits into a tile. `get_clock_zone(c)` returns the clock zone of cell `c` at
`(c.x / tile_size_x, c.y / tile_size_y, 0)`.

Clock numbers belong to clock zones. `assign_clock_number(cz, number)` takes the clock-zone coordinate, whereas
`get_clock_number(c)` takes a cell and looks up its clock zone. For a tile size of 2 by 2, assigning zone `(1, 1)`
changes the result for cells `(2, 2)` through `(3, 3)` on every layer. The QCA layout additionally stores
synchronization elements per clock zone.

Clock-zone dimensions must be positive. The constructor and tile-size setters reject zero; a rejected setter call
preserves the stored dimensions.

**Header:** `fiction/layouts/tile_clocking.hpp`

```{doxygenclass} fiction::layouts::tile_clocking
:members:
```
