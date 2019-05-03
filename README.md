# Let there be a fiction

This code base provides a framework for **fi**eld-**c**oupled **t**echnology-**i**ndependent **o**pen **n**anocomputing
in C++14 using the [EPFL Logic Synthesis Libraries](https://github.com/lsils/lstools-showcase). *fiction* focuses on physical
design of emerging nanotechnologies. As a promising class of post-CMOS technologies,
[Field-coupled Nanocomputing (FCN)](https://www.springer.com/de/book/9783662437216) devices like Quantum-dot Cellular
Automata (QCA) in manifold forms (e.g. atomic or molecular), Nanomagnet Logic (NML) devices, and many more, allow for high
computing performance with tremendously low power consumption without the flow of electric current.

With ongoing research in the field, it is unclear, which technology will eventually be large scale fabricable.
Therefore, *fiction* is able to perform placement and routing tasks for specific tile-based layouts on a data structure
that abstracts from physical cell design. Using an extensible set of gate libraries, technologies, and cell types, these
can be translated easily into what ever FCN technology is desired.

### Technologies

So far, created layouts can be converted into QCA circuits using a custom version of the
[QCA-ONE](https://ieeexplore.ieee.org/document/7538997/) gate library. Those can be written as files for
[QCADesigner](https://waluslab.ece.ubc.ca/qcadesigner/) to run physical simulations.

More technologies and libraries are to come!

### Layout Algorithms

An [exact approach](http://www.informatik.uni-bremen.de/agra/doc/konf/2018DATE_ExactMethodforDesignExplorationOfQCA.pdf)
in terms of circuit area is provided which is the first one ever in the field of FCN circuits.
It is based on Satisfiability Modulo Theories (SMT) and utilizes the solver engine [Z3](https://github.com/Z3Prover/z3)
by Microsoft Research. It is only feasible for small circuits but provides the best results.

Additionally, a [scalable method](http://www.informatik.uni-bremen.de/agra/doc/konf/2019_ASP-DAC_Scalable_Design_for_Field-coupled_Nanocomputing_Circuits.pdf)
is implemented which is based on Orthogonal Graph Drawing (OGD). It has a huge runtime advantage compared to the SMT technique.
Though, its results are not optimal in terms of area. It is feasible even for larger circuits and provides results real quick.

This is ongoing research but more algorithms are to come!

## Building process

Git, g++, cmake and the Boost libraries are necessary in order to build *fiction*. Since a Python interpreter and
GNU readline are utilized by some dependencies, it is also recommended to set them up.
If you are on *Ubuntu*, you should be fine installing them with this command:

```sh
sudo apt-get install git g++ cmake libboost-all-dev python libreadline-dev
```

Note that there is no guarantee that your system does not lack some required packages which are not listed here!
CMake will inform you about missing dependencies during the build process.

Check out the git project (and all of its submodules) using the following command:

```sh
git clone --recursive https://github.com/marcelwa/fiction.git
```

Several third-party libraries will be cloned within the `libs/` folder. The `cmake` build process will take care of
them automatically.

Should you have cloned the repository before, `git submodule update --init --recursive` will fetch the latest version of
all external modules used.

Afterwards, *fiction* is ready to be built. Simply enter the following commands:

```sh
cd fiction
mkdir build
cd build
cmake ..
make
```

This process may take a while!

You have the choice to change the `cmake` call to `cmake -DCMAKE_BUILD_TYPE=Debug ..` if you prefer building with debug
information. The build mode can also be toggled via the `ccmake` CLI. Note that building with debug information will
have a significant negative impact on the runtime.

### Building on Windows Subsystem for Linux (WSL)

To build *fiction* on Windows, WSL has to be installed. Instructions can be found on
[the official Windows page](https://docs.microsoft.com/en-us/windows/wsl/install-win10). During installation, it is highly
recommended to pick *Ubuntu* as the Linux distribution. Then, simply follow the build steps listed above on your WSL.

If you want to use [Visual Studio](https://visualstudio.microsoft.com/de/) (VS) to work with the source code, have a look at
how to [Target WSL from VS](https://devblogs.microsoft.com/cppblog/targeting-windows-subsystem-for-linux-from-visual-studio/),
[Cross Platform C++ with VS](https://devblogs.microsoft.com/cppblog/using-visual-studio-for-cross-platform-c-development-targeting-windows-and-linux/),
and [CMake with VS](https://docs.microsoft.com/en-us/cpp/linux/cmake-linux-project?view=vs-2017).

### Troubleshooting

On some CMake systems, it might be necessary to type

```sh
make z3
make fiction
```

after the `cmake` call instead of just `make` to guarantee the correct order of build steps. Try this if the initial
call leads to some kind of error messages revolving around Z3.

#### Troubleshooting while using the WSL

If you cannot install `libboost-all-dev` (the error might look like this: `Unable to locate package libboost-all-dev`) 
it should be fixed by adding `universe` to your repositories and trying again:

```sh
sudo add-apt-repository universe
sudo apt-get update
sudo apt-get install libboost-all-dev
```

Note that it is possible that *alice* does not find the readline-library even though it is 
installed. CMake will inform you about this with a warning: 
```
CMake Warning at libs/alice/include/CMakeLists.txt:35 (message):
  Cannot find readline library, fall back to native interface.
```
There is no solution to this as of yet and it may lead to missing tab-completion while interacting with *fiction*.

## Usage

The build process generates an executable called `fiction`. A simple non-exhaustive workflow is described in the
following. To get some information about the available commands, run `fiction` with a help flag:

```sh
./fiction --help
```
Starting the interactive mode by entering `./fiction` greets the user with a prompt. Again, input of `help` produces a
list of available commands. You can generate a plain text documentation by entering `help --docs <filename>`. 

### Preface

*fiction* utilizes the CLI [alice](https://github.com/msoeken/alice) by Mathias Soeken. It provides a flexible
store-based architecture. A store for logic networks holds specifications which then can be transformed into placed
and routed FCN circuit layouts. Specifications can be provided in terms of synthesized Verilog netlist files. Using the
tool [ABC](https://github.com/berkeley-abc/abc) for example, such netlist files can be generated by

```sh
read spec.bench
strash
write spec.v
```

ABC supports numerous file formats from which `.bench` is just an example one. *fiction* also comes with a lot of preset
netlists in the `benchmarks` folder.

### Stores

When you are in interactive mode, enter `read <filename>` to read a logic network into a store or `read <directory>` to
parse all parsable files within that given directory powered by the [lorina](https://github.com/hriener/lorina) parser
by Heinz Riener. The flag `-s` allows prior sorting. The content of the logic network store can be briefly viewed by
entering `store -w` whereas `print -w` writes a [Graphviz](https://www.graphviz.org/) dot file of the current network
to the standard output. Arbitrarily many logic networks can be held in store from which the latest is always the
active one. Change active network with `current -w <n>` where you replace `<n>` by the number of the store element you
want to activate.

### Placement & Routing

Initiate an exact placement and routing call with the active logic network element in store by `exact`. This approach
has several flags and options which can be viewed in detail by `exact --help`. The approach relies on a series of SMT
solver calls to which `-u 20` defines an upper bound of 20 FCN tiles. Note that since the approach is exact, only small
networks can be placed and routed in reasonable runtime. Another call of `exact -u 25 -xaib` would try to place
and route the current network on 25 FCN tiles, utilizing crossings (`-x`) and artificial latches (`-a`) while also
balancing I/O paths to internally synchronize the circuit using designated pins (`-i`) plus routing all I/Os to the
grid borders for easier access (`-b`).

Additionally, you can provide a pre-defined clocking scheme to prune the search space with option `-s`. Possible schemes
besides the default open clocking, are [2DDWave](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=1717097),
[USE](https://ieeexplore.ieee.org/stamp/stamp.jsp?arnumber=7219390),
[RES](https://www.tandfonline.com/doi/pdf/10.1080/21681724.2019.1570551?needAccess=true), and
[BANCS](https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8533251&tag=1). For detailed information on usage see
`exact -h`. Additional clocking schemes can be implemented in the header file `fcn_clocking_scheme.h`.

Calls to `ortho` are not expecting flags. This algorithm based on orthogonal graph drawing produces layouts in much
shorter time. Though their quality in terms of area is far from optimum. Plus, for physical implementation, it might be
necessary to route primary inputs/outputs to the grid borders by hand.

Generated FCN gate layouts are also saved in stores. Entering `store -g` shows a list of all gate layouts available.
Statistical information about store elements can be printed using command `ps`. For a gate layout, a call to `ps -g`
could for instance output the following.
```
c17 - 5 × 7, #G: 18, #W: 18, #C: 3, #L: 0, CP: 11, TP: 1/1
```
It displays from left to right the name of the layout, the dimension in tiles, the number of gate tiles (including
designated I/O pins and fan-outs), wire elements, crossings, latches
([synchronization elements](http://www.informatik.uni-bremen.de/agra/doc/konf/2018NANO_SynchronizationOfClockedFieldCoupledCircuits.pdf)),
the length of the critical path, and the throughput of the design (where 1/1 is maximum throughput).

### Physical synthesis

To synthesize a gate layout to a cell level one, type `cell`, where the `-l` flag indicates the gate library to
use. Currently, only QCA-ONE is available which is the default setting. Cell layouts are also saved in stores which can
be accessed by typing `store -c`. Due to significantly larger size of cell layouts compared to gate layouts, the
`print -c` command to write layouts to the terminal should be used carefully. See [SVG export](#svg-export) instead.

Nevertheless, you can use `qca <filename>` to create a QCADesigner file for running physical simulations. If no
filename is given, the stored layout name will be used and the file will be written to the current folder.

### SVG export

Cell level layouts can be exported as scalable vector graphics (`.svg` files) using the command `show -c`. It will open
your standard SVG program immediately to give you a more sophisticated visual representation of the current cell layout
in store. If you want to just generate the SVG file without opening it in your standard viewer, use
`show -c <filename>.svg --silent`.

Alternatively, open the exported file with a different program by using `show -c --program "\"google-chrome\" {}"` for
instance to open it with your Chrome browser. Note, that this behavior is platform dependent and might not work properly
under your system. For more information see `show --help` or the full
[alice documentation](https://libalice.readthedocs.io/en/latest/index.html).

The used color scheme is based on the one known from [QCADesigner](https://waluslab.ece.ubc.ca/qcadesigner/).

There is also an option for showing a simple version of cells that leaves out the quantum dots and clock zone numbers.
This makes the exported files significantly smaller, especially for large layouts. The corresponding flag is `-s`.

### Automation by scripting

All steps can also be specified within a *fiction script* file like the following called `c17_synth.fs`

```sh
read ../benchmarks/ISCAS85/c17.v
ps -w
exact -xp
print -g
ps -g
cell
ps -c
qca
show -c c17.svg --silent
```

which can be executed by `./fiction -ef c17_synth.fs -l c17_log.json` where statistics are to be logged in a JSON file
called `c17_log.json`.

These scripts can also be nested. Use `< script.fs` within a *fiction script* to load `script.fs` in that very position.
A script called `shortcuts.fs` has been placed in the top level folder. It can be loaded on start-up by calling
`./fiction -if ../shortcuts.fs` in the build folder. This makes predefined commands and flows available as shortcuts.
Try `synth xibs use` for instance to perform the whole flow of layouting (utilizing `USE` clocking) and physical
synthesis down to cell level including visual representation.

Additionally, *fiction* itself can be part of a bash script. Consider the following snippet

```sh
for filepath in ../benchmarks/TOY/*.v; do
    f="${filepath##*/}"
    ./fiction -c "read $filepath; ortho; cell; qca ${f%.*}.qca"
done
```

where the for-loop iterates over all Verilog files in the `../benchmarks/TOY/` folder. Using the flag `-c`, a
semicolon-separated list of commands can be entered in *fiction*. In this case, the files are to be read in a store,
placed and routed using the `ortho` algorithm, synthesized to cell level, and written as QCA using their original file
name.

## Uninstall

Since all tools were built locally, simply delete the git folder you have cloned initially to uninstall this project.

## Ongoing research

*fiction* is part of my PhD Thesis entitled "Layout of Large Scale Field-coupled Nanocomputing Circuits" (working title).
Please find my publication list [here](http://www.informatik.uni-bremen.de/agra/eng/pub.php?search=Marcel%20Walter).
Should you be interested in a collaboration or an academic exchange, feel free to reach out at any time.

## Acknowledgments

I would like to thank Frank Sill Torres for introducing me to FCN and for countless helpful discussions and support
with this framework. Also, I thank Gregor Kuhn for implementing the SVG export and for reporting troublesome bugs; and
Mario Kneidinger for code contributions, pointing out the Visual Studio resources and documentation inconsistencies
on my side.

## Disclaimer

This is academic software made available under the CRAPL license. For more information see the LICENSE file.
