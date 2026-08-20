# Upgrading

How to migrate across a breaking change in _fiction_, newest release first. Each entry
names what changed and shows the code that replaces what stopped working.

The [changelog](https://fiction.readthedocs.io/en/latest/changelog.html) records every
change in one or two sentences; this file covers the ones that need more. `AGENTS.md`
states which changes need an entry here.

## Unreleased

### Removed algorithms, commands, and headers

Mugen-based one-pass synthesis, jump-point search, and QCA energy dissipation are gone,
together with the `onepass` and `energy` CLI commands and the `range_t` header.

`a_star` replaces `jump_point_search`. It takes the same arguments and returns the same
path type, so the call changes in name only.

Before:

```c++
#include <fiction/algorithms/path_finding/jump_point_search.hpp>

const auto path = fiction::jump_point_search<fiction::layout_coordinate_path<Lyt>>(lyt, {source, target});
```

After:

```c++
#include <fiction/algorithms/path_finding/a_star.hpp>

const auto path = fiction::a_star<fiction::layout_coordinate_path<Lyt>>(lyt, {source, target});
```

`exact` replaces `one_pass_synthesis`, but it is a different algorithm rather than a
drop-in. It places and routes a network that already exists and takes
`exact_physical_design_params` where one-pass synthesis took `one_pass_synthesis_params`.
The overload that accepted truth tables has no counterpart: synthesize a network from the
specification first, then pass that network to `exact`.

`qca_energy_dissipation` and the `energy` command have no replacement.

`range_t` and `fiction/utils/range.hpp` are gone. `coordinates()` and
`ground_coordinates()` on `cartesian_layout` and `hexagonal_layout` return a
`std::ranges::subrange` instead, which iterates identically. Only code that included the
header or named `range_t` as a type has to change.

### `technology_mapping` maps with `mockturtle::emap`

`technology_mapping` and the `map` command now call `mockturtle::emap` instead of
`mockturtle::map`. The parameter and statistics members change type with it, so code that
names either type no longer compiles.

Before:

```c++
mockturtle::map_params mp{};
mp.verbose = true;

fiction::technology_mapping_params ps{};
ps.mapper_params = mp;
```

After:

```c++
mockturtle::emap_params mp{};
mp.verbose = true;

fiction::technology_mapping_params ps{};
ps.mapper_params = mp;
```

`technology_mapping_stats::mapper_stats` is a `mockturtle::emap_stats` for the same
reason. A setting that exists only on `mockturtle::map_params` has no counterpart:
`enable_logic_sharing` is the one the CLI exposed, and the `map --logic_sharing` flag is
removed with it.

### `determine_all_combinations_of_distributing_k_entities_on_n_positions` moved

The function moved out of `fiction/utils/math_utils.hpp` into the new
`fiction/utils/combination_utils.hpp` to keep the heavier includes off the path that
`traits.hpp` pulls in. Its name and signature are unchanged, so only the include changes.

Before:

```c++
#include <fiction/utils/math_utils.hpp>
```

After:

```c++
#include <fiction/utils/combination_utils.hpp>
```

`design_sidb_gates.hpp` and `displacement_robustness_domain.hpp` include the new header
themselves, so code that reached the function through one of those keeps compiling. Only a
direct include of `math_utils.hpp` breaks.

### The `map` command reports a failed mapping

The `map` command warns when the network it maps is already mapped, and reports a mapping
error instead of storing the failed result. A script that mapped twice used to gain a store
entry either way; it now prints `[w] ... is already mapped` on the second pass and, where
the mapping fails, `[e] an error occurred in mockturtle's technology mapper` and no new
entry. Check the store after `map` rather than assuming the entry is there.

### Generated docstring symbols are named `mkd_doc_*`

The docstring generator emits `mkd_doc_*` symbols instead of `__doc_*`, which the standard
reserves. `DOC(...)` is unchanged, so binding code that reads docstrings through the macro
needs no edit. A hand-written docstring that defines such a symbol directly has to be
renamed.

Before:

```c++
inline constexpr auto __doc_fiction_cartesian_layout_overridden = R"doc(...)doc";
```

After:

```c++
inline constexpr auto mkd_doc_fiction_cartesian_layout_overridden = R"doc(...)doc";
```

One substitution covers a fork that carries its own docstrings:

```bash
sed -i 's/\b__doc_/mkd_doc_/g' <files>
```

### `generate_defective_surface.py` produces a different surface

The out-of-bounds guard in the defect placement was always false, so a defect whose
footprint ran off the right or bottom edge was placed clipped instead of being rejected.
The guard now rejects it, and the generated surface changes.

Regenerate any surface produced by an earlier revision. A measurement taken on an older
surface is not comparable to one taken on a surface generated now, because the two contain
different defects.
