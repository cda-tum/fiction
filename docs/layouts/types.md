# Preset Types

Network and truth-table presets instantiate the library algorithms.
Gate-layout types are documented in {doc}`gate_layout`.

**Headers:** `fiction/types.hpp`, `fiction/synthesis/types.hpp`

```{doxygentypedef} fiction::networks::named_aig_network
```

```{doxygentypedef} fiction::networks::named_xag_network
```

```{doxygentypedef} fiction::networks::named_mig_network
```

```{doxygentypedef} fiction::networks::named_technology_network
```

Truth-table-only code can include `fiction/synthesis/types.hpp`.
`fiction/types.hpp` also exports the truth-table preset.

```{doxygentypedef} fiction::synthesis::truth_table
```
