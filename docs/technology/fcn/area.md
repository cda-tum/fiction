# Area Requirements

::::{tab-set}
:sync-group: language

:::{tab-item} C++
:sync: cpp

**Header:** `fiction/technology/fcn/area.hpp`

```{doxygenstruct} fiction::fcn::area_params
:members:
```

```{doxygenstruct} fiction::fcn::area_stats
:members:
```

```{doxygenfunction} fiction::fcn::area(const sidb::layout& lyt, const area_params<sidb::layout>& ps = {}, area_stats* pst = nullptr) noexcept
```

```{doxygenfunction} fiction::fcn::area(const Lyt& lyt, const area_params<Dims>& ps = {}, area_stats* pst = nullptr)

```

```{doxygenfunction} fiction::fcn::area(const layouts::bounding_box<Lyt>& bb, const area_params<Lyt>& ps = {}, area_stats* pst = nullptr)

```

:::

:::{tab-item} Python
:sync: python

```{eval-rst}
.. autofunction:: mnt.pyfiction.fcn.area
```

:::

::::
