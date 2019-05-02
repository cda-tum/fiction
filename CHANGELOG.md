# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## v0.2.1 - 2019-05-02
*Fiction is art and art is the triumph over chaos.* &mdash; John Cheever

### Added
- Support for BANCS clocking and integration in `exact`
- Name strings for `fcn_clocking_scheme` objects and corresponding name-based look-up
- Version and build information accessible within the code by including `version.h`
- Parameter `-i` for command `ortho`
- `shortcuts.fs` with predefined flows
- `benchmarks/MAJ/` folder with some TOY benchmarks using MAJ gates

### Changed
- Calls to `exact -s` now need to name the desired clocking, e.g. `exact -s use` (case insensitive)
- `incoming`/`outgoing_information_flow_tiles` have been renamed to `incoming`/`outgoing_data_flow` and handle multi wires now
- Renamed diagonal clocking schemes to 2DDWAVE and gave proper credit.
- More verbose error messages

### Fixed
- TP calculation for layouts without designated I/O pins (thanks to Mario Kneidinger!)
- I/O port orientation of PI/PO gates using QCA-ONE library
- Usage of non-PI/PO MAJ gates in QCA-ONE library
- Visualization of clock latches in `show -c` (thanks to Gregor Kuhn!)
- Multi direction assignment to wires and gates in `exact` leading to physically impossible layouts
- `shrink_to_fit` in `fcn_gate_layout` incorporates the BGL bug now. Minimum size in each dimension is 2.
For more information, see <https://svn.boost.org/trac10/ticket/11735>
- Parameters for `exact` no longer get stuck once set

### Removed
- Parameter `-n` for `exact`

## v0.2.0 - 2019-03-21
*Fiction reveals the truth that reality obscures.* &mdash; Jessamyn West
### Added
- Export `fcn_cell_layout` objects as SVG using `show -c`. See README for more information
- `ps -g` displays and logs critical path and throughput of `fcn_gate_layout` objects
- Support for RES clocking and integration in `exact` 
- New TOY benchmarks in respective folder
- New command `clear` to remove all elements from stores (as a shorthand for `store --clear ...`)
- Information about how to build fiction for WSL
- Functions to distinguish different `logic_network` types like AIGs/MIGs/...
- Parameter `-n` for command `ortho`

### Changed
- Revised folder structure due to the increasing amount of source files
- `read_verilog` is now called `read` and can process directories
- `gate_to_cell` is now called `cell`
- `write_qca` is now called `qca` and handles file names automatically if necessary
- `-u` is not a required parameter for `exact` anymore
- Richer output for `print -w`
- Included latest bugfixes for [alice](https://github.com/msoeken/alice)
- Included latest update for [lorina](https://github.com/hriener/lorina)

### Fixed
- Starting layout size for calls to `exact -i` was too low and has been corrected
- Several code and comment inconsistencies

### Removed
- ITC99 benchmark files


## v0.1.1 - 2018-12-29
*Literature is a luxury; fiction is a necessity.* &mdash; G. K. Chesterton
### Added
- Technology-specific energy model for `fcn_gate_layout`; supports QCA thus far
- Support for `print -c` to write a textual representation of `fcn_cell_layout` objects
- Information on nested fiction scripts and documentation generation in README
- *linguist* flags in `.gitattributes` to prevent benchmark files from being viewed as source code

### Changed
- Moved to version 0.4 of [Alice](https://github.com/msoeken/alice)
- Moved to version 4.8.4 of [Z3](https://github.com/Z3Prover/z3)
- `fcn_gate_library` objects now have name strings
- `print -g` now displays incorrectly assigned directions by bidirectional arrows
- "Release" is the standard build mode now

### Fixed
- Copy and move constructors of `logic_network` work properly now
- Calculation of `bounding_box` size on `fcn_gate_layout` now handles empty layouts correctly
- Several minor and rare bugs, code inconsistencies, and performance issues

### Removed
- Nothing

## v0.1.0 - 2018-10-29
*Let there be a fiction*

This is the initial release. Please find a feature overview in the README.
