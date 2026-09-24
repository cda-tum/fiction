(clocking schemes)=

# Clocking Schemes

**Header:** `fiction/layouts/clocking_scheme.hpp`

Clocking is one of the most important differences between conventional CMOS technologies and the FCN domain. Clocking
directs information flow and provides a notion of timing for both combinational and sequential circuits alike. Usually,
clocking information is assigned to the coordinates of a layout by the means of a clocking scheme.

This header provides the `clocking::scheme` value type, which assigns a clock number to every tile position
$(x, y)$ of a gate-level or tile-clocked cell-level layout by repeating a rectangular cutout in all directions, as well
as pre-defined schemes from the literature. Schemes can be copied, assigned, and compared, and individual clock numbers
can be overridden.

```{doxygenclass} fiction::layouts::clocking::scheme
:members:
```

```{doxygenenum} fiction::layouts::clocking::num_clks
```

```{doxygenenum} fiction::layouts::clocking::hex_arrangement
```

## Pre-defined schemes

### Open

```{doxygenfunction} fiction::layouts::clocking::open

```

### Columnar

:::{figure} /_static/columnar.png
:alt: Columnar clocking scheme with phases progressing across columns
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::columnar

```

### Row

:::{figure} /_static/row.png
:alt: Row clocking scheme with phases progressing across rows
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::row

```

### 2DDWave

:::{figure} /_static/2ddwave.png
:alt: 2DDWave clocking scheme with phases progressing along two dimensions
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::twoddwave

```

```{doxygenfunction} fiction::layouts::clocking::twoddwave_hex

```

### USE

:::{figure} /_static/use.png
:alt: USE clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::use

```

### RES

:::{figure} /_static/res.png
:alt: RES clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::res

```

### ESR

:::{figure} /_static/esr.png
:alt: ESR clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::esr

```

### CFE

:::{figure} /_static/cfe.png
:alt: CFE clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::cfe

```

### Ripple

:::{figure} /_static/ripple.png
:alt: Ripple clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::ripple

```

### SRS

:::{figure} /_static/srs.png
:alt: SRS clocking scheme
:width: 200
:::

```{doxygenfunction} fiction::layouts::clocking::srs

```

### BANCS

:::{figure} /_static/bancs.png
:alt: BANCS clocking scheme
:width: 150
:::

```{doxygenfunction} fiction::layouts::clocking::bancs

```

## Utility functions

```{doxygenfunction} fiction::layouts::clocking::is_linear

```

```{doxygenfunction} fiction::layouts::clocking::get_scheme(const std::string_view scheme_name, const std::optional<hex_arrangement> hex)

```

```{doxygenfunction} fiction::layouts::clocking::get_scheme(const std::string_view scheme_name)

```
