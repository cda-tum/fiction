# Upgrading

This file records breaking changes between releases of _fiction_ and how to adapt to
them.

## Unreleased — the `include/fiction` restructure

`include/fiction/` has been reorganized so that the directory a header lives in tells
you what the header is about, and nested namespaces now mirror that tree. See
[#1096](https://github.com/cda-tum/fiction/issues/1096) and
[#1097](https://github.com/cda-tum/fiction/issues/1097).

_fiction_ is header-only, so an include path is public API. **This is a breaking change**
and it is deliberately a clean break: no forwarding headers are left at the old paths,
and no `[[deprecated]]` `using` declarations are left at the old namespace scopes. A
forwarder cannot help a translation unit whose include path has already stopped
resolving, so shipping one would add a release of clutter for no migration benefit.

Every change is listed below.

### The shape of the new tree

```text
include/fiction/
├── traits.hpp  types.hpp          fiction::
├── layouts/                       fiction::layouts::
│   ├── io/                        fiction::layouts::io::
│   └── utils/                     fiction::layouts::utils::
├── networks/                      fiction::networks::
│   ├── views/                     fiction::networks::views::
│   ├── io/                        fiction::networks::io::
│   └── utils/                     fiction::networks::utils::
├── synthesis/                     fiction::synthesis::
├── physical_design/               fiction::physical_design::
│   ├── path_finding/              fiction::physical_design::path_finding::
│   └── utils/                     fiction::physical_design::utils::
├── verification/                  fiction::verification::
├── technology/                    (adds no namespace level of its own)
│   ├── fcn/                       fiction::fcn::
│   ├── qca/                       fiction::qca::
│   │   └── io/                    fiction::qca::io::
│   ├── inml/                      fiction::inml::
│   │   └── io/                    fiction::inml::io::
│   └── sidb/                      fiction::sidb::
│       ├── primitives/            fiction::sidb::primitives::
│       ├── model/                 fiction::sidb::model::
│       ├── simulation/            fiction::sidb::simulation::
│       │   ├── engines/           fiction::sidb::simulation::engines::
│       │   ├── analysis/          fiction::sidb::simulation::analysis::
│       │   ├── defects/           fiction::sidb::simulation::defects::
│       │   ├── logic/             fiction::sidb::simulation::logic::
│       │   ├── generic/           fiction::sidb::simulation::generic::
│       │   └── utils/             fiction::sidb::simulation::utils::
│       ├── libraries/             fiction::sidb::libraries::
│       ├── generators/            fiction::sidb::generators::
│       └── io/                    fiction::sidb::io::
└── utils/                         fiction::utils::
    ├── math/                      fiction::utils::math::
    ├── stl/                       fiction::utils::stl::
    └── debug/                     fiction::utils::debug::
```

Two rules explain the whole table:

1. **Namespaces mirror directories.** Each directory level below `include/fiction/`
   adds one namespace level. `technology/` is the single exception: it groups the
   technology directories on disk but adds no namespace, so `technology/sidb/model/`
   is `fiction::sidb::model`, not `fiction::technology::sidb::model`.
2. **Redundant identifier prefixes are dropped.** Once the namespace carries the
   domain, `sidb_simulation_parameters` becomes `sidb::model::simulation_parameters`.
   The prefix is kept where it is part of a published proper name (`qca_one_library`)
   or where the enclosing scope does not imply it.

The old flat `io/` directory is gone. File readers and writers now live with the
module whose data they serialize.

### Moved headers

125 of 148 headers changed path. Headers not listed here kept their path.

#### `layouts`

| old                               | new                                       |
| --------------------------------- | ----------------------------------------- |
| `fiction/io/print_layout.hpp`     | `fiction/layouts/io/print_layout.hpp`     |
| `fiction/io/read_fgl_layout.hpp`  | `fiction/layouts/io/read_fgl_layout.hpp`  |
| `fiction/io/write_fgl_layout.hpp` | `fiction/layouts/io/write_fgl_layout.hpp` |
| `fiction/io/write_svg_layout.hpp` | `fiction/layouts/io/write_svg_layout.hpp` |
| `fiction/utils/layout_utils.hpp`  | `fiction/layouts/utils/layout_utils.hpp`  |

#### `networks`

| old                                   | new                                            |
| ------------------------------------- | ---------------------------------------------- |
| `fiction/io/dot_drawers.hpp`          | `fiction/networks/io/dot_drawers.hpp`          |
| `fiction/io/network_reader.hpp`       | `fiction/networks/io/network_reader.hpp`       |
| `fiction/io/tt_reader.hpp`            | `fiction/networks/io/tt_reader.hpp`            |
| `fiction/utils/mockturtle_utils.hpp`  | `fiction/networks/utils/mockturtle_utils.hpp`  |
| `fiction/utils/name_utils.hpp`        | `fiction/networks/utils/name_utils.hpp`        |
| `fiction/utils/network_utils.hpp`     | `fiction/networks/utils/network_utils.hpp`     |
| `fiction/utils/truth_table_utils.hpp` | `fiction/networks/utils/truth_table_utils.hpp` |

#### `synthesis`

| old                                                                 | new                                                |
| ------------------------------------------------------------------- | -------------------------------------------------- |
| `fiction/algorithms/network_transformation/delete_virtual_pis.hpp`  | `fiction/synthesis/delete_virtual_pis.hpp`         |
| `fiction/algorithms/network_transformation/fanout_substitution.hpp` | `fiction/synthesis/fanout_substitution.hpp`        |
| `fiction/algorithms/network_transformation/network_balancing.hpp`   | `fiction/synthesis/network_balancing.hpp`          |
| `fiction/algorithms/network_transformation/network_conversion.hpp`  | `fiction/synthesis/network_conversion.hpp`         |
| `fiction/algorithms/network_transformation/technology_mapping.hpp`  | `fiction/synthesis/technology_mapping.hpp`         |
| `fiction/technology/technology_mapping_library.hpp`                 | `fiction/synthesis/technology_mapping_library.hpp` |

#### `physical_design`

| old                                                                   | new                                                                  |
| --------------------------------------------------------------------- | -------------------------------------------------------------------- |
| `fiction/algorithms/physical_design/apply_gate_library.hpp`           | `fiction/physical_design/apply_gate_library.hpp`                     |
| `fiction/algorithms/physical_design/color_routing.hpp`                | `fiction/physical_design/color_routing.hpp`                          |
| `fiction/algorithms/physical_design/determine_clocking.hpp`           | `fiction/physical_design/determine_clocking.hpp`                     |
| `fiction/algorithms/physical_design/exact.hpp`                        | `fiction/physical_design/exact.hpp`                                  |
| `fiction/algorithms/physical_design/graph_oriented_layout_design.hpp` | `fiction/physical_design/graph_oriented_layout_design.hpp`           |
| `fiction/algorithms/physical_design/hexagonalization.hpp`             | `fiction/physical_design/hexagonalization.hpp`                       |
| `fiction/algorithms/physical_design/orthogonal.hpp`                   | `fiction/physical_design/orthogonal.hpp`                             |
| `fiction/algorithms/path_finding/a_star.hpp`                          | `fiction/physical_design/path_finding/a_star.hpp`                    |
| `fiction/algorithms/path_finding/cost.hpp`                            | `fiction/physical_design/path_finding/cost.hpp`                      |
| `fiction/algorithms/path_finding/distance.hpp`                        | `fiction/physical_design/path_finding/distance.hpp`                  |
| `fiction/algorithms/path_finding/distance_map.hpp`                    | `fiction/physical_design/path_finding/distance_map.hpp`              |
| `fiction/algorithms/path_finding/enumerate_all_paths.hpp`             | `fiction/physical_design/path_finding/enumerate_all_paths.hpp`       |
| `fiction/algorithms/path_finding/k_shortest_paths.hpp`                | `fiction/physical_design/path_finding/k_shortest_paths.hpp`          |
| `fiction/algorithms/physical_design/post_layout_optimization.hpp`     | `fiction/physical_design/post_layout_optimization.hpp`               |
| `fiction/algorithms/iter/aspect_ratio_iterator.hpp`                   | `fiction/physical_design/utils/aspect_ratio_iterator.hpp`            |
| `fiction/algorithms/graph/generate_edge_intersection_graph.hpp`       | `fiction/physical_design/utils/generate_edge_intersection_graph.hpp` |
| `fiction/algorithms/graph/graph_coloring.hpp`                         | `fiction/physical_design/utils/graph_coloring.hpp`                   |
| `fiction/algorithms/graph/mincross.hpp`                               | `fiction/physical_design/utils/mincross.hpp`                         |
| `fiction/utils/placement_utils.hpp`                                   | `fiction/physical_design/utils/placement_utils.hpp`                  |
| `fiction/utils/routing_utils.hpp`                                     | `fiction/physical_design/utils/routing_utils.hpp`                    |
| `fiction/algorithms/optimization/simulated_annealing.hpp`             | `fiction/physical_design/utils/simulated_annealing.hpp`              |
| `fiction/algorithms/physical_design/wiring_reduction.hpp`             | `fiction/physical_design/wiring_reduction.hpp`                       |

#### `verification`

| old                                                                     | new                                                            |
| ----------------------------------------------------------------------- | -------------------------------------------------------------- |
| `fiction/algorithms/properties/count_gate_types.hpp`                    | `fiction/verification/count_gate_types.hpp`                    |
| `fiction/algorithms/properties/critical_path_length_and_throughput.hpp` | `fiction/verification/critical_path_length_and_throughput.hpp` |
| `fiction/algorithms/verification/design_rule_violations.hpp`            | `fiction/verification/design_rule_violations.hpp`              |
| `fiction/algorithms/verification/equivalence_checking.hpp`              | `fiction/verification/equivalence_checking.hpp`                |
| `fiction/algorithms/verification/virtual_miter.hpp`                     | `fiction/verification/virtual_miter.hpp`                       |

#### `technology`

| old                                                                               | new                                                                                        |
| --------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| `fiction/technology/area.hpp`                                                     | `fiction/technology/fcn/area.hpp`                                                          |
| `fiction/technology/cell_ports.hpp`                                               | `fiction/technology/fcn/cell_ports.hpp`                                                    |
| `fiction/technology/cell_technologies.hpp`                                        | `fiction/technology/fcn/cell_technologies.hpp`                                             |
| `fiction/technology/constants.hpp`                                                | `fiction/technology/fcn/constants.hpp`                                                     |
| `fiction/technology/fcn_gate_library.hpp`                                         | `fiction/technology/fcn/gate_library.hpp`                                                  |
| `fiction/io/write_qcc_layout.hpp`                                                 | `fiction/technology/inml/io/write_qcc_layout.hpp`                                          |
| `fiction/io/write_qll_layout.hpp`                                                 | `fiction/technology/inml/io/write_qll_layout.hpp`                                          |
| `fiction/technology/magcad_magnet_count.hpp`                                      | `fiction/technology/inml/magcad_magnet_count.hpp`                                          |
| `fiction/technology/inml_topolinano_library.hpp`                                  | `fiction/technology/inml/topolinano_library.hpp`                                           |
| `fiction/io/read_fqca_layout.hpp`                                                 | `fiction/technology/qca/io/read_fqca_layout.hpp`                                           |
| `fiction/io/write_fqca_layout.hpp`                                                | `fiction/technology/qca/io/write_fqca_layout.hpp`                                          |
| `fiction/io/write_qca_layout.hpp`                                                 | `fiction/technology/qca/io/write_qca_layout.hpp`                                           |
| `fiction/technology/qca_one_library.hpp`                                          | `fiction/technology/qca/qca_one_library.hpp`                                               |
| `fiction/technology/sim7_mol_library.hpp`                                         | `fiction/technology/qca/sim7_mol_library.hpp`                                              |
| `fiction/algorithms/physical_design/design_sidb_gates.hpp`                        | `fiction/technology/sidb/generators/design_sidb_gates.hpp`                                 |
| `fiction/technology/is_sidb_gate_design_impossible.hpp`                           | `fiction/technology/sidb/generators/is_gate_design_impossible.hpp`                         |
| `fiction/algorithms/physical_design/on_the_fly_sidb_circuit_design.hpp`           | `fiction/technology/sidb/generators/on_the_fly_circuit_design.hpp`                         |
| `fiction/algorithms/simulation/sidb/random_sidb_layout_generator.hpp`             | `fiction/technology/sidb/generators/random_layout_generator.hpp`                           |
| `fiction/io/read_sqd_layout.hpp`                                                  | `fiction/technology/sidb/io/read_sqd_layout.hpp`                                           |
| `fiction/io/read_sidb_surface_defects.hpp`                                        | `fiction/technology/sidb/io/read_surface_defects.hpp`                                      |
| `fiction/io/write_defect_influence_domain.hpp`                                    | `fiction/technology/sidb/io/write_defect_influence_domain.hpp`                             |
| `fiction/io/write_location_and_ground_state.hpp`                                  | `fiction/technology/sidb/io/write_location_and_ground_state.hpp`                           |
| `fiction/io/write_operational_domain.hpp`                                         | `fiction/technology/sidb/io/write_operational_domain.hpp`                                  |
| `fiction/io/write_sqd_layout.hpp`                                                 | `fiction/technology/sidb/io/write_sqd_layout.hpp`                                          |
| `fiction/io/write_sqd_sim_result.hpp`                                             | `fiction/technology/sidb/io/write_sqd_sim_result.hpp`                                      |
| `fiction/technology/sidb_bestagon_library.hpp`                                    | `fiction/technology/sidb/libraries/bestagon_library.hpp`                                   |
| `fiction/technology/sidb_on_the_fly_gate_library.hpp`                             | `fiction/technology/sidb/libraries/on_the_fly_gate_library.hpp`                            |
| `fiction/technology/sidb_skeleton_bestagon_library.hpp`                           | `fiction/technology/sidb/libraries/skeleton_bestagon_library.hpp`                          |
| `fiction/technology/sidb_surface_analysis.hpp`                                    | `fiction/technology/sidb/libraries/surface_analysis.hpp`                                   |
| `fiction/technology/sidb_charge_state.hpp`                                        | `fiction/technology/sidb/model/charge_state.hpp`                                           |
| `fiction/technology/sidb_cluster_hierarchy.hpp`                                   | `fiction/technology/sidb/model/cluster_hierarchy.hpp`                                      |
| `fiction/technology/sidb_defects.hpp`                                             | `fiction/technology/sidb/model/defects.hpp`                                                |
| `fiction/technology/sidb_nm_distance.hpp`                                         | `fiction/technology/sidb/model/nm_distance.hpp`                                            |
| `fiction/technology/sidb_nm_position.hpp`                                         | `fiction/technology/sidb/model/nm_position.hpp`                                            |
| `fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp`               | `fiction/technology/sidb/model/simulation_parameters.hpp`                                  |
| `fiction/technology/charge_distribution_surface.hpp`                              | `fiction/technology/sidb/primitives/charge_distribution_surface.hpp`                       |
| `fiction/technology/sidb_defect_surface.hpp`                                      | `fiction/technology/sidb/primitives/defect_surface.hpp`                                    |
| `fiction/technology/sidb_lattice.hpp`                                             | `fiction/technology/sidb/primitives/lattice.hpp`                                           |
| `fiction/technology/sidb_lattice_orientations.hpp`                                | `fiction/technology/sidb/primitives/lattice_orientations.hpp`                              |
| `fiction/algorithms/simulation/sidb/band_bending_resilience.hpp`                  | `fiction/technology/sidb/simulation/analysis/band_bending_resilience.hpp`                  |
| `fiction/algorithms/simulation/sidb/calculate_energy_and_state_type.hpp`          | `fiction/technology/sidb/simulation/analysis/calculate_energy_and_state_type.hpp`          |
| `fiction/algorithms/simulation/sidb/critical_temperature.hpp`                     | `fiction/technology/sidb/simulation/analysis/critical_temperature.hpp`                     |
| `fiction/algorithms/simulation/sidb/energy_distribution.hpp`                      | `fiction/technology/sidb/simulation/analysis/energy_distribution.hpp`                      |
| `fiction/algorithms/simulation/sidb/occupation_probability_of_excited_states.hpp` | `fiction/technology/sidb/simulation/analysis/occupation_probability_of_excited_states.hpp` |
| `fiction/algorithms/simulation/sidb/physical_population_stability.hpp`            | `fiction/technology/sidb/simulation/analysis/physical_population_stability.hpp`            |
| `fiction/algorithms/simulation/sidb/defect_clearance.hpp`                         | `fiction/technology/sidb/simulation/defects/defect_clearance.hpp`                          |
| `fiction/algorithms/simulation/sidb/defect_influence.hpp`                         | `fiction/technology/sidb/simulation/defects/defect_influence.hpp`                          |
| `fiction/algorithms/simulation/sidb/displacement_robustness_domain.hpp`           | `fiction/technology/sidb/simulation/defects/displacement_robustness_domain.hpp`            |
| `fiction/algorithms/simulation/sidb/sidb_simulation_domain.hpp`                   | `fiction/technology/sidb/simulation/domain.hpp`                                            |
| `fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp`                   | `fiction/technology/sidb/simulation/engine.hpp`                                            |
| `fiction/algorithms/simulation/sidb/clustercomplete.hpp`                          | `fiction/technology/sidb/simulation/engines/clustercomplete.hpp`                           |
| `fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp`       | `fiction/technology/sidb/simulation/engines/exhaustive_ground_state_simulation.hpp`        |
| `fiction/algorithms/simulation/sidb/ground_state_space.hpp`                       | `fiction/technology/sidb/simulation/engines/ground_state_space.hpp`                        |
| `fiction/algorithms/simulation/sidb/quickexact.hpp`                               | `fiction/technology/sidb/simulation/engines/quickexact.hpp`                                |
| `fiction/algorithms/simulation/sidb/quicksim.hpp`                                 | `fiction/technology/sidb/simulation/engines/quicksim.hpp`                                  |
| `fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp`               | `fiction/technology/sidb/simulation/generic/can_positive_charges_occur.hpp`                |
| `fiction/algorithms/simulation/sidb/is_ground_state.hpp`                          | `fiction/technology/sidb/simulation/generic/is_ground_state.hpp`                           |
| `fiction/algorithms/simulation/sidb/minimum_energy.hpp`                           | `fiction/technology/sidb/simulation/generic/minimum_energy.hpp`                            |
| `fiction/algorithms/simulation/sidb/physically_valid_parameters.hpp`              | `fiction/technology/sidb/simulation/generic/physically_valid_parameters.hpp`               |
| `fiction/algorithms/simulation/sidb/potential_to_distance_conversion.hpp`         | `fiction/technology/sidb/simulation/generic/potential_to_distance_conversion.hpp`          |
| `fiction/algorithms/iter/bdl_input_iterator.hpp`                                  | `fiction/technology/sidb/simulation/logic/bdl_input_iterator.hpp`                          |
| `fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp`                         | `fiction/technology/sidb/simulation/logic/detect_bdl_pairs.hpp`                            |
| `fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp`                         | `fiction/technology/sidb/simulation/logic/detect_bdl_wires.hpp`                            |
| `fiction/algorithms/simulation/sidb/is_operational.hpp`                           | `fiction/technology/sidb/simulation/logic/is_operational.hpp`                              |
| `fiction/algorithms/simulation/sidb/operational_domain.hpp`                       | `fiction/technology/sidb/simulation/logic/operational_domain.hpp`                          |
| `fiction/algorithms/simulation/sidb/operational_domain_ratio.hpp`                 | `fiction/technology/sidb/simulation/logic/operational_domain_ratio.hpp`                    |
| `fiction/algorithms/simulation/sidb/verify_logic_match.hpp`                       | `fiction/technology/sidb/simulation/logic/verify_logic_match.hpp`                          |
| `fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp`                   | `fiction/technology/sidb/simulation/result.hpp`                                            |
| `fiction/algorithms/simulation/sidb/equivalence_check_for_simulation_results.hpp` | `fiction/technology/sidb/simulation/utils/equivalence_check_for_simulation_results.hpp`    |
| `fiction/algorithms/simulation/sidb/time_to_solution.hpp`                         | `fiction/technology/sidb/simulation/utils/time_to_solution.hpp`                            |

#### `utils`

| old                                              | new                                        |
| ------------------------------------------------ | ------------------------------------------ |
| `fiction/utils/combination_utils.hpp`            | `fiction/utils/math/combination_utils.hpp` |
| `fiction/utils/math_utils.hpp`                   | `fiction/utils/math/math_utils.hpp`        |
| `fiction/utils/array_utils.hpp`                  | `fiction/utils/stl/array_utils.hpp`        |
| `fiction/io/csv_writer.hpp`                      | `fiction/utils/stl/csv_writer.hpp`         |
| `fiction/utils/execution_utils.hpp`              | `fiction/utils/stl/execution_utils.hpp`    |
| `fiction/algorithms/iter/gray_code_iterator.hpp` | `fiction/utils/stl/gray_code_iterator.hpp` |
| `fiction/utils/hash.hpp`                         | `fiction/utils/stl/hash.hpp`               |
| `fiction/utils/map_utils.hpp`                    | `fiction/utils/stl/map_utils.hpp`          |
| `fiction/utils/phmap_utils.hpp`                  | `fiction/utils/stl/phmap_utils.hpp`        |
| `fiction/utils/stl_utils.hpp`                    | `fiction/utils/stl/stl_utils.hpp`          |

### Headers that kept their path

Their namespace still changed — see the tree above.

| path                                                 | new namespace              |
| ---------------------------------------------------- | -------------------------- |
| `fiction/layouts/bounding_box.hpp`                   | `fiction::layouts`         |
| `fiction/layouts/cartesian_layout.hpp`               | `fiction::layouts`         |
| `fiction/layouts/cell_level_layout.hpp`              | `fiction::layouts`         |
| `fiction/layouts/clocked_layout.hpp`                 | `fiction::layouts`         |
| `fiction/layouts/clocking_scheme.hpp`                | `fiction::layouts`         |
| `fiction/layouts/coordinates.hpp`                    | `fiction::layouts`         |
| `fiction/layouts/gate_level_layout.hpp`              | `fiction::layouts`         |
| `fiction/layouts/hexagonal_layout.hpp`               | `fiction::layouts`         |
| `fiction/layouts/obstruction_layout.hpp`             | `fiction::layouts`         |
| `fiction/layouts/shifted_cartesian_layout.hpp`       | `fiction::layouts`         |
| `fiction/layouts/synchronization_element_layout.hpp` | `fiction::layouts`         |
| `fiction/layouts/tile_based_layout.hpp`              | `fiction::layouts`         |
| `fiction/networks/technology_network.hpp`            | `fiction::networks`        |
| `fiction/networks/views/bfs_topo_view.hpp`           | `fiction::networks::views` |
| `fiction/networks/views/edge_color_view.hpp`         | `fiction::networks::views` |
| `fiction/networks/views/mutable_rank_view.hpp`       | `fiction::networks::views` |
| `fiction/networks/views/static_depth_view.hpp`       | `fiction::networks::views` |
| `fiction/networks/virtual_pi_network.hpp`            | `fiction::networks`        |
| `fiction/traits.hpp`                                 | `fiction`                  |
| `fiction/types.hpp`                                  | `fiction`                  |
| `fiction/utils/debug/layout_printer.hpp`             | `fiction::utils::debug`    |
| `fiction/utils/debug/network_writer.hpp`             | `fiction::utils::debug`    |
| `fiction/utils/version_info.hpp.in`                  | `fiction::utils`           |

### New files

| path                                             | namespace              | why                                                                       |
| ------------------------------------------------ | ---------------------- | ------------------------------------------------------------------------- |
| `fiction/layouts/io/layout_drawers.hpp`          | `fiction::layouts::io` | gate-layout drawers split out of io/dot_drawers.hpp                       |
| `fiction/technology/inml/technology.hpp`         | `fiction::inml`        | inml_technology, split out of cell_technologies.hpp                       |
| `fiction/technology/qca/io/write_qll_layout.hpp` | `fiction::qca::io`     | re-export of inml::io::write_qll_layout; QLL serves QCA, molQCA and iNML  |
| `fiction/technology/qca/technology.hpp`          | `fiction::qca`         | qca_technology and mol_qca_technology, split out of cell_technologies.hpp |
| `fiction/technology/sidb/technology.hpp`         | `fiction::sidb`        | sidb_technology, split out of cell_technologies.hpp                       |

### Renamed symbols

60 public symbols changed name as well as namespace. Everything not listed
here kept its identifier and only gained a namespace.

| old                                               | new                                                          |
| ------------------------------------------------- | ------------------------------------------------------------ |
| `fiction::area`                                   | `fiction::layouts::coords::area`                             |
| `fiction::constants`                              | `fiction::fcn::constants`                                    |
| `fiction::coord_iterator`                         | `fiction::layouts::coords::iterator`                         |
| `fiction::cube::coord_t`                          | `fiction::layouts::coords::cube`                             |
| `fiction::fcn_gate_library`                       | `fiction::fcn::gate_library`                                 |
| `fiction::has_offset_ucoord_v`                    | `fiction::has_offset_coord_v`                                |
| `fiction::inml_technology`                        | `fiction::inml::technology`                                  |
| `fiction::inml_topolinano_library`                | `fiction::inml::topolinano_library`                          |
| `fiction::is_offset_ucoord_v`                     | `fiction::is_offset_coord_v`                                 |
| `fiction::is_sidb_gate_design_impossible`         | `fiction::sidb::generators::is_gate_design_impossible`       |
| `fiction::mol_qca_technology`                     | `fiction::qca::mol_technology`                               |
| `fiction::offset::ucoord_t`                       | `fiction::layouts::coords::offset`                           |
| `fiction::offset_to_cube_coord`                   | `fiction::layouts::coords::offset_to_cube`                   |
| `fiction::on_the_fly_sidb_circuit_design`         | `fiction::sidb::generators::on_the_fly_circuit_design`       |
| `fiction::qca_one_library`                        | `fiction::qca::qca_one_library`                              |
| `fiction::qca_technology`                         | `fiction::qca::technology`                                   |
| `fiction::random_sidb_layout_generator`           | `fiction::sidb::generators::random_layout_generator`         |
| `fiction::sidb_100_lattice`                       | `fiction::sidb::primitives::lattice_100`                     |
| `fiction::sidb_111_lattice`                       | `fiction::sidb::primitives::lattice_111`                     |
| `fiction::sidb_bestagon_library`                  | `fiction::sidb::libraries::bestagon_library`                 |
| `fiction::sidb_binary_cluster_hierarchy_node`     | `fiction::sidb::model::binary_cluster_hierarchy_node`        |
| `fiction::sidb_binary_cluster_hierarchy_node_ptr` | `fiction::sidb::model::binary_cluster_hierarchy_node_ptr`    |
| `fiction::sidb_charge_space_composition`          | `fiction::sidb::model::charge_space_composition`             |
| `fiction::sidb_charge_state`                      | `fiction::sidb::model::charge_state`                         |
| `fiction::sidb_charge_states_for_base_number`     | `fiction::sidb::model::charge_states_for_base_number`        |
| `fiction::sidb_cluster`                           | `fiction::sidb::model::cluster`                              |
| `fiction::sidb_cluster_charge_state`              | `fiction::sidb::model::cluster_charge_state`                 |
| `fiction::sidb_cluster_charge_state_space`        | `fiction::sidb::model::cluster_charge_state_space`           |
| `fiction::sidb_cluster_hierarchy`                 | `fiction::sidb::model::cluster_hierarchy`                    |
| `fiction::sidb_cluster_hierarchy_linkage_method`  | `fiction::sidb::model::cluster_hierarchy_linkage_method`     |
| `fiction::sidb_cluster_projector_state`           | `fiction::sidb::model::cluster_projector_state`              |
| `fiction::sidb_cluster_projector_state_ptr`       | `fiction::sidb::model::cluster_projector_state_ptr`          |
| `fiction::sidb_cluster_ptr`                       | `fiction::sidb::model::cluster_ptr`                          |
| `fiction::sidb_cluster_ptr_hash`                  | `fiction::sidb::model::cluster_ptr_hash`                     |
| `fiction::sidb_cluster_receptor_state`            | `fiction::sidb::model::cluster_receptor_state`               |
| `fiction::sidb_clustering`                        | `fiction::sidb::model::clustering`                           |
| `fiction::sidb_clustering_state`                  | `fiction::sidb::model::clustering_state`                     |
| `fiction::sidb_defect`                            | `fiction::sidb::model::defect`                               |
| `fiction::sidb_defect_surface`                    | `fiction::sidb::primitives::defect_surface`                  |
| `fiction::sidb_defect_surface_params`             | `fiction::sidb::primitives::defect_surface_params`           |
| `fiction::sidb_defect_type`                       | `fiction::sidb::model::defect_type`                          |
| `fiction::sidb_energy_and_state_type`             | `fiction::sidb::simulation::analysis::energy_and_state_type` |
| `fiction::sidb_lattice`                           | `fiction::sidb::primitives::lattice`                         |
| `fiction::sidb_nm_distance`                       | `fiction::sidb::model::nm_distance`                          |
| `fiction::sidb_nm_position`                       | `fiction::sidb::model::nm_position`                          |
| `fiction::sidb_on_the_fly_gate_library`           | `fiction::sidb::libraries::on_the_fly_gate_library`          |
| `fiction::sidb_on_the_fly_gate_library_params`    | `fiction::sidb::libraries::on_the_fly_gate_library_params`   |
| `fiction::sidb_simulation_domain`                 | `fiction::sidb::simulation::domain`                          |
| `fiction::sidb_simulation_engine`                 | `fiction::sidb::simulation::engine`                          |
| `fiction::sidb_simulation_engine_name`            | `fiction::sidb::simulation::engine_name`                     |
| `fiction::sidb_simulation_parameters`             | `fiction::sidb::model::simulation_parameters`                |
| `fiction::sidb_simulation_result`                 | `fiction::sidb::simulation::result`                          |
| `fiction::sidb_skeleton_bestagon_library`         | `fiction::sidb::libraries::skeleton_bestagon_library`        |
| `fiction::sidb_surface_analysis`                  | `fiction::sidb::libraries::surface_analysis`                 |
| `fiction::sidb_technology`                        | `fiction::sidb::technology`                                  |
| `fiction::sim7_mol_library`                       | `fiction::qca::sim7_mol_library`                             |
| `fiction::siqad::coord_t`                         | `fiction::layouts::coords::siqad`                            |
| `fiction::siqad::to_fiction_coord`                | `fiction::layouts::coords::to_fiction_coord`                 |
| `fiction::siqad::to_siqad_coord`                  | `fiction::layouts::coords::to_siqad_coord`                   |
| `fiction::volume`                                 | `fiction::layouts::coords::volume`                           |

Two rows change only the namespace. They are listed because the prefix looks droppable
and deliberately was not dropped: `qca_one_library` is the published name of the QCA ONE
gate library, and `sim7_mol_library` names the SIM(7)-MolPDK library.

### Deliberately unchanged

- **`fiction/traits.hpp` and `fiction/types.hpp`** keep their paths and stay in plain
  `fiction::`. They are the library's root vocabulary and belong to no single module.
- **The 36 layout aliases in `types.hpp`** keep their technology prefixes:

  ```text
  qca_cell_clk_lyt                        qca_cell_clk_lyt_ptr
  mol_qca_cell_clk_lyt                    mol_qca_cell_clk_lyt_ptr
  inml_cell_clk_lyt                       inml_cell_clk_lyt_ptr
  sidb_cell_clk_lyt                       sidb_cell_clk_lyt_ptr
  sidb_cell_clk_lyt_cube                  sidb_cell_clk_lyt_cube_ptr
  sidb_cell_clk_lyt_siqad                 sidb_cell_clk_lyt_siqad_ptr
  sidb_100_cell_clk_lyt                   sidb_100_cell_clk_lyt_ptr
  sidb_100_cell_clk_lyt_cube              sidb_100_cell_clk_lyt_cube_ptr
  sidb_100_cell_clk_lyt_siqad             sidb_100_cell_clk_lyt_siqad_ptr
  sidb_111_cell_clk_lyt                   sidb_111_cell_clk_lyt_ptr
  sidb_111_cell_clk_lyt_cube              sidb_111_cell_clk_lyt_cube_ptr
  sidb_111_cell_clk_lyt_siqad             sidb_111_cell_clk_lyt_siqad_ptr
  sidb_defect_cell_clk_lyt                sidb_defect_cell_clk_lyt_ptr
  sidb_defect_cell_clk_lyt_cube           sidb_defect_cell_clk_lyt_cube_ptr
  sidb_defect_cell_clk_lyt_siqad          sidb_defect_cell_clk_lyt_siqad_ptr
  sidb_defect_100_cell_clk_lyt            sidb_defect_100_cell_clk_lyt_ptr
  sidb_defect_100_cell_clk_lyt_cube       sidb_defect_100_cell_clk_lyt_cube_ptr
  sidb_defect_100_cell_clk_lyt_siqad      sidb_defect_100_cell_clk_lyt_siqad_ptr
  ```

  These live in plain `fiction::`, where the prefix is the only thing distinguishing
  `qca_cell_clk_lyt` from `sidb_cell_clk_lyt`. Dropping it would collide.

- **Nested members keep their names.** `fcn_gate_library::fcn_gate`,
  `sidb_defect_surface::sidb_surface_storage` and
  `write_sidb_layout_svg_params::sidb_lattice_mode` are scoped by their enclosing type,
  not by a namespace, so the rename pass leaves them alone.
- **`detail` namespaces** are untouched. They keep working nested.
- **`experiments/`** changed only its include directives. It reproduces published
  papers and is not refactored.

### Renamed struct members

Ten parameter and result structs declared a member whose name repeated its type:

```cpp
sidb_simulation_parameters simulation_parameters{};
```

The member is now `sim_params`, which is what two of these structs and several
locals already called it:

```cpp
sidb::model::simulation_parameters sim_params{};
```

So `params.simulation_parameters` becomes `params.sim_params`. The structs affected
are `quickexact_params`, `quicksim_params`, `clustercomplete_params`,
`is_operational_params`, `critical_temperature_params`,
`physical_population_stability_params`, `generate_random_sidb_layout_params`,
`ground_state_space_params`, `sidb_simulation_result`, and
`charge_distribution_surface`'s storage. `is_sidb_gate_design_impossible_params`
spelled it `simulation_params` and is unified to `sim_params` too.

### A name collision to know about

`fiction::technology<Lyt>` is a trait alias in `traits.hpp` that yields a layout's
technology tag. The tags themselves are now called `technology` inside their own
namespace, so within `fiction::qca`, `fiction::inml`, and `fiction::sidb` the struct
shadows the trait. Code in those namespaces that wants the trait must name it
`fiction::technology<Lyt>` explicitly. Outside them, both resolve as before.

### Notable splits

Three headers were split rather than moved. If you included one of them, you may now
need two includes.

- **`fiction/io/dot_drawers.hpp`** → `fiction/networks/io/dot_drawers.hpp` keeps the
  network drawers (`technology_dot_drawer`, `color_view_drawer`,
  `edge_color_view_drawer`); the gate-layout drawers (`simple_gate_layout_tile_drawer`,
  `gate_layout_cartesian_drawer`, `gate_layout_shifted_cartesian_drawer`,
  `gate_layout_hexagonal_drawer`) move to `fiction/layouts/io/layout_drawers.hpp`.
- **`fiction/technology/cell_technologies.hpp`** → the four technology tags each move
  to their own technology directory. `fiction/technology/fcn/cell_technologies.hpp`
  remains as an umbrella header that includes all of them, so existing code that wants
  every tag needs only the new path.
- **`fiction/io/write_qll_layout.hpp`** → defined at
  `fiction/technology/inml/io/write_qll_layout.hpp`, because QLL is the ToPoliNano and
  MagCAD format and that is the iNML toolchain. The format also serves QCA and molQCA,
  so `fiction/technology/qca/io/write_qll_layout.hpp` re-exports it into
  `fiction::qca::io`. Either include works.
