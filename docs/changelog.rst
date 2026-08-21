Changelog
=========

All notable changes to this project will be documented in this file.

The format is based on `Keep a Changelog <https://keepachangelog.com/en/1.0.0/>`_.

Unreleased
----------

Added
#####
- Algorithms:
    - Added the ``missing_required_gates_exception`` that ``technology_mapping`` throws when the
      technology library is missing the gates required by the base network type (AIG requires INV and
      AND; XAG requires INV, AND, and XOR; MIG requires INV and MAJ)
    - Added an ``is_operational`` overload that takes one layout per input pattern, and
      ``generate_bdl_input_pattern_layouts`` to generate them
    - Added a ``critical_temperature_gate_based`` overload that takes the pre-generated input pattern
      layouts and BDL detection results instead of deriving them from the layout
    - Added ``number_of_threads`` to ``operational_domain_params``, ``defect_influence_params``, and
      ``displacement_robustness_domain_params``, defaulting to the number of hardware threads, which
      leaves the previous behavior unchanged. Pinning it makes runtime comparisons reproducible and
      lets a computation leave cores free
    - ``operational_domain_contour_tracing`` now supports three or more sweep dimensions, where it
      collects the boundary surface instead of walking a closed curve. ``operational_domain_flood_fill``
      no longer caps at three dimensions
    - Added ``cell_layout_digest``, which hashes a cell-level layout in agreement with
      ``are_cell_layouts_identical`` so that callers can group candidates before comparing them exactly
- Build system:
    - Added ``-DFICTION_ENABLE_TIME_TRACE=ON`` to emit Clang ``-ftime-trace`` compilation profiles
- CLI:
    - Added ``opdom --sketch/-s``, which determines the operational status by filtering instead of by
      physical simulation. It implies kink rejection, since the filtering steps are only defined there
- Continuous integration:
    - The 🐍 Packaging jobs now run ``check-sdist --inject-junk``, which fails if the source
      distribution drops a tracked source or ships an untracked one
    - Added a 🐍 Lint job that runs the ``mypy`` hook, which pre-commit.ci no longer runs
    - Added a 🐍 Minimums job that runs ``nox -s minimums``, so a lower bound that is too low fails
      CI instead of a downstream install
- Data structures:
    - Added a ``std::hash`` specialization for ``fiction::sidb_defect``
    - Added ``hash_combine_unordered``, which folds hash values commutatively and therefore suits
      containers whose iteration order is not canonical
- Experiments:
    - Added ``operational_domain_3d_bestagon_grid_vs_sketch``, which compares grid search against the
      operational domain sketch over a three-dimensional parameter space
- Gate libraries:
    - Added ``sim7_mol_library`` and ``mol_qca_technology`` for applying the SIM(7)-MolPDK
      molecular QCA standard-cell library to gate-level layouts, including QLL/SVG export support,
      Python bindings, and tests
- Python bindings:
    - Exposed ``generate_bdl_input_pattern_layouts`` and the new ``is_operational`` and
      ``critical_temperature_gate_based`` overloads
    - Exposed ``number_of_threads`` on ``operational_domain_params`` and
      ``displacement_robustness_domain_params``
    - Exposed ``mol_qca_technology``, ``mol_qca_layout``, ``write_mol_qca_layout_svg``, and
      ``apply_sim7_mol_library``
    - Exposed ``state_type``, which makes ``calculate_energy_and_state_type_with_kinks_accepted``/``_rejected``
      and ``occupation_probability_gate_based`` callable from Python
- Tooling:
    - Added the ``license-tools`` prek hook, which puts an MIT copyright header on every Python
      file and rewrites any that departs from the canonical text
    - Added the ``minimums`` nox session, which runs the Python test suite on Python 3.10 against
      the lowest declared direct dependencies instead of the newest compatible ones

Changed
#######
- Algorithms:
    - ``technology_mapping`` and the ``map`` command now default to ``mockturtle::emap`` instead of
      ``mockturtle::map``
    - **Breaking:** ``technology_mapping_params::mapper_params`` is now a ``mockturtle::emap_params``
      (was ``mockturtle::map_params``) and ``technology_mapping_stats::mapper_stats`` is now a
      ``mockturtle::emap_stats`` (was ``mockturtle::map_stats``)
    - **Breaking:** the ``map`` command now warns when remapping an already-mapped network and reports
      mapping errors instead of storing a failed mapping
    - ``operational_domain`` and ``critical_temperature_domain`` now generate the input pattern
      layouts once instead of once per sample point. SiQAD grid search gets about 10% faster; larger
      gates are dominated by the physical simulation and gain little
    - Parallelized ``operational_domain_flood_fill`` over a pool of worker threads. The result is
      independent of exploration order and therefore unchanged
    - ``critical_temperature_domain`` now reuses the operational domain's input pattern layouts and BDL
      detection results instead of re-deriving them for every parameter point
    - ``is_operational`` now builds the canvas charge distribution surface once per call instead of once
      per canvas enumeration, which the ``FILTER_ONLY`` and ``FILTER_THEN_SIMULATION`` strategies repeat
      for every combination of input kink state and output pattern
    - ``bdl_input_iterator`` now determines the BDL dot distances that decide the input assignment once
      in its constructor instead of on every increment; they cannot change over its lifetime
    - ``is_operational_impl`` no longer detects the output BDL pairs twice under ``TOLERATE_KINKS``
    - ``quicksim`` and ``multi_simulated_annealing`` no longer pass a parallel execution policy to
      their ``std::find`` and ``std::min_element``, which scan a handful of elements where the
      dispatch costs an order of magnitude more than the scan
    - ``generate_multiple_random_sidb_layouts`` now rejects duplicate candidates through a digest
      lookup instead of comparing each candidate against every layout it has already collected.
      Collecting 4000 layouts of 10 SiDBs takes about 7 ms instead of 160 ms
- Build system:
    - Bumped the required C++ standard from C++17 to C++20
    - Fetch dependencies as release archives instead of git clones, which cuts ``tests-slim``'s
      ``_deps`` from 504 MB to 262 MB. ``mockturtle`` stays a clone because it uses a submodule
    - The ``ci-debug``, ``dev``, and ``tests-slim`` presets now enable
      ``FICTION_LIGHTWEIGHT_DEBUG_BUILDS``, which cuts Debug compile time and memory substantially
    - ``FICTION_ENABLE_PCH`` now covers the test suite as well as the CLI, and is on in the ``dev``
      and ``tests-slim`` presets
    - The CI presets no longer build the experiments; the 🧪 Experiments job compiles them instead
    - ``vcs-versioning`` replaces ``setuptools-scm`` for deriving the version from git, which drops
      ``setuptools`` from the build. Building from source now requires ``scikit-build-core`` 1.0
    - The CMake policy range now ends at 4.4
- Code quality:
    - Pruned the include graph of the most widely included headers, keeping ``nlohmann/json.hpp``,
      ``fmt``, and the vendored ``combinations.h`` off the path that ``traits.hpp`` pulls in
    - **Breaking:** moved ``determine_all_combinations_of_distributing_k_entities_on_n_positions``
      from ``fiction/utils/math_utils.hpp`` to the new ``fiction/utils/combination_utils.hpp``.
      Include the latter to keep using it
    - ``orthogonal`` and ``graph_oriented_layout_design`` no longer template their implementation on
      the specification network type, which they convert away before doing any work. Their public
      entry points are unchanged
    - Modernized the entire code base for C++20, adopting ``std::ranges`` algorithms, concepts,
      defaulted comparison operators, and designated initializers throughout
    - Replaced unchecked ``operator[]`` with bounds-checked ``at()`` in the operational domain module
    - The type checks on the path-finding, operational domain, and ``hexagonal_layout`` entry points
      are now ``requires`` clauses. An unsatisfied one is reported as a failed constraint at the call
      site instead of a ``static_assert`` message from inside the body
    - ``hexagonal_layout`` now rejects an invalid ``HexagonalCoordinateSystem`` where the type is
      named rather than where it is constructed
    - ``exact`` no longer polls its worker futures every 10 ms while solving asynchronously
    - ``exact`` now surfaces a failure in one of its asynchronous workers instead of reporting it
      as "no layout found"
    - Cleared the pre-existing Clang-Tidy findings in ``exact.hpp``
    - Migrated the ``pyfiction`` test suite from ``unittest`` to pytest and enabled ruff's ``PT``,
      ``PTH``, and ``E501`` rule sets. The suite now fails on warnings
    - Every Python file now carries ``from __future__ import annotations``, which ruff's
      ``future-annotations`` setting had assumed of all of them and only six of them had
    - Retired ruff's TODO ignore list. Every entry that remains states a decision in a comment,
      including ``CPY001``, which stays off because the ``license-tools`` hook enforces the
      headers instead
    - ``mypy`` now checks every Python file the repository owns, where it previously checked only
      the bindings and ``noxfile.py``
- Continuous integration:
    - The docstring generator now parses with a pinned libclang, ``-std=c++20``, and the include
      paths and defines of a configured build. Parse errors drop from about 180 to zero, so a
      ``requires`` clause no longer silences the Doxygen comments that follow it
    - The twelve workflows are consolidated into ``ci.yml`` and ``cd.yml`` over a set of callable
      workflows. Change detection decides what runs, and a single ``🚦 Check`` job aggregates the
      outcome
    - **Action required:** the workflows that produced the former status contexts are gone, so an
      administrator must repoint branch protection at ``🚦 Check`` together with
      ``🔍 Change / 🕵️ Check``, ``pre-commit.ci - pr``, and ``docs/readthedocs.org:fiction``. Never
      require an individual matrix leg; ``🚦 Check`` covers them and survives a matrix change
    - The ``ci-*`` presets now reuse the precompiled header that the ``dev`` preset already used,
      which the 120 single-source test executables each paid for separately before
    - The build jobs no longer cap themselves at two hours. Three of them had started failing on
      that cap rather than on a regression; the short jobs keep a ceiling
    - Publishing moved out of CI: only ``cd.yml``, which runs on a published release, holds the
      credentials for PyPI, Docker Hub, and ghcr.io
    - The wheel jobs now run whenever anything the extension is compiled from changes, not only
      the packaging metadata. ``cibuildwheel`` runs the Python test suite, so this is what
      decides whether it runs at all
    - Updated the Ubuntu compiler matrix for C++20: dropped ``g++-10``, ``clang++-14``, and
      ``clang++-15``, and added ``clang++-19`` and ``clang++-20``
    - Halved the OS matrices; ``docs/getting_started.rst`` records the combinations we verify
    - The wheel builds now run the ``pyfiction`` test suite against the repaired wheel instead of
      only smoke-testing the import
    - ``prek`` now validates ``.codecov.yml`` against its schema
    - The ``prek`` hooks now carry explicit priorities, which run the read-only checks
      concurrently and cut a warm ``prek run -a`` by about 18%. Hooks that rewrite the same file
      keep exclusive priorities, since ``prek`` runs hooks that share one concurrently
    - The docstring generator runs on ``main`` only. It pushes with a token that starts no workflow
      run, so its commit on a pull request head would carry no ``🚦 Check``
    - Publishing to PyPI runs in a ``pypi`` deployment environment, which is where a required
      reviewer or a wait timer on releases would go
- Dependencies:
    - **Breaking:** raised the declared ``z3-solver`` floor from 4.8.0 to 4.8.5, which is the
      version ``find_package(Z3 4.8.5)`` has required all along. A pin between 4.8.0 and 4.8.4 no
      longer resolves
- Documentation:
    - The README's six per-workflow status badges are replaced by one ``CI`` and one ``CD`` badge,
      matching the two workflows that remain
    - ``docs/contributing.rst`` describes the checks a pull request now reports and what
      ``🚦 Check`` means
- Experiments:
    - Modernized ``generate_defective_surface.py`` now that ``experiments/AGENTS.md`` opens the
      directory to it. Apart from the out-of-bounds fix below, the surface it generates is unchanged
    - ``generate_defective_surface.py`` is executable and declares its dependencies in a PEP 723
      block, so running it needs nothing installed first
    - ``generate_defective_surface.py`` now writes the surface it generates, which it previously
      discarded, and takes the parameters it used to hard-code as command-line arguments
- Python bindings:
    - **Breaking:** generated docstring symbols are now named ``mkd_doc_*`` instead of the reserved
      ``__doc_*``. ``DOC(...)`` is unchanged, but hand-written docstrings that define such a symbol
      directly must be renamed
    - ``mnt.pyfiction`` now advertises developers and information technology as intended audiences
      and physics as a topic on PyPI

Removed
#######
- Algorithms:
    - Removed Mugen support: the vendored Glucose SAT solver (``vendors/mugen/``), the ``onepass`` CLI
      command, and ``one_pass_synthesis()``. Use ``exact_physical_design()`` instead
    - Removed ``jump_point_search()``. Use ``a_star()`` instead
    - Removed ``qca_energy_dissipation()`` and the ``energy`` CLI command
- Build system:
    - Removed ``FICTION_ENABLE_UNITY_BUILD``, which set a non-propagating property on an
      ``INTERFACE`` target and therefore never did anything
- CLI:
    - Removed the ``--logic_sharing`` flag from ``map``, which ``mockturtle::emap`` does not support
- Continuous integration:
    - Removed the 🐍 CI workflow; the wheel builds now cover the same ground. ``nox -s tests``
      remains the local entry point
- Data structures:
    - Removed ``range_t`` (``fiction/utils/range.hpp``); ``cartesian_layout``'s and ``hexagonal_layout``'s
      ``coordinates()``/``ground_coordinates()`` now return a ``std::ranges::subrange`` instead, with no
      change in usage

Fixed
#####
- Algorithms:
    - Requesting the operational domain sketch (``FILTER_ONLY``) without ``REJECT_KINKS`` or on a layout
      without ``LOGIC`` cells now throws instead of silently falling back to a full simulation of the
      entire parameter space
    - Fixed a division by zero in the parallel operational domain, defect influence, and displacement
      robustness helpers, which derive their slice size by dividing by a worker count that is zero when
      there is no work at all. ``operational_domain_random_sampling`` with ``samples = 0`` reached it
    - Fixed ``is_operational`` reporting every layout operational without checking it when ``FILTER_ONLY``
      was combined with ``TOLERATE_KINKS``. All entry points now decide canvas filtering in one place
    - Fixed the enclosure inference of ``operational_domain_contour_tracing``, which an inverted guard
      had left permanently inactive. Its flood fill is now bounded by the traced contour and expands over
      the von Neumann neighborhood, so it can no longer suppress the tracing of other operational islands
    - Fixed an off-by-one in the boundary search of ``operational_domain_contour_tracing``, which skipped
      the first sweep dimension's lowest step index. Where the operational region reached that edge, the
      traced contour came out empty and every reachable point was marked operational without simulation
    - Fixed a data race on ``quicksim``'s timeout flag, which every worker thread wrote as a plain
      ``bool``. It is now ``std::atomic_bool``
- Build system:
    - ``.gitignore`` now covers the build, virtual environment, and editor droppings that a local
      ``uv build --sdist`` would otherwise ship into the source distribution
    - Fixed the source distribution shipping none of the C++ sources it needs to build, which made
      ``pip install mnt.pyfiction`` fail wherever no matching wheel exists
- Code quality:
    - Fixed the execution-policy guard in ``execution_utils.hpp``, which read the feature-test macros
      before including ``<version>`` and misread Clang's ``__GNUC__`` of 4 as an old GCC. Parallel STL
      algorithms were disabled on every Clang build
    - Fixed several ``fmt`` compile-time format-string misuses surfaced by the C++20 bump
    - Fixed ``std::string_view::data()`` calls that assumed null termination, which ``std::string_view``
      does not guarantee
    - Fixed plain ``char`` values being passed to ``::toupper``, ``::tolower``, ``::isdigit``, and
      ``::isxdigit``, which is undefined behavior on platforms with a signed ``char``
    - Fixed a signed-integer overflow in ``to_siqad_coord`` for the minimum representable ``y`` value
- Continuous integration:
    - Renamed ``.readthedocs.yml`` to ``.readthedocs.yaml``. The preview guard and the
      ``check-readthedocs`` hook both expect that name, so editing the file now rebuilds the
      documentation preview instead of skipping it
    - Publishing the Docker image no longer clears its Docker Hub short description, which it
      derived from a field that never held one
    - Fixed the Renovate ``github-tags`` custom managers to reference ``owner/repository`` package names
      instead of full GitHub URLs, which the datasource requires to resolve tags
    - Fixed patch-level CMake ``GIT_TAG`` bumps being eligible for Renovate's automerge
    - Pinned the vendored ``alice`` dependency's ``GIT_TAG`` to a fixed commit carrying a C++20 fix,
      instead of floating on ``master``
    - Fixed ccache being skipped on every ``ubuntu-24.04-arm`` job, leaving the slowest runners cold
    - Fixed the CodeQL ccache key interpolating an undefined ``matrix.os``
    - The CodeQL jobs now declare their build mode: ``manual`` for C++, whose database comes
      from the CMake build, and ``none`` for Python, which has no build. Python stops warning
      that it cannot build an overlay database; C++ still does, since a leg that builds
      cannot have one
    - Fixed the packaging path filter still pointing at the removed ``bindings/pyfiction/**``
- Documentation:
    - Fixed the ``doc_overview_table`` directive rendering ``None`` in the description column for a
      function without a Doxygen brief description
- Experiments:
    - **Breaking:** fixed the always-false out-of-bounds guard in ``generate_defective_surface.py``
      that let a defect be placed clipped at the surface edge. The generated surface changes
    - ``generate_defective_surface.py`` now rejects a coverage outside ``[0.0, 1.0]`` and a
      non-positive surface dimension, where it used to write an empty surface and report success
- Python bindings:
    - Fixed the ``sidb_defect`` ``operator!=`` binding, which referenced a docstring symbol that is no
      longer emitted now that the operator is compiler-synthesized
    - Fixed nine ``DOC(...)`` references that named symbols the broken parse had invented; the
      ``*_stats`` runtime members are now documented under their real names
    - Fixed ``bdl_input_iterator.py`` never being collected, as its name did not match pytest's
      ``python_files`` pattern, so its five tests had never run
v0.7.0 - 2026-07-31
-------------------

.. epigraph::

   Cliché or not, "stranger than fiction" expresses exactly how I feel about the truth.

   -- Richard Dawkins

Added
#####
- Documentation:
    - Added ``AGENTS.md`` to guide AI agents in the repository
- Build system:
    - Added ``CMakePresets.json`` with presets for common local development workflows (``dev``,
      ``dev-asan``, ``dev-full``, ``release``, ``deploy``, ``coverage``) as well as CI-mirroring presets
      (``ci-debug``, ``ci-release``, ``ci-codeql``, ``ci-tidy``) that let CI failures be reproduced locally
      with a single ``cmake --preset <name>`` invocation
    - Added support for CMake version 4+
- Tooling:
    - Added the following `pre-commit` hooks:
        - `check-vcs-permalinks`
        - `check-symlinks`
        - `check-json`
        - `cmake-format-precommit`
        - `uv-pre-commit`
    - Enabled auto-merging stable non-major dependency releases via Renovate
- Continuous integration:
    - Added a 🌈 Zizmor workflow that statically analyzes the GitHub Actions workflows for security
      issues on every push, pull request, and merge group, and uploads findings to the code scanning
      API; fork pull requests print findings to the log instead, since their read-only token can't
      upload to it

Changed
#######
- Build system:
    - Restructured the CLI command implementation to improve code organization, modularity, and compilation speed
    - Refactored the entire CMake build system to use ``FetchContent`` for dependency management instead of git submodules
    - Moved vendored libraries from ``libs/`` to ``vendors/``
    - Improved build configuration and option handling for better modularity
    - Addressed several CMake and compiler warnings (including CMP0148 and Pybind11 compatibility) as well as CMake code smells
    - Streamlined package installation and discovery process
- Continuous integration:
    - The 🐧/🍎/🪟 CI, ☂️ Coverage, and 📝 CodeQL workflows, the Clang-Tidy Review workflow, and the Docker
      build now configure via the new CMake presets instead of hand-rolled ``-D`` flag lists
    - The 🔖 Release Drafter workflow now runs on the smaller ``ubuntu-slim`` runner instead of
      ``ubuntu-latest``, since it only calls the GitHub API and needs no compiler toolchain
    - Hardened every workflow against the findings from the new 🌈 Zizmor workflow: scoped
      ``permissions`` blocks to the minimum each job needs (instead of relying on the broad default),
      set ``persist-credentials: false`` on checkouts that don't push, routed matrix/workspace values
      through ``env`` vars instead of interpolating them directly into shell commands, and disabled
      the PyPI packaging workflow's build cache to close a cache-poisoning path into published
      artifacts
- Python bindings:
    - Restructured the ``pyfiction`` bindings to use multiple translation units (one ``.cpp`` file per
      binding) for better modularity and faster incremental compilation
- Tooling:
    - Replaced ``pre-commit`` with ``prek``, a faster Rust reimplementation that stays compatible with
      ``pre-commit.ci``
- Dependencies:
    - Updated all dependencies to their latest versions

Fixed
#####
- Code quality:
    - Addressed several ``clang-tidy`` warnings throughout the code base
    - Addressed several CodeQL findings: a potential unsigned-subtraction underflow in a test assertion, an
      unnecessarily nested ``switch`` over a single-valued enum, imprecise ``assertTrue(a == b)``-style
      Python test assertions, a duplicate ``import``/``from ... import`` of the same module, an unused
      local variable, and dead code
- Continuous integration:
    - Fixed Renovate attempting to bump the ``sphinx`` pin used for Python <3.11, which is unsupported by
      Sphinx 8.2+
    - Excluded vendored third-party dependencies (fetched via CMake ``FetchContent``) and the purely
      stylistic ``cpp/poorly-documented-function`` query from CodeQL analysis, removing hundreds of
      alerts this project cannot act on
- Documentation:
    - Fixed several stale ``**Header:**`` references pointing at renamed header files
    - Fixed an inconsistent ``fiction::`` namespace omission in a ``doxygenfunction`` directive
    - Fixed a duplicated ``doxygenstruct`` directive for ``displacement_robustness_domain``
    - Fixed broken ``:ref:`` links in the publications list
    - Added missing Python binding exports (``charge_distribution_history``,
      ``sidb_charge_states_for_base_number``) that were referenced by the documentation but not importable
- Python bindings:
    - Added missing docstrings for numerous ``pyfiction`` constructors (e.g., ``cartesian_gate_layout``) that
      were previously undocumented despite an equivalent, documented constructor overload existing
    - Added missing docstrings for several dunder/operator methods (e.g., ``bdl_input_iterator_params``,
      ``__repr__``, ``__hash__``, ``__getitem__``) across the layout, network, and SiDB simulation bindings

v0.6.12 - 2025-10-29
--------------------

Added
#####
- Algorithms:
    - Random fanout substitution strategy
    - Maximum-effort mode in ``gold`` utilizing random fanout substitution strategies and random topological orderings to design high-quality layouts
    - Flag in ``gold`` to enforce NOT gates to be routed non-bending only
    - Parameter in ``gold`` to skip tiles when placing PIs, leading to higher success probabilities in discovering layouts
    - Flag in ``gold`` to randomize the number of skipped tiles when placing PIs
    - Improved performance of ``hexagonalization``
    - Improved performance of SiDB simulation algorithms through changes in the underlying data structures
    - Reimplementation of Graphviz's "Mincross" algorithm
    - Breadth-first topological view
    - ``PRUNING_BASED`` option for SiDB gate design to speed up the design process by pruning non-operational layouts
- Experiments:
    - Figures-of-Merit (FoM) SiDB layout analysis experiment script
    - QuickCell experiment script
    - Minimal SiDB gate design experiment script
- Continuous integration:
    - Python 3.14 support
- Documentation:
    - References to newly published papers

Changed
#######
- CLI:
    - ``area`` now outputs the layout width and height in addition to the area
- Continuous integration:
    - Switched to the `Ninja` generator for Windows CI builds
- Documentation:
    - Modernized README and Getting Started section of the documentation
- Dependencies:
    - Updated all dependencies to their latest versions

Fixed
#####
- Data structures:
    - Fixed I/O cell handling in ``cell_level_layout``'s ``assign_cell_type`` member function
    - Fixed system energy calculation in ``charge_distribution_surface`` in corner cases
    - Added missing ``clone`` member functions to *pyfiction*'s SiDB lattices
- Algorithms:
    - Fixed a corner case in ``hexagonalization`` when extending POs to the bottom border
    - Fixed a bug in ``post_layout_optimization`` and ``wiring_reduction`` that lead to POs not being placed at the borders and possibly dying
    - Fixed ``band_bending_resilience`` calculation of SiDB layouts for unspecified transition types
- Experiments:
    - Experiment scripts requiring ``ALGLIB`` now prompt an error message instead of failing to compile when the library is not found
- Continuous integration:
    - ``clang-tidy`` CI workflow no longer fails when run from forks
- Code quality:
    - Addressed several ``clang-tidy`` warnings throughout the code base

Removed
#######
- Continuous integration:
    - macOS 13 has been removed, along with support for the x86_64 (Intel) architecture
    - Dropped Windows 2019 and v142 support
    - Dropped Python 3.9 support due to its end-of-life status


v0.6.11 - 2025-04-23
--------------------

Added
#####
- Algorithms:
    - Parameters to move inputs to top border or outputs to the bottom border after ``hexagonalization``
- Experiments:
    - Added experiment for a comprehensive analysis of the critical temperature domain
    - Added total simulation runtime to physical simulation experiment
- Python bindings:
    - Pythonic dictionary-like interface for ``operational_domain`` and ``critical_temperature_domain``

Changed
#######
- Documentation:
    - Modernized the documentation builds by migrating the RTD build from ``pip`` to ``uv``
- Algorithms:
    - Refactored ``QuickSim`` to remove magic number for upper limit calculation
    - Refactored random layout design functions

Fixed
#####
- Fixed a bug in the z-dimension when determining the aspect ratio for cell-level layouts
- Fixed an issue in the TTS calculation caused by the automatic base detection being activated
- Fixed links in the README due to the migration of the ``mqt-core`` repository to a new namespace
- Exclude ``ClusterComplete`` from simulation engine selection when ALGBLIB is disabled
- Fixed inconsistencies and various bugs in SiDB simulation/analysis
- Fixed LT, LE, GT, and LE gate handling in the gate-level-layout and the corresponding read and write functions
- Fixed a bug in the crossing count calculation of gate-level layouts
- Increased floating point precision to avoid undetected degeneracy in the physical simulation of SiDB layouts


v0.6.10 - 2025-02-21
--------------------

Added
#####
- Algorithms:
    - Added parameters to extend the PIs to the first layout row or POs to the last layout row after ``hexagonalization``

Fixed
#####
- Fixed the Python bindings for the critical temperature domain simulation

v0.6.9 - 2025-02-12
-------------------

Added
#####
- Algorithms:
    - Added ``ClusterComplete``, a novel exact physical simulator for SiDB layouts with substantially improved runtimes, allowing the simulation of multi-gate SiDB layouts
    - Added temperature-aware operational domain simulation and restructured the function to simplify the integration of additional figures of merit in the future

Changed
#######
- Build and documentation:
    - Added optional usage of `jemalloc <https://github.com/jemalloc/jemalloc>`_ to speed up certain parallelized applications

Fixed
#####
- Algorithms:
    - Fixed a bug in ``apply_gate_library`` that resulted in incorrect cell-level layout sizing
- Build and documentation:
    - Added a CMake file for detecting pre-installed jemalloc
- Experiments:
    - Fixed a non-critical bug that occurred when ALGLIB was disabled


v0.6.8 - 2025-01-25
-------------------

Added
#####
- Data structures:
    - ``virtual_pi_network`` that allows for the duplication of input signals
    - ``mutable_rank_view`` that allows for the reordering of nodes
    - ``static_depth_view`` that disables depth recomputation
- Algorithms:
    - Logic synthesis:
        - Equivalence checking for networks with virtual PIs
- Clocking schemes:
    - SRS
- Continuous integration:
    - Added Linux on ARM runners

Fixed
#####
- Continuous deployment:
    - Fixed a critical issue with wheel building and deployment to PyPI that caused some operating systems to not be served
- Experiments:
    - Fixed outdated paths in the ``QuickCell`` experiment script


v0.6.7 - 2025-01-20
-------------------

Added
#####
- Algorithms:
    - Simulation:
        - Implemented new algorithms utilizing grid search, random sampling, and contour tracing to determine defect clearance distances of SiDB layouts
        - Restructured defect influence simulation for enhanced usability
        - Integrated ``QuickCell's`` pruning strategies to efficiently identify non-operational layouts
- Experiments:
    - Added rectangular SiDB gate skeletons (16.896 nm x 16.896 nm) and a script for designing a corresponding library
    - Enhanced results tables in the ``QuickCell`` experiment script with information about how many layouts remain and are pruned at each pruning step
- Continuous integration:
    - Added MacOS 15 and Windows 2025 to the CI
    - Added `Renovate <https://github.com/renovatebot/renovate>`_ to keep dependencies up-to-date
    - Added `ruff <https://docs.astral.sh/ruff/>`_ as a Python linter and `mypy <https://mypy-lang.org/>`_ for static Python type checking. Extended the pre-commit configuration with ``check-jsonschema``, ``validate-pyproject-schema-store``, ``nb-clean``, ``prettier``, ``ruff``, and ``mypy``
- CLI:
    - Added an optional callback to `ABC <https://github.com/berkeley-abc/abc>`_ in fiction's CLI

Changed
#######
- Continuous integration:
    - Updated the workflow to reduce runtime when building the wheels by excluding redundant combinations of OSs and Python versions

Fixed
#####
- Fixed a bug in the ``show`` command of the CLI

Removed
#######
- Continuous integration:
    - Dropped Python 3.8 support due to its end-of-life status
    - Dropped Dependabot


v0.6.6 - 2024-11-26
-------------------

Added
#####
- Algorithms:
    - Simulation:
        - Added option to determine if kinks induce layout to become non-operational
        - Kink control option for critical temperature simulation of SiDB layouts
- Python bindings:
    - Support for Python 3.13 (including GIL-free multi-threading)
- I/O:
    - SVG drawer for SiDB layouts
- Experiments:
    - Ship the SiQAD and Bestagon gate libraries as SQD files
- Documentation:
    - Added wiring reduction paper to publication list
    - Added Willem Lambooy to the authors list
- Continuous integration:
    - Several improvements to the Docker workflow including publishing images to DockerHub

Changed
#######
- Continuous integration:
    - Exclude long-running tests from the Debug CI workflows

Fixed
#####
- Fixed a bug in SiDB gate design when using skeletons with I/O wires facing east
- Adapted ``post-layout optimization`` and ``wiring reduction`` to handle layouts with PIs not placed at the borders
- Fix neutral defect handling in CDS and correct gate design termination condition
- Enforce runtime evaluation of dynamic formatting strings to fix consteval contexts
- Microsoft logo in CI badge by replacing the logo slug with a base64 encoding of the SVG image
- Remove explicit XCode version setup for macOS 13 CIs
- Adjusted PyPI deployment target for macOS
- Several bugs resulting from the new cell_type::LOGIC in the SiDB technology
- Several compiler and linter warnings
- Documentation for BDL wire detection

v0.6.5 - 2024-10-22
-------------------

Added
#####
- Experiments:
    - Script to simulate the critical temperature of SiQAD and Bestagon gates
- Algorithms:
    - Physical Design:
        - QuickCell algorithm for automatic standard cell design in silicon dangling bond logic
        - Added an option to GOLD to specify discretionary cost objectives
        - Added a flag to GOLD to enable multi-threading
        - Added a timeout option to post-layout optimization
    - Simulation:
        - Added support for different ways of implementing input information in SiDB technology to the BDL input iterator
        - Extended BDL input iterator to support different SiDB input representations
- Documentation:
    - Added a ``CITATION.cff`` file
    - Added documentation on our latest papers from IEEE-NANO

Fixed
#####
- Addressed some ``clang-tidy`` warnings


v0.6.4 - 2024-08-30
-------------------

Added
#####
- Algorithms:
    - Path-finding:
        - Squared Euclidean distance function
        - Chebyshev distance function
- Data structures:
    - ``gate_level_layout`` now tracks its number of crossings
- CLI:
    - ``ps -g`` and ``store -g`` now display the current ``gate_level_layout``'s number of crossings

Fixed
#####
- Fixed disappearing clocking schemes when applying a gate library to a gate-level layout
- Fixed a few oversights in the RTD documentation of SiDB simulation functionality
- Fixed several typos and docstrings in the codebase
- Addressed some ``clang-tidy`` warnings


v0.6.3 - 2024-08-22
-------------------

Added
#####
- Algorithms:
    - Physical Design:
        - Graph-Oriented Layout Design (GOLD) for 2DDWave-clocked Cartesian gate-level layouts to trade-off runtime vs. result quality (based on `this paper <https://www.cda.cit.tum.de/files/eda/2024_ieee_nano_a_star_is_born.pdf>`_)
        - Flag for planar post-layout optimization
        - Flag for optimizing POs only in post-layout optimization
    - Simulation:
        - Defect-aware on-the-fly SiDB circuit design on defective H-Si surfaces (based on `this paper <https://www.cda.cit.tum.de/files/eda/2024_ieee_nano_on_the_fly_gate_design.pdf>`_)
        - Displacement robustness domain simulation for SiDB layouts
        - Finding valid physical parameters for a given SiDB layout charge distribution
        - Multi-dimensional operational domain computation for SiDB layouts

Changed
#######
- Switched from execution policy-based multithreading to manual thread management in operational domain computation for platform-independence and better performance in the Python bindings
- Extended time-to-solution (TTS) calculation functions
- Add a warning when leak sanitizers are used with AppleClang since they are not supported
- Switched to new compiler and OS versions in the GitHub Actions workflows
- Updated all libraries to the latest versions

Fixed
#####
- Utilizing tolerance to mitigate floating-point inaccuracies in operational domain computation
- Some bugs in post-layout optimization
- Corner case in ``ortho`` regarding multi-output nodes
- Enable relocation of all 2-input gates during post-layout optimization


v0.6.2 - 2024-05-22
-------------------

Added
#####
- Experiments:
    - Script for runtime evaluation of ExGS, QuickExact, and QuickSim on the Bestagon gate set

Fixed
#####
- Python bindings:
    - ``detect_bdl_pairs`` no longer require the ``_100`` or ``_111`` suffix
    - Minor inconsistencies


v0.6.1 - 2024-05-16
-------------------

Added
#####
- Utils:
    - Truth table helpers for the creation of 3-input functions (NPN class representatives)
- Documentation:
    - Information on hexagonalization and post-layout optimization in README and ReadTheDocs

Changed
#######
- Continuous integration:
    - Limit the number of CI runs for PyPI deployment

Fixed
#####
- Continuous deployment:
    - Fixed the PyPI deployment workflow to publish all wheels properly


v0.6.0 - 2024-05-05
-------------------
*When it comes to the past, everyone writes fiction.* --- Stephen King

Added
#####
- Python bindings:
    - Python bindings for most of the core functionality of *fiction* using `pybind11 <https://github.com/pybind/pybind11>`_
    - Hosted on `PyPI <https://pypi.org/project/mnt.pyfiction/>`_
- Technology:
    - H-Si lattice orientation support
        - H-Si(100)-2x1
        - H-Si(111)-1x1
- Algorithms:
    - Post-layout optimization and wiring reduction for 2DDWave-clocked Cartesian gate-level layouts (based on `this paper <https://www.cda.cit.tum.de/files/eda/2023_nanoarch_post-layout_optimization_for_fcn.pdf>`_)
    - SAT-based clock number assignment
    - Added an upper bound option for the total layout area to ``exact``
    - Automatic and exhaustive SiDB gate designer (based on `this paper <https://www.cda.cit.tum.de/files/eda/2023_nanoarch_minimal_gate_design.pdf>`_)
    - Operational domain computation for SiDB layouts (based on `this paper <https://www.cda.cit.tum.de/files/eda/2023_nanoarch_reducing_the_complexity_of_operational_domain_computation_in_silicon_dangling_bond_logic.pdf>`_)
    - Novel SiDB simulator ``quickexact`` for exhaustive but fast SiDB layout simulation including atomic defects (based on `this paper <https://www.cda.cit.tum.de/files/eda/2024_aspdac_efficient_exact_simulation.pdf>`_)
    - Random SiDB layout generator
    - 2DDWave distance function
    - Hexagonalization algorithm for transforming Cartesian 2DDWave-clocked layouts into ROW-clocked hexagonal layouts (based on `this paper <https://www.cda.cit.tum.de/files/eda/2023_ieeenano_45_degree_sidb_design.pdf>`_)
    - Temperature-aware SiDB simulation (based on `this paper <https://www.cda.cit.tum.de/files/eda/2023_ieeenano_temperature_behavior.pdf>`_)
    - Atomic defect-aware physical design for SiDB layouts. Many thanks to Jeremiah Croshaw and Samuel Sze Hang Ng for the collaboration on `the paper <https://arxiv.org/abs/2311.12042>`_!
- Data types:
    - Distance maps for faster path-finding via caching or pre-computation
    - Enable ``coord_iterator`` for ``siqad::coord_t``
- I/O:
    - Unified ``print_layout`` function for all layout types
    - Support ``charge_distribution_surface`` in ``print_layout``
    - Support atomic defects in ``print_layout``
    - Support atomic defects in reading and writing SQD files
    - Proprietary file format writer for SiDB layouts together with simulation results
    - SiDB simulation file writer for `SiQAD <https://github.com/siqad/siqad>`_
- Clocking schemes:
    - Ripple
- CLI:
    - Commands ``miginvopt`` and ``miginvprop`` for MIG network optimization and inverter propagation from ``mockturtle``
- Utils:
    - Function to round a number to ``n`` decimal places
- Libraries:
    - Updated all libraries to the latest versions
- Continuous integration:
    - Added a workflow to build and test the Python bindings
    - Added a workflow to publish the Python bindings to `PyPI <https://pypi.org/project/mnt.pyfiction/>`_
    - Added a workflow to extract the docstrings from C++ to make them available in Python
    - Added a `CodeCov <https://about.codecov.io/>`_ configuration file
    - Setup `pre-commit <https://pre-commit.com/>`_ checks for code formatting and linting
- Build and documentation:
    - Added documentation on the Python bindings
    - Overhauled the README
    - Flags to partially compile select features of the CLI
    - Added latest paper references to the documentation
    - Added the new Munich Nanotech Toolkit logo
    - Added missing thanks to Giuliana Beretta
    - Added contribution and support info
- Benchmarks:
    - Combinational networks from the `IWLS93 suite <https://ddd.fit.cvut.cz/www/prj/Benchmarks/IWLS93.pdf>`_
    - Code benchmarking via `Catch2 <https://github.com/catchorg/Catch2>`_

Changed
#######
- Usability:
    - Added return types to the ``area``, ``critical_path_length_and_throughput``, and ``equivalence_checking`` functions instead of relying on the passed statistics objects
    - Refactored the technology mapping interface
    - Enabled ``offset::ucoord_t`` and ``cube::coord_t`` as coordinate types for SiDB simulations
    - Enhanced path-finding versatility by enabling them on all layout abstractions
    - ``random_coordinate`` function for all layout types
    - Added the EPFL and ISCAS85 benchmarks to the benchmark selector in the experiments
    - Changed the unit of the ``lambda_tf`` physical parameter from meter to nanometer
- Continuous integration:
    - Increased parallelism for building and testing in the Ubuntu and Windows workflows
    - Use ``mold`` instead of ``ld`` for faster link times
    - Switched to the newest OS versions in the GitHub Actions workflows
- Build and documentation:
    - Overhauled and modernized the CMake build system
    - Updated the Doxygen documentation system
- Linting:
    - Make ClangFormat aware of different line ending types and enforce ``LF``
- Miscellaneous:
    - Updated the linguist attributes
    - ``fiction`` moved to the ``cda-tum`` GitHub organization

Fixed
#####
- Minor oversights in using ``static constexpr`` and ``noexcept``
- Fixed conversion of cube coordinate with negative y-value to SiQAD coordinate
- Fixed an inconsistency in SiDB layout printing
- Fixed hop energy calculation from neutral to positive SiDB
- ``read_sqd_layout`` now updates the aspect ratio properly for SiQAD-coordinate based layouts
- Atomic defects can now be updated and new ones can be assigned to specific coordinates
- Case style of experiments folders corrected in ``fiction_experiments.hpp``
- Fixed CodeQL warnings
- Fixed a bug that caused pre-mature termination of ``sidb_surface_analysis``
- Fixed design-rule violation testing and equivalence checking on empty gate-level layouts
- Fixed compiler warnings
- Fixed a documentation bug in the physical constants section
- Fixed the bug that some physical parameters were not correctly passed to the simulators
- Fixed ``equivalence_checking`` on ``obstruction_layout`` objects
- Fixed fragments from the move to ``cda-tum`` and adjusted the tracking of publications
- Missing physical validity check in ``quicksim`` for special cases
- Bug fixes and improvements related to the coordinate system
- Fixed wrong SiDB locations in a Bestagon tile's input wire
- Fixed an issue with ``charge_distribution_surface`` not being recognized as a ``cell_level_layout``
- Fixed port routing determination for unconnected gates in the Bestagon library


v0.5.0 - 2023-03-30
-------------------
*Fiction is a way to challenge the status quo and to push the boundaries of conventional thinking.* --- unknown

Added
#####
- Technology:
    - Support for the SiDB *Bestagon* gate library, a standard-tile library for the SiDB technology based on hexagonal grids. Many thanks to Samuel Sze Hang Ng for the collaboration on `the paper <https://dl.acm.org/doi/abs/10.1145/3489517.3530525>`_!
    - Support for charge states of SiDBs
- Algorithms:
    - Electrostatic ground state simulation for SiDB cell-level layouts
        - Exhaustive simulation
        - Heuristic simulation
        - Energy calculations
    - Four established path-finding algorithms on arbitrary layouts with arbitrary clocking schemes
        - Recursive enumeration of all possible paths
        - A* for the shortest path
        - Jump Point Search (JPS) for the shortest path (proof-of-concept)
        - `k` shortest paths via Yen's algorithm
    - Distance functions and functors for layouts
        - Manhattan distance
        - Euclidean distance
        - A* distance
    - Cost functions and functors for layouts
        - Unit cost
        - Random cost
    - Graph coloring with a selection of SAT solvers or heuristic algorithms
    - Efficient multi-path signal routing on gate-level layouts (based on `this paper <https://www.cda.cit.tum.de/files/eda/2022_nanoarch_efficient_multi-path_signal_routing_for_fcn.pdf>`_)
    - Specify a black list of tiles and gates to avoid in exact physical design
    - Generic function optimizer based on simulated annealing
- Data types:
    - Obstruction layout to represent obstacles in a layout
    - Edge intersection graphs from enumerated routing paths
    - Charge distribution surface for SiDB layouts
    - Coordinate type ``siqad::coord_t`` representing signed SiDB coordinates as represented in `SiQAD <https://github.com/siqad/siqad>`_
- I/O:
    - Molecular FCN support in the QLL writer for MagCAD and SCERPA (many thanks to Giuliana Beretta!)
    - SQD reader for the SiDB technology
- Clocking Schemes:
    - CFE
- Traits:
    - ``has_*_technology`` traits to check for specific cell technologies of layouts and libraries
- Utils:
    - Routing utils
    - STL extensions
    - Truth table utils
- Libraries:
    - `phmap <https://github.com/greg7mdp/parallel-hashmap>`_ for faster hash maps (applied in many core data structures for performance reasons)
    - `TinyXML2 <https://github.com/leethomason/tinyxml2>`_ for XML parsing
- Continuous integration:
    - `clang-tidy <https://clang.llvm.org/extra/clang-tidy/>`_ workflow for static code analysis
    - `ClangFormat <https://clang.llvm.org/docs/ClangFormat.html>`_ workflow for automatic code formatting
    - `Release Drafter <https://github.com/marketplace/actions/release-drafter>`_ workflow to keep an up-to-date changelog for the next release
    - Docker image workflow to build Docker images for the latest release
- Build and documentation:
    - Automatic linking with TBB for parallel algorithms
    - `Dependabot <https://github.com/dependabot>`_ to automatically keep the dependencies up-to-date
    - `CodeQL <https://codeql.github.com/>`_ to automatically scan the code for security vulnerabilities
    - GitHub templates for issues and pull requests

Changed
#######
- CLI:
    - Split ``exact``'s and ``onepass``' parameter ``upper_bound`` into ``upper_x`` and ``upper_y``
- Clocking schemes:
    - Renamed ESP to ESR
- Libraries:
    - Updated all libraries to the latest versions
    - Moved to the upstream version of `Catch2 v3 <https://github.com/catchorg/Catch2>`_
- Continuous integration:
    - Updated CI runners to the latest versions
    - Setup `Z3 <https://github.com/Z3Prover/z3>`_ via a designated action. Many thanks to Lukas Burgholzer for his support!
    - Enabled `Ccache <https://ccache.dev/>`_ for faster compilation in CIs
    - Activated experiments in CI builds to ensure that they are building correctly
    - Run CI only when relevant files have changed
    - Switched to single-threaded builds in CI to avoid out-of-memory issues
- Build and documentation:
    - Refactored the CMake buildsystem
    - Improved the README and the documentation

Fixed
#####
- Compilation issues when a certain header was included multiple times
- Exclusion of experiment compilation if Z3 is not found
- Wrong DOT drawer in ``write_dot_layout``
- MSVC compilation issues
- Performance issues with ``foreach_*`` functions on layout types
- Performance issues with ``std::string`` where ``std::string_view`` was sufficient
- Regex in the FQCA reader
- Issue with ``clear_tile`` that would lose track of PI and PO count
- Duplicate crossing cells in the iNML ToPoliNano library
- Several I/O issues in the CLI
- Excess template parameter in the ``restore_names`` utility function
- Errors with the CMake build system if IPO was enabled through multiple sources
- Linker errors and CMake name collisions
- Warnings detected by CodeQL

Removed
#######
- LGTM badge as the service is no longer available


v0.4.0 - 2022-01-27
-------------------
*Fiction is about stuff that's screwed up.* --- Nancy Kress

Added
#####
- Technology:
    - Support for the Silicon Dangling Bond (SiDB) technology with `SiQAD <https://github.com/siqad/siqad>`_ tool support. Many thanks to Samuel Sze Hang Ng for the collaboration!
    - Support for 3D QCA layouts with `QCA-STACK <https://github.com/wlambooy/QCA-STACK>`_ tool support. Many thanks to Willem Lambooy for the collaboration!
- Data types:
    - New coordinate type ``cube::coord_t`` representing signed cube coordinates
    - New layout type ``hexagonal_layout`` representing a grid of hexagonal tiles
    - New layout type ``shifted_cartesian_layout`` replacing the ``offset`` parameter of legacy ``fcn_layout``
    - New layout type ``synchronization_element_layout`` replacing the ``clock_latch`` member of legacy ``fcn_layout``
    - New layout types ``cartesian_layout``, ``tile_based_layout``, ``gate_level_layout``, and ``cell_level_layout`` replacing various aspects of legacy ``fcn_layout``, ``fcn_gate_layout``, and ``fcn_cell_layout`` types
    - All layout types can be layered to expand their functionality, e.g., a clocked Cartesian layout type with offset coordinates results from ``clocked_layout<cartesian_layout<offset::ucoord_t>>``
    - Support for arbitrary ``mockturtle`` logic networks as layout specifications
    - New logic network type ``technology_network`` replacing legacy ``logic_network`` type
    - New view types that can be layered on top of networks ``reverse_topo_view`` and ``out_of_place_edge_color_view`` refactoring aspects from the ``ortho`` algorithm out into their own data structures
- Traits:
    - Added a trait system that can identify the appropriateness of a data type for the usage as parameter to an algorithm at compile time
    - Many traits are provided out-of-the-box like checks for the existence of certain functions or members, e.g., ``is_clocked_layout`` or ``has_foreach_tile``
    - Some pre-defined types used within the CLI can be found in the ``types.hpp`` file
- Algorithms:
    - ``convert_network`` as an extension of ``mockturtle::cleanup_dangling`` to convert between extended logic network types
    - ``apply_gate_library`` to provide an interface that generates any cell-level layout type from any gate-level layout type via the application of any gate library type
- Clocking schemes:
    - Columnar
    - Row-based
    - ESP
- Visualization:
    - Custom ``write_dot_layout`` function that creates Graphviz DOT files from gate-level layouts together with custom DOT drawers for various layout types
    - Custom ``technology_dot_drawer`` as an extension to ``mockturtle::gate_dot_drawer`` that supports more gate types
- CLI:
    - Command ``map`` for `technology mapping <https://mockturtle.readthedocs.io/en/latest/algorithms/mapper.html>`_ of logic networks using a given set of gate functions. Many thanks to Alessandro Tempia Calvino for his support!
    - Command ``sqd`` to write SiDB layouts to `SiQAD <https://github.com/siqad/siqad>`_ files
    - Command ``qll`` to write iNML layouts to `ToPoliNano & MagCAD <https://topolinano.polito.it/>`_ files (complements the existing ``qcc`` command)
    - Command ``fqca`` to write QCA layouts to `QCA-STACK <https://github.com/wlambooy/QCA-STACK>`_ files
    - Command ``blif`` to write logic networks to BLIF files
    - Added option ``--hex`` to ``exact`` and ``ortho`` instructing the algorithms to create a hexagonal layout instead of a Cartesian one. The option expects a hexagonal orientation that has to be one of the following ``odd_row``, ``even_row``, ``odd_column``, or ``even_column``
- Utility:
    - Added utility functions for networks, layouts, placement, names, arrays, ranges, and hashing to the ``utils`` folder
- Build and documentation:
    - Option to disable the CLI to be built
    - Option to enable tests to be built
    - Option to enable experiments to be built
    - Code coverage CI via `Codecov <https://app.codecov.io/gh/marcelwa/fiction>`_
    - Online documentation via `Readthedocs <https://fiction.readthedocs.io/>`_
    - Code quality analysis via `LGTM <https://lgtm.com/projects/g/marcelwa/fiction/logs/languages/lang:cpp>`_. Many thanks to Stefan Hillmich for his support!

Changed
#######
- Architecture:
    - Reworked *fiction* into a platform that offers
        (1) a header-only template library for use in external projects,
        (2) a CLI built upon said library that provides the established functionality (plus the new additions),
        (3) a framework for experiments that allows to quickly prototype ideas and compile them as stand-alone binaries built with *fiction*
    - Reworked the CMake build system to be simpler to use, yet more capable
    - Templatized all algorithms and data structures and switched to a trait-based API system. This allows for far more flexible system and the support of any type that implements certain functionality via duck typing
- CLI:
    - Command ``read`` can now also parse BLIF and FQCA files
    - Command ``read`` can now create various types of logic networks from parsing input files. A flag determines which one to create, e.g., ``--aig``, ``--mig``, or ``--xag``
    - Command ``gates`` supports more gate types now including the 3-input gates presented in `Marakkalage et al. <https://ieeexplore.ieee.org/document/9233431>`_
    - ``exact --clock_latches/-l`` has been renamed to ``exact --sync_elems/-e``
    - A technology flag ``--topolinano`` has been added to ``exact`` instructing it to respect ToPoliNano's requirements for iNML layouts
    - The ``ToPoliNano`` clocking scheme has been renamed to ``Columnar``
- Continuous Integration:
    - Moved from Travis CI to GitHub Actions with CI builds and testing under ubuntu, macOS, and Windows
- Build & Documentation:
    - Z3 is now an optional dependency that can be found automatically by *fiction* when ``-DFICTION_Z3=ON`` is passed to ``cmake``. If it is not found, some algorithms are simply excluded from compilation
    - Trimmed README in favor of Readthedocs

Fixed
#####
- Compilation issues under Windows
- SEGFAULT when using ``ortho`` under rare circumstances

Removed
#######
- Third-party dependencies:
    - Boost
    - Z3 (now optional)
    - cppitertools
- Data types:
    - ``fcn_gate_layout`` (replaced with the ``is_gate_level_layout`` trait)
    - ``fcn_cell_layout`` (replaced with the ``is_cell_level_layout`` trait)
    - ``logic_network`` (replaced with the ``mockturtle::is_network_type`` trait)
- CLI:
    - ``ortho -b`` flag because routing border I/Os is the default behavior now

v0.3.2 - 2021-01-06
-------------------
*Sometimes fiction is more easily understood than true events.* --- Young-ha Kim

Added
#####
- Command ``onepass`` for a combined SAT-based logic synthesis and physical design using `Mugen <https://github.com/whaaswijk/mugen>`_. Thanks to Winston Haaswijk for cooperating with us on this project!
- SVG output for irregular (cell-based) clocked ``fcn_cell_layout``\ s (thanks to Sophia Kuhn!)
- ``csv_writer`` for conveniently formatting experiments' results
- ``tt_reader`` for reading truth tables from a `file format used by Alan Mishchenko <https://people.eecs.berkeley.edu/~alanmi/temp5/>`_

Changed
#######
- ``exact --asynchronous/-a`` has been renamed to ``exact --async/-a`` and ``exact --asynchronous_max/-A`` has been renamed to ``exact --async_max``
- outsourced Verilog and AIGER file handling into a distinct ``network_reader`` class so that it can be used in custom experiments

Fixed
#####
- ``Docker`` build that broke down due to updates to ``mockturtle`` and ``bill``

v0.3.1 - 2020-06-04
-------------------
*There is no doubt fiction makes a better job of the truth.* --- Doris Lessing

Added
#####
- Command ``equiv`` for logical and delay equivalence checking of ``fcn_gate_layout``\ s against a specification
- Command ``energy`` to print and log energy dissipation of current ``fcn_gate_layout`` based on a physical model for the QCA-ONE library
- Command ``area`` to print and log area usage in nm²
- Parameter ``-a`` and flag ``-A`` to enable asynchronous parallelism for ``exact``
- Flag ``--minimize_wires/-w`` for ``exact`` to compute the minimum amount of wire segments needed
- Flag ``-s`` for ``show -n`` for less detailed visualization of ``logic_network`` objects
- ``Dockerfile`` and instructions for how to create an image using `Docker <https://www.docker.com/>`_ (thanks to Mario Kneidinger!)
- CMake option to toggle animated progress bars on command line

Changed
#######
- ``exact`` has been completely reworked to utilize true incremental SMT solving without push/pop mechanics (thanks to Alan Mishchenko for the inspiration!)
- ``exact --artificial_latches/-a`` has been renamed to ``exact --clock_latches/-l``
- ``exact -m`` has been renamed to ``exact -c``
- Standard resolves for clocking scheme names to their commonly used variants, e.g., ``2DDWave`` becomes ``2DDWave4``
- Energy dissipation will no longer be logged using command ``ps -g``; use new command ``energy`` instead
- Command ``cell`` can be found in command class ``Technology`` now
- Increased font size of clock numbers in SVG files by 2pt for better readability
- Changed constructor parameter types for core data structures (network and layouts)
- Changed ``std::size_t`` to fixed-size data types in lots of places
- Use library caching for *Travis* builds to speed up build time
- Moved to the latest releases of all libraries

Fixed
#####
- Python detection in CMake under different versions
- Runtime logging in ``exact``
- Performance issues in ``ortho``
- SEGFAULTS caused by ``ortho`` on large networks when compiling with gcc
- ``ortho -b`` losing bent wire connections
- ``fcn_layout::random_face``\ 's index to coordinate mapping again, but for real now (thanks to Till Schlechtweg!)
- ``logic_network``\ s are deep-copied for each physical design call now to secure them from external changes
- Gates and wires without directions assigned are mapped to standard rotations using QCA-ONE library now
- Rotation issues with border gate-pin I/Os using QCA-ONE library
- 3-output fan-outs are correctly printed as fan-outs when using ``print -g`` now
- Testing ``ofstream``\ 's for ``is_open`` in writers now (thanks to DeepCode!)
- Several compiler issues under MacOS and Windows (thanks to Umberto Garlando and Fabrizio Riente for pointing them out!)
- Z3 build script error under Unix with CMake version <= 3.12.0
- Z3 linking on MacOS (thanks to Daniel Staack!)
- bibTeX citation information correctly handles last names and arXiv prefixes now

Removed
#######
- ``exact --limit_crossings/-c`` and ``exact --limit_wires/-w`` as they have been replaced by respective optimization flags
- Legends in ``print -g/-c``

v0.3.0 - 2019-11-22
-------------------
*Sometimes, fiction was so powerful that it even had reverberations in the real world.* --- Delphine de Vigan

Added
#####
- Support for iNML technology using `ToPoliNano <https://topolinano.polito.it/>`_\ 's gate library and clocking scheme. Thanks to Umberto Garlando for cooperating with us on this project!
- Support for vertically shifted ``fcn_layout``\ s to emulate column-based clocking schemes
- Enhanced ``logic_network`` by incorporating `mockturtle <https://github.com/lsils/mockturtle>`_ for logic representation
- Truth table store (mnemonic ``-t``) and command ``tt``. Thanks to Mathias Soeken for granting permission to use code from `CirKit <https://github.com/msoeken/cirkit>`_!
- Command ``simulate`` to compute ``truth_table``\ s for ``logic_network`` and ``fcn_gate_layout`` objects. Thanks to Mathias Soeken for granting permission to use code from `CirKit <https://github.com/msoeken/cirkit>`_!
- Command ``akers`` to perform Akers' Majority synthesis to generate a ``logic_network`` from a ``truth_table``
- Command ``random`` to generate random ``logic_network`` objects
- Command ``check`` to verify structural integrity of designed ``fcn_gate_layout`` objects
- Command ``gates`` to list gate counts for each vertex type in the current ``logic_network``
- Command ``fanouts`` to substitute high-degree inputs into fan-out vertices in ``logic_network``\ s using a given strategy
- Command ``balance`` to subdivide ``logic_network`` edges to equalize path lengths by inserting auxiliary wire vertices
- Command ``qcc`` to write ``iNML`` ``cell_layout``\ s to component files readable by `ToPoliNano and MagCAD <https://topolinano.polito.it/>`_
- Capability to enforce straight inverter gates in ``exact`` with flag ``-n``
- Capability to minimize the number of used crossing tiles in ``exact`` with flag ``-m``
- Capability to parse AIGER (``*.aig``) files using ``read``
- Parameter ``-b`` for ``ortho``
- Progress bars for ``exact`` and ``ortho``
- ``show -n`` to display ``logic_network`` objects
- Several convenience functions in the core data structures for easier access
- An overview `paper <https://github.com/marcelwa/fiction/blob/main/bib/paper.pdf>`_ and a `poster <https://github.com/marcelwa/fiction/blob/main/bib/poster.pdf>`_ about the features of *fiction*. Please find citation information in the :ref:`publication list <publications>`

Changed
#######
- Moved to C++17
- Moved to version 1.0 of `cppitertools <https://github.com/ryanhaining/cppitertools>`_
- Included latest updates for `alice <https://github.com/msoeken/alice>`_
- Switched ``logic_network``\ 's CLI mnemonic to ``-n`` as it is no longer reserved by ``alice``
- Renamed ``pi``\ /``po_count`` to ``num_pis``\ /``pos``
- ``read`` does no longer substitute fan-outs automatically, ``exact`` and ``ortho`` do it instead if the user did not call ``fanouts``
- ``exact --path_discrepancy/-p`` has been renamed to ``exact --desynchronize/-d`` to express its use case better
- ``exact --timeout/-t`` expects its parameter in seconds instead of milliseconds now
- ``exact --fixed_size/-f`` expects its own parameter instead of using ``--upper_bound``\ 's one
- Renamed ``version.h`` to ``version_info.h``
- Renamed *Placement & Routing* to *Physical Design* where appropriate to match the documentation

Fixed
#####
- Segfault when using ``ortho -i`` with certain compilers in release mode
- Missing input ports for 3-output fan-out gates in QCA-ONE library
- Wire vertices not handled properly by QCA-ONE library
- Wrong clocking look-up for ``BANCS`` clocking in ``fcn_cell_layout``
- Tile directions when assigning and dissociating multiple edges
- ``fcn_layout::random_face``\ 's index to coordinate mapping (thanks to Till Schlechtweg!)
- Format issues with benchmark files
- Constant outputs of some benchmark files
- Additionally, there are several performance improvements in core data structures and algorithms

Removed
#######
- Submodule ``lorina`` as it is included in ``mockturtle``
- ``verilog_parser.h`` as ``mockturtle`` comes with its own one
- ``print -n`` as it is replaced by ``show -n``
- ``operation::BUF``; use ``operation::W`` instead
- ``operation::CONST0``, ``operation::CONST1``, and ``operation::XOR``
- ``operator[x][y][z]`` for ``fcn_layout``\ s as it was slow and therefore not used; use ``face/tile/cell{x,y,z}`` instead

v0.2.1 - 2019-05-02
-------------------
*Fiction is art and art is the triumph over chaos.* --- John Cheever

Added
#####
- Support for BANCS clocking and integration in ``exact``
- Name strings for ``fcn_clocking_scheme`` objects and corresponding name-based look-up
- Version and build information accessible within the code by including ``util/version.h``
- Parameter ``-i`` for command ``ortho``
- ``shortcuts.fs`` with predefined flows
- ``benchmarks/MAJ/`` folder with some TOY benchmarks using MAJ gates

Changed
#######
- Calls to ``exact -s`` now need to name the desired clocking, e.g. ``exact -s use`` (case insensitive)
- ``incoming``\ /``outgoing_information_flow_tiles`` have been renamed to ``incoming``\ /``outgoing_data_flow`` and handle multi wires now
- Renamed diagonal clocking schemes to 2DDWAVE and gave proper credit
- More verbose error messages

Fixed
#####
- TP calculation for layouts without designated I/O pins (thanks to Mario Kneidinger!)
- I/O port orientation of PI/PO gates using QCA-ONE library
- Usage of non-PI/PO MAJ gates in QCA-ONE library
- Visualization of clock latches in ``show -c`` (thanks to Sophia Kuhn!)
- Multi direction assignment to wires and gates in ``exact`` leading to physically impossible layouts
- ``shrink_to_fit`` in ``fcn_gate_layout`` incorporates the BGL bug now. Minimum size in each dimension is 2. For more information, see https://svn.boost.org/trac10/ticket/11735
- Parameters for ``exact`` no longer get stuck once set

Removed
#######
- Parameter ``-n`` for ``exact``

v0.2.0 - 2019-03-21
-------------------
*Fiction reveals the truth that reality obscures.* --- Jessamyn West

Added
#####
- Export ``fcn_cell_layout`` objects as SVG using ``show -c``. See README for more information
- ``ps -g`` displays and logs critical path and throughput of ``fcn_gate_layout`` objects
- Support for RES clocking and integration in ``exact``
- New TOY benchmarks in respective folder
- New command ``clear`` to remove all elements from stores (as a shorthand for ``store --clear ...``)
- Information about how to build fiction for WSL
- Functions to distinguish different ``logic_network`` types like AIGs/MIGs/...
- Parameter ``-n`` for command ``ortho``

Changed
#######
- Revised folder structure due to the increasing amount of source files
- ``read_verilog`` is now called ``read`` and can process directories
- ``gate_to_cell`` is now called ``cell``
- ``write_qca`` is now called ``qca`` and handles file names automatically if necessary
- ``-u`` is not a required parameter for ``exact`` anymore
- Richer output for ``print -w``
- Included latest bugfixes for `alice <https://github.com/msoeken/alice>`_
- Included latest update for `lorina <https://github.com/hriener/lorina>`_

Fixed
#####
- Starting layout size for calls to ``exact -i`` was too low and has been corrected
- Several code and comment inconsistencies

Removed
#######
- ITC99 benchmark files


v0.1.1 - 2018-12-29
-------------------
*Literature is a luxury; fiction is a necessity.* --- G. K. Chesterton

Added
#####
- Technology-specific energy model for ``fcn_gate_layout``; supports QCA thus far
- Support for ``print -c`` to write a textual representation of ``fcn_cell_layout`` objects
- Information on nested fiction scripts and documentation generation in README
- *linguist* flags in ``.gitattributes`` to prevent benchmark files from being viewed as source code

Changed
#######
- Moved to version 0.4 of `alice <https://github.com/msoeken/alice>`_
- Moved to version 4.8.4 of `Z3 <https://github.com/Z3Prover/z3>`_
- ``fcn_gate_library`` objects now have name strings
- ``print -g`` now displays incorrectly assigned directions by bidirectional arrows
- "Release" is the standard build mode now

Fixed
#####
- Copy and move constructors of ``logic_network`` work properly now
- Calculation of ``bounding_box`` size on ``fcn_gate_layout`` now handles empty layouts correctly
- Several minor and rare bugs, code inconsistencies, and performance issues

Removed
#######
- Nothing

v0.1.0 - 2018-10-29
-------------------
*Let there be a fiction*

This is the initial release. Please find a feature overview in the README.
