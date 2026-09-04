(publications)=

# Publications

_fiction_ provides design automation for field-coupled nanocomputing (FCN).
If you use _fiction_, please cite the framework report and the Munich Nanotech Toolkit paper
{cite:p}`fiction,mnt`.

{download}`Download the bibliography as BibTeX <references.bib>`.

## Algorithms and Gate Libraries

If you use an algorithm or gate library, please also cite its corresponding papers.

- {ref}`Exact physical design <exact>` (`exact`):
  {cite:p}`walter2018exact`

- {ref}`Scalable physical design <ortho>` (`ortho`):
  {cite:p}`walter2019ortho`

- {ref}`Equivalence checking <equiv>` (`equiv`):
  {cite:p}`walter2020equiv`

- {ref}`Multi-path routing <color_routing>` (`color_routing`):
  {cite:p}`walter2022colorrouting`

  [Multi-path routing experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/color_routing)

- {ref}`QuickSim <quicksim>` (`quicksim`):
  {cite:p}`drewniok2023quicksim`

  [QuickSim experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/electrostatic_ground_state)

- {ref}`QuickExact <quickexact>` (`quickexact`):
  {cite:p}`drewniok2024quickexact`

  [QuickExact experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/electrostatic_ground_state)

- {ref}`ClusterComplete <clustercomplete>` (`clustercomplete`):
  {cite:p}`lambooy2026clustercomplete`

- {ref}`Critical temperature simulation <critical_temperature>` (`critical_temperature`):
  {cite:p}`drewniok2023temperature`

  [Critical temperature experiment scripts](https://github.com/cda-tum/fiction/tree/main/experiments/sidb_simulation/temperature)

- {ref}`Operational domain computation <opdom>` (`opdom`):
  {cite:p}`walter2023opdom,walter2025domainexplorer`

  - [Operational domain experiment scripts](https://github.com/cda-tum/fiction/tree/main/experiments/operational_domain)
  - [Operational Domain Explorer](https://github.com/cda-tum/mnt-opdom-explorer)

- {ref}`SiDB gate design <gate_design>` (`gate_design`):
  {cite:p}`drewniok2023gatedesigner,drewniok2025efficientgatedesign,drewniok2025quickcell`

  [Gate design experiment scripts](https://github.com/cda-tum/fiction/tree/main/experiments/standard_cell_design)

- {ref}`Mapping Cartesian to hexagonal layouts <hexagonalization>` (`hexagonalization`):
  {cite:p}`hofmann2023hexagonalization`

  [Hexagonalization experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/hexagonalization)

- {ref}`Post-layout optimization <post_layout_optimization>` (`post_layout_optimization`):
  {cite:p}`hofmann2023postlayout`

  [Post-layout optimization experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/post_layout_optimization)
  {cite:p}`hofmann2025scalablepostlayout`

  [Scalable post-layout optimization experiment script](https://github.com/cda-tum/fiction/blob/main/experiments/post_layout_optimization/scalable_post_layout_optimization.cpp)

- {ref}`Wiring reduction <wiring_reduction>` (`wiring_reduction`):
  {cite:p}`hofmann2024wiring`

  [Wiring reduction experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/wiring_reduction)

- {ref}`Graph-oriented layout design <graph_oriented_layout_design>` (`gold`):
  {cite:p}`hofmann2024gold`

  [Graph-oriented layout design experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/graph_oriented_layout_design)
  {cite:p}`hofmann2025goldcost`

  [Graph-oriented layout design with discretionary cost objectives experiment script](https://github.com/cda-tum/fiction/blob/main/experiments/graph_oriented_layout_design/gold_cost_objectives.cpp)

- {ref}`SAT-based clock number assignment <determine_clocking>`:
  {cite:p}`walter2024clocknumber`

  [Clock number assignment experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/clock_number_assignment)

- Towards Defect-Aware Physical Design of Silicon Dangling Bond Logic:
  {cite:p}`walter2024defectaware`

  [Towards defect-aware physical design of silicon dangling bond logic experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/defect_aware_physical_design)

- {ref}`QuickTrace: An Efficient Contour Tracing Algorithm for Defect Robustness Simulation of Silicon Dangling Bond Logic <defect_influence>`
  {cite:p}`drewniok2025quicktrace`

  [QuickTrace experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/quicktrace)

- {ref}`On-the-fly Defect-Aware Design of Circuits <on_the_fly_design>`:
  {cite:p}`drewniok2024ontheflydesign`

  [On-the-fly defect-aware design of circuits experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/physical_design_with_on_the_fly_gate_design)

- {ref}`Unifying Figures of Merit for Silicon Dangling Bond Logic <band_bending_resilience>`:
  {cite:p}`drewniok2024figuresofmerit`

- {ref}`Alternative Silicon Orientations <alternative_hsi_lattices>`:
  {cite:p}`ng2024latorientations`

- {ref}`SiDB Bestagon library <bestagon>`:
  {cite:p}`walter2022hexagons`

  [Bestagon experiment script](https://github.com/cda-tum/fiction/tree/main/experiments/bestagon)

- {ref}`Molecular QCA library <molQCA>`:
  {cite:p}`hien2025molqca`

## Related Publications

The bibliography also includes further papers published by the Chair for Design Automation at TUM.
These cover related tools, technology studies, and design methods; inclusion does not imply
that a method is implemented in _fiction_.

```{bibliography}
:all:
```
