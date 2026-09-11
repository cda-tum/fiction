(cli)=

# Command Line Interface (CLI)

`fiction` is an interactive shell around the `mnt.pyfiction` Python bindings. It reads logic networks and layouts
into stores, runs the design, verification, and simulation algorithms on them, and writes the results to files.
Every command also accepts `-h` for its full list of options.

## Installation

The shell ships with the Python package:

```console
$ pip install mnt.pyfiction
$ fiction
```

`python -m mnt.pyfiction.cli` starts the same shell. The {ref}`Docker image <cli-docker>` starts it as well.

## The shell

```text
fiction> help
```

lists the commands by category, `help <command>` shows one command's options, and `quit` or `Ctrl-D` leaves the
shell. The Tab key completes command names, options, and file paths. The command history persists in
`~/.fiction_history` between sessions. The line below the prompt shows how many elements each store holds and
which one is active.

Several commands may follow one another on a line, separated by `;`, and a `#` starts a comment. Quotes group
words: `tt -e "[(ab)(!ac)]"`. When a command fails, the shell prints the reason and drops the rest of the line;
the shell itself keeps running.

## Stores

Stores hold what the shell has read, created, or designed. There are four of them, each selected by a flag:

- `-t`: truth tables
- `-n`: logic networks
- `-g`: gate-level layouts
- `-c`: cell-level layouts

Each store has one _active_ element, the one commands work on, which is the most recently added element unless
`current` selects another one. The generic commands take store flags:

- `store [-t -n -g -c]` lists the elements of the given stores, or of all stores, and marks the active one
- `current -t|-n|-g|-c INDEX` makes an element the active one
- `ps -t|-n|-g|-c` prints the statistics of the active element
- `print -t|-g|-c` prints the active element as text; a simulated SiDB layout prints its ground state charges too
- `show -n|-g|-c [-o FILE] [--silent] [--simple]` draws the active element and opens it in the default viewer:
  networks and gate-level layouts as Graphviz `.dot` files, cell-level layouts as `.svg` files. `--silent` only
  writes the file, `-o` chooses where, and `--simple` draws QCA cells without dots and clock numbers
- `clear [-t -n -g -c]` empties the given stores, or all of them

## Reading and writing files

`read FILE` reads a file, chosen by its suffix:

| Suffix                 | Reads               | Store |
| ---------------------- | ------------------- | ----- |
| `.v`, `.aig`, `.blif`  | logic network       | `-n`  |
| `.aag`, `.pla`         | logic network (AIG) | `-n`  |
| `.fgl`                 | gate-level layout   | `-g`  |
| `.sqd`                 | SiDB layout         | `-c`  |
| `.fqca`                | QCA layout          | `-c`  |

Networks are read as technology networks unless `--type aig|xag|mig|tec` says otherwise; `.aag` and `.pla` files
always yield AIGs and BLIF files always technology networks. Gate-level layouts are read with the topology
`--topology cartesian|shifted_cartesian|hexagonal` names, Cartesian by default, because an FGL file does not
record it. `read DIRECTORY` reads every network file in a directory, in order of gate count with `--sort`. The
`benchmarks` folder of the repository holds many networks to start from.

`write FILE` writes the active element in the format the suffix names:

| Suffix           | Writes                                                     | Element           |
| ---------------- | ---------------------------------------------------------- | ----------------- |
| `.v`, `.blif`    | gate-level Verilog, BLIF                                   | network           |
| `.aig`           | binary AIGER                                               | network (AIG)     |
| `.dot`           | Graphviz drawing                                           | layout, or network with `-n` |
| `.fgl`           | fiction gate-level layout                                  | gate-level layout |
| `.qca`           | [QCADesigner](https://waluslab.ece.ubc.ca/qcadesigner/)    | QCA layout        |
| `.fqca`          | [QCA-STACK](https://github.com/wlambooy/QCA-STACK)         | QCA layout        |
| `.qcc`           | [ToPoliNano](https://topolinano.polito.it/) component      | iNML layout       |
| `.qll`           | ToPoliNano, MagCAD, or [SCERPA](https://ieeexplore.ieee.org/document/8935211) layout | QCA, molQCA, or iNML layout |
| `.sqd`           | [SiQAD](https://github.com/siqad/siqad)                    | SiDB layout       |
| `.svg`           | drawing                                                    | cell-level layout |

`--via-layers` and `--no-via-layers` add or omit the inter-layer via cells of `.qca` and `.fqca` files, which
`.qca` files carry by default and `.fqca` files do not. Verilog files name their module `top`, as the readers
expect, and a technology network is written as an equivalent XAG, because gate-level Verilog has no buffers.

### Truth tables

`tt` creates a truth table from a bit string (`tt -t 1000`), a hex string (`tt -t 0xe8`), a Boolean expression
(`tt -e "<abc>"`), or at random (`tt -r 3`). Bit strings are most significant bit first, so `1000` is the
two-input AND. An expression is a constant `0` or `1`, a variable `a` to `p`, a negation `!E`, a conjunction
`(E...E)`, a disjunction `{E...E}`, an exclusive OR `[E...E]`, or a majority `<EEE>`; `[(ab)(!ac)]` is
if-then-else. The number of variables follows from the largest variable used. Truth tables specify the function
that the gate-based SiDB simulations check a layout against.

## Logic networks

The network store holds AND-inverter graphs (AIG), XOR-AND-inverter graphs (XAG), majority-inverter graphs
(MIG), and technology networks (TEC). Technology mapping, gate counting, simulation, and the writers take any of
them. Physical design and verification work on technology networks; when the active network is of another type,
the command converts it on the fly and says so in its help.

- `map` maps the active network onto a set of gate types, producing a technology network: `map --and --or --inv`
  yields AND-OR-inverter networks, `--all2`, `--all3`, and `--all` select every standard 2-input, 3-input, or
  supported function, and `--decay` also tries to reduce the gate count. This is what makes a network fit an FCN
  gate library that lacks some gate types.
- `fanouts [-d 2|3] [-s breadth|depth|random]` replaces high-degree outputs by fan-out nodes. Physical design does
  this with default settings; run it beforehand to choose them.
- `balance [-u]` balances every path with buffers. No physical design algorithm needs it, and balanced networks
  produce much larger layouts.
- `gates -n|-g [--detailed]` counts the gate types of the active network or gate-level layout.
- `simulate -n|-g [--store] [--silent]` computes the truth table of every output; `--store` adds them to the truth
  table store. Layouts are simulated on the logic level, following the clocking; timing is not considered.
- `random -n INPUTS -g GATES [--seed N]` generates a random AIG named after its seed, and
  `generate rca|cla|multiplier|mux|decoder -b WIDTH` an adder, multiplier, multiplexer, or decoder.

### AIG optimization

AIG optimization runs in [aigverse](https://github.com/marcelwa/aigverse). `aig PASS...` applies its passes to
the active AIG in the given order: `rewrite` (cut rewriting), `resub` (resubstitution), `refactor` (SOP
refactoring), `balance` (ESOP balancing), and `cleanup`. A typical flow is

```text
fiction> read mux21.v --type aig; aig rewrite resub balance; map --and --or --inv; ortho
```

(abc-cli)=

### ABC

`abc` hands the active AIG to an external [ABC](https://github.com/berkeley-abc/abc) installation, found on
`PATH` as `abc` or through the `AIGVERSE_ABC` environment variable, and reads the result back. `abc -s resyn2`
runs one of the named scripts `resyn`, `resyn2`, `resyn3`, `compress`, `compress2`, `resyn2rs`, `compress2rs`,
and `dc2`; `abc -c "balance; rewrite; refactor"` runs a command string. The read and write steps are added around
the given commands, and the network travels as an AIGER file, so every gate arrives in ABC as ANDs and inverters.

## Physical design

Physical design turns the active network into a gate-level layout. The exact approach finds minimal layouts for
small networks; the two heuristics scale to large ones. Every result lands in the gate-level layout store, where
`ps -g` prints its statistics:

```text
fiction> ps -g
name           c17
topology       cartesian
clocking       2DDWAVE
size x         5
size y         7
size area      35
inputs         5
outputs        2
gates          8
wires          28
crossings      0
critical_path  11
throughput     1
```

`throughput` is the denominator of the throughput `1/x`; `1` is the maximum.

### SMT-based (`exact`)

`exact` describes the placement and routing problem in first-order logic and lets the SMT solver
[Z3](https://github.com/Z3Prover/z3) find a minimal layout, see
[the paper](https://ieeexplore.ieee.org/document/8342060). Since minimal FCN layouts are an
[NP-complete problem](https://dl.acm.org/doi/10.1145/3312661), only small networks finish in reasonable time.
The most important options:

- `-s SCHEME` fixes the clocking scheme (`2ddwave`, `use`, `res`, `esr`, `cfe`, `ripple`, `srs`, `bancs`,
  `columnar`, `row`, `open`, and their 3-phase and 4-phase variants); this speeds up the search significantly
- `-x` allows wire crossings, which most networks need
- `-b` routes every input and output to the layout border
- `-d` drops global synchronization, which high-fanin networks with border I/O need
- `-t SECONDS` gives up after a timeout
- `--topology cartesian|shifted_cartesian|hexagonal` chooses the layout topology, and `--topolinano` applies
  ToPoliNano's iNML constraints on a shifted Cartesian layout

Solutions are found fastest with crossings, desynchronization, and 2DDWave: `exact -xd -s 2ddwave`. Multi-threading
(`-a THREADS`, `--async-max`) can help for larger networks, but it stops the solver runs from sharing information.

### OGD-based (`ortho`)

`ortho` places and routes with a linear-time orthogonal graph drawing heuristic, see
[the paper](https://dl.acm.org/doi/10.1145/3287624.3287705). It handles large networks, always produces a
2DDWave-clocked Cartesian layout, and needs a network whose gates have at most two inputs (AND, OR, and inverters
after `map --and --or --inv`).

### Graph-oriented layout design (`gold`)

`gold` searches the space of partial layouts with A\*, where every placement is one step, see
[the paper](https://www.cda.cit.tum.de/files/eda/2024_ieee_nano_a_star_is_born.pdf). It handles large networks,
produces 2DDWave-clocked Cartesian layouts, and has the same two-input restriction as `ortho`. Options:

- `-e high_efficiency|high_effort|highest_effort|maximum_effort` sets how many search space graphs to explore
- `-c area|wires|crossings|acp` sets what to minimize; `acp` is the area-crossing product
- `-r` stops at the first valid layout, `-p` forbids crossings, `-m` explores the graphs in parallel
- `-t SECONDS`, `-n EXPANSIONS`, `-s SEED`, `-i` (straight inverters), `-g N` and `-j` (tiles kept after inputs)

### Hexagonalization (`hex`)

`hex` turns the active 2DDWave-clocked Cartesian layout into a hexagonal, row-clocked one, which the Bestagon SiDB
gate library needs, see [the paper](https://ieeexplore.ieee.org/document/10231278). `-i` and `-o` extend the
inputs to the top row and the outputs to the bottom row, and `-p` reroutes the extended pins without crossings.

### Post-layout optimization (`optimize`)

`optimize` shrinks the active 2DDWave-clocked Cartesian layout by moving gates and shortening wires, see
[this paper](https://dl.acm.org/doi/10.1145/3611315.3633247) and
[this paper](https://www.cda.cit.tum.de/files/eda/2024_dac_wiring_reduction_for_field-coupled_nanotechnologies.pdf).
The optimized copy is added to the store; the original stays. `-w` only reduces wiring, which suits very large
layouts, `-m N` moves at most `N` gates (`-m 1` for layouts beyond 100000 tiles), `-p` only moves a gate when no
crossing results, and `-t SECONDS` stops after a timeout.

## Verification

`check` runs the design rule checker on the active gate-level layout, which tests topological and structural
properties, prints every violation, and reports the number of warnings and violations. It is the quickest way
to find bugs in layouts produced by custom algorithms.

`equiv` checks a gate-level layout against a network or two store elements against each other, see
[the paper](https://ieeexplore.ieee.org/abstract/document/9218641). `equiv -n -g` takes the active network as the
specification and the active layout as the implementation; `equiv -n` or `equiv -g` compares the two most recent
elements of one store. The result is one of three:

1. The implementation computes a different function under at least one input: **not** equivalent.
2. The implementation computes the same function at full throughput: **strongly** equivalent.
3. The implementation computes the same function with a delay of some clock cycles: **weakly** equivalent.

## Physical synthesis (`cell`)

`cell -l LIBRARY` compiles the active gate-level layout into a cell-level layout with a gate library:

- `qca-one`, the default, is [QCA ONE](https://ieeexplore.ieee.org/document/7538997/) for Cartesian layouts
- `sim7-mol` is the molecular QCA library for Cartesian layouts
- `topolinano` is [ToPoliNano](https://topolinano.polito.it/)'s iNML library for shifted Cartesian layouts, which
  `exact --topolinano` produces
- `bestagon` is the [Bestagon](https://dl.acm.org/doi/10.1145/3489517.3530525) SiDB library for hexagonal layouts,
  which `hex` produces, or `exact --topology hexagonal -s row -xbd`

Cell-level layouts are much larger than gate-level ones, so `show -c` is the better way to look at them than
`print -c`. `write` exports them for the simulators listed above.

`area [-x WIDTH] [-y HEIGHT] [--hspace H] [--vspace V]` computes the physical area of the active cell-level layout
in nm². Unset dimensions default to the technology's values from [QCADesigner](https://waluslab.ece.ubc.ca/qcadesigner/),
and [NMLSim](https://dl.acm.org/doi/10.1145/3338852.3339856):

| Technology | Width | Height | Horizontal spacing | Vertical spacing |
| ---------- | ----- | ------ | ------------------ | ---------------- |
| QCA        | 18 nm | 18 nm  | 2 nm               | 2 nm             |
| iNML       | 50 nm | 100 nm | 10 nm              | 25 nm            |

SiDB area uses the layout's lattice geometry. Cell-dimension overrides do not apply to SiDB layouts.

## Physical simulation of SiDBs

Physical simulation predicts how an SiDB layout behaves without fabricating it. `read` an `.sqd` file, or produce
a layout with `cell -l bestagon`, and simulate it with one of the engines. Every engine takes the physical
parameters `-e` (relative permittivity $\epsilon_r$, default 5.6), `-l` (Thomas-Fermi screening length
$\lambda_{tf}$ in nm, default 5.0), and `-m` (energy transition level $\mu_-$ in eV, default -0.32). A
simulation adds a new element to the cell-level layout store that carries the result; `print -c` shows its ground
state charges, `show -c` draws them, and `ps -c` reports the number of stable states and the ground state energy.

### QuickExact (`quickexact`)

_QuickExact_ determines every physically valid charge distribution, three orders of magnitude faster than the
exhaustive search of SiQAD through physically informed pruning, partial solution caching, and effective state
enumeration, see [the paper](https://www.cda.cit.tum.de/files/eda/2024_aspdac_efficient_exact_simulation.pdf).
It detects whether two or three charge states per SiDB are needed. `-g` applies a global external potential.

### ClusterComplete (`clustercomplete`)

_ClusterComplete_ is exact as well and scales to layouts with several gates in base 3 by pruning charge
assignments to clusters of SiDBs in a hierarchy. `--base 2|3` sets the charge states, `-w` and `-o` set the
witness partitioning limits of the first pruning stage, and `-r` reports its statistics, which indicate the
complexity of the remaining problem. The command exists when `pyfiction` was built with ALGLIB, as the PyPI wheels
are.

### QuickSim (`quicksim`)

_QuickSim_ approximates the ground state with max-min diversity search space pruning, see
[the paper](https://ieeexplore.ieee.org/document/10231266). `-i` sets the iterations (default 80) and `-a` the
$\alpha$ value (default 0.7).

### Critical temperature (`temp`)

The critical temperature is the temperature at which the ground state's population probability drops below the
confidence level `-c` (default 0.99), see [the paper](https://ieeexplore.ieee.org/document/10231259). `temp`
searches up to `-t` K (default 400). With `-g`, the layout is a gate implementing the active truth table, and
the erroneous states are those with a wrong output. `--engine quickexact|clustercomplete|quicksim|exgs` chooses the
simulator and `--base 2|3` the charge states.

### Operational domain (`opdom`)

The operational domain is the set of parameter points at which a gate implements the active truth table, see
[the paper](https://www.cda.cit.tum.de/files/eda/2023_nanoarch_reducing_the_complexity_of_operational_domain_computation_in_silicon_dangling_bond_logic.pdf).
`opdom FILE` computes it for the active SiDB layout and writes it as CSV. The input BDL pairs of the layout are
assumed to be in the order of the truth table's inputs.

The x and y axes sweep `epsilon_r` from 1 to 10 and `lambda_tf` from 1 to 10 in steps of 0.1 by default;
`-x`, `-y`, and `-z` name the swept parameter of each axis (`epsilon_r`, `lambda_tf`, or `mu_minus`), and
`--x-min`, `--x-max`, `--x-step` and their `y` and `z` counterparts set the ranges. The z axis is unused unless
`-z` names a parameter; its default range is -0.5 to -0.1 in steps of 0.025, made for `mu_minus`.

Grid search evaluates every point. `-r N`, `-f N`, and `-c N` start random sampling, flood fill, and contour
tracing from `N` random samples instead. Grid search and random sampling accept any number of dimensions; flood
fill and contour tracing follow the operational region and need at least two, and in three dimensions contour
tracing collects the boundary surface. `-s` computes the _operational domain sketch_, which judges every point by
filtering alone instead of by simulation: dramatically faster, never rejecting an operational point, but
reporting some non-operational points as operational. The sketch implies kink rejection and needs a layout with
`LOGIC` dots for the filters to enumerate; it pairs best with grid search and random sampling. `-o` writes only
the operational points, and `--engine` chooses the simulator.

## Scripting

`fiction -c "read c17.v; ortho; cell; write c17.qca"` runs a `;`-separated command string and exits. The exit
status is `1` when a command fails, which stops the string at that command. `fiction -f FLOW` runs the commands
of a file, one line per line, in the same way; a missing file exits with `2`. Inside the shell and inside
scripts, `source FILE` runs another script, and scripts may source one another.

Shell loops turn this into batch runs:

```bash
for filepath in benchmarks/TOY/*.v; do
    f="${filepath##*/}"
    fiction -c "read $filepath; ortho; cell; write ${f%.*}.qca"
done
```

### Statistics log

`fiction -l LOG.json ...` writes the statistics of every command to a JSON file when the shell ends. The file
holds a list with one object per command:

```json
[
  {
    "command": "ortho",
    "args": {"verbose": false},
    "started": "2026-09-05T10:12:03+00:00",
    "runtime_s": 0.41,
    "status": "ok",
    "result": {
      "gate_layout": {"name": "c17", "topology": "cartesian", "clocking": "2DDWAVE", "size": {"x": 5, "y": 7, "area": 35}, "inputs": 5, "outputs": 2, "gates": 8, "wires": 28, "crossings": 0, "critical_path": 11, "throughput": 1},
      "stats": {"time_total_s": 0.39, "x_size": 5, "y_size": 7, "num_gates": 8, "num_wires": 28, "num_crossings": 0}
    }
  }
]
```

`args` holds the parsed options. A failing command has `"status": "error"` and an `error` message instead of a
`result`. Every store element is described by one schema wherever it appears, the same one `ps` prints:

- a truth table by `vars`, `hex`, and, up to eight variables, `binary`
- a network by `name`, `type`, `inputs`, `outputs`, `gates`, and `depth`
- a gate-level layout by `name`, `topology`, `clocking`, `size` (`x`, `y`, `area`), `inputs`, `outputs`, `gates`,
  `wires`, `crossings`, `critical_path`, and `throughput`
- a cell-level layout by `name`, `technology`, `size`, `inputs`, `outputs`, and `cells`; an SiDB layout uses
  `dots` for its count and adds `lattice` and `defects`, and a simulated one a `simulation` object with `engine`, `stable_states`,
  `ground_state_energy_ev`, and `runtime_s`

Algorithm statistics appear under `stats` with the attribute names of the bindings' `*_stats` classes; durations
carry an `_s` suffix and are in seconds. Keys are `snake_case` throughout, and the JSON never contains strings
where a number will do.
