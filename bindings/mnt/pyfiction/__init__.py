"""MNT fiction framework.

This file is part of the MNT fiction framework released under the MIT license.
See https://github.com/cda-tum/fiction for more information.
"""

import os
import sys
from pathlib import Path

if sys.platform == "win32" and sys.version_info > (3, 8, 0) and "Z3_ROOT" in os.environ:
    lib_path = Path(os.environ["Z3_ROOT"]) / "lib"
    if lib_path.exists():
        os.add_dll_directory(str(lib_path))
    bin_path = Path(os.environ["Z3_ROOT"]) / "bin"
    if bin_path.exists():
        os.add_dll_directory(str(bin_path))

from .pyfiction import (
    # Algorithms
    ## iter
    input_bdl_configuration,
    bdl_input_iterator_params,
    bdl_input_iterator_100,
    bdl_input_iterator_111,

    ## network_transformation
    substitution_strategy,
    fanout_substitution_params,
    fanout_substitution,
    is_fanout_substituted,

    network_balancing_params,
    network_balancing,
    is_balanced,

    technology_mapping_params,
    technology_mapping_stats,
    and_or_not,
    and_or_not_maj,
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    technology_mapping,

    ## path_finding
    a_star_params,
    a_star,
    a_star_distance,

    manhattan_distance,
    euclidean_distance,
    squared_euclidean_distance,
    twoddwave_distance,
    chebyshev_distance,

    enumerate_all_paths_params,
    enumerate_all_paths,

    yen_k_shortest_paths_params,
    yen_k_shortest_paths,

    ## physical_design
    apply_qca_one_library,
    apply_topolinano_library,
    apply_bestagon_library,

    graph_coloring_engine,
    color_routing_params,
    color_routing,

    design_sidb_gates_mode,
    design_sidb_gates_params,
    design_sidb_gates,

    # technology_constraints,
    # exact_params,
    # exact_stats,
    # exact_cartesian,
    # exact_shifted_cartesian,
    # exact_hexagonal,

    gold_effort_mode,
    gold_cost_objective,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
    graph_oriented_layout_design,

    hexagonalization,

    orthogonal_params,
    orthogonal_stats,
    orthogonal,

    post_layout_optimization_params,
    post_layout_optimization_stats,
    post_layout_optimization,

    wiring_reduction_params,
    wiring_reduction_stats,
    wiring_reduction,

    ## properties
    critical_path_length_and_throughput,

    ## simulation
    ### sidb
    transition_type,
    population_stability_information,
    assess_physical_population_stability_params,
    assess_physical_population_stability,

    calculate_energy_and_state_type_with_kinks_accepted,
    calculate_energy_and_state_type_with_kinks_rejected,
    can_positive_charges_occur,
    check_simulation_results_for_equivalence,

    compute_operational_ratio_params,
    compute_operational_ratio,

    convert_potential_to_distance,

    critical_temperature_stats,
    critical_temperature_params,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,

    bdl_pair,
    detect_bdl_pairs_params,
    detect_bdl_pairs,

    bdl_wire_selection,
    detect_bdl_wires_params,
    bdl_wire_100,
    bdl_wire_111,
    detect_bdl_wires_100,
    detect_bdl_wires_111,

    determine_groundstate_from_simulation_results,

    physically_valid_parameters_domain,
    determine_physically_valid_parameters,

    dimer_displacement_policy,
    displacement_analysis_mode,
    displacement_robustness_domain_params,
    displacement_robustness_domain_stats,
    displacement_robustness_domain_100,
    displacement_robustness_domain_111,
    determine_displacement_robustness_domain_100,
    determine_displacement_robustness_domain_111,

    energy_distribution,

    exhaustive_ground_state_simulation,

    is_ground_state,

    operational_status,
    operational_condition,
    is_operational_params,
    is_operational,
    operational_input_patterns,

    # maximum_defect_influence_distance_params,
    # maximum_defect_influence_position_and_distance,

    minimum_energy,

    occupation_probability_gate_based,
    occupation_probability_non_gate_based,

    parameter_point,
    sweep_parameter,
    operational_domain,
    operational_domain_value_range,
    operational_domain_params,
    operational_domain_stats,
    operational_domain_grid_search,
    operational_domain_random_sampling,
    operational_domain_flood_fill,
    operational_domain_contour_tracing,

    automatic_base_number_detection,
    quickexact_params,
    quickexact,

    quicksim_params,
    quicksim,

    positive_charges,
    generate_random_sidb_layout_params,
    generate_random_sidb_layout,
    generate_multiple_random_sidb_layouts,

    sidb_simulation_engine,
    exact_sidb_simulation_engine,

    sidb_simulation_parameters,

    sidb_simulation_result_100,
    sidb_simulation_result_111,

    time_to_solution_params,
    time_to_solution_stats,
    time_to_solution,
    time_to_solution_for_given_simulation_results,

    ### logic
    simulate,

    ## verfication
    gate_level_drv_params,
    gate_level_drvs,

    eq_type,
    equivalence_checking_stats,
    equivalence_checking,

    # Inout
    fgl_parsing_error,
    read_cartesian_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,

    unsupported_character_exception,
    undefined_cell_label_exception,
    unrecognized_cell_definition_exception,
    read_fqca_layout,

    sqd_parsing_error,
    read_sqd_layout_100,
    read_sqd_layout_111,

    write_dot_network,
    write_dot_layout,

    write_fgl_layout,

    out_of_cell_names_exception,
    write_fqca_layout_params,
    write_fqca_layout,

    # write_location_and_ground_state,

    sample_writing_mode,
    write_operational_domain_params,
    write_operational_domain,

    write_qca_layout_params,
    write_qca_layout,

    write_qcc_layout,

    write_qll_layout,

    write_sqd_layout,

    write_sqd_sim_result,

    write_qca_layout_svg_params,
    write_qca_layout_svg,
    color_mode,
    sidb_lattice_mode,
    write_sidb_layout_svg_params,
    write_sidb_layout_svg,
    write_sidb_layout_svg_to_string,

    # Layouts
    cartesian_layout,

    qca_technology,
    inml_technology,
    sidb_technology,
    qca_layout,
    inml_layout,
    sidb_layout,

    clocked_cartesian_layout,
    clocked_shifted_cartesian_layout,
    clocked_hexagonal_layout,

    offset_coordinate,
    cube_coordinate,
    siqad_coordinate,
    offset_area,
    cube_area,
    siqad_area,
    offset_volume,
    cube_volume,
    siqad_volume,
    to_offset_coord,
    to_cube_coord,
    to_siqad_coord,

    cartesian_gate_layout,
    shifted_cartesian_gate_layout,
    hexagonal_gate_layout,

    hexagonal_layout,

    cartesian_obstruction_layout,
    shifted_cartesian_obstruction_layout,
    hexagonal_obstruction_layout,

    shifted_cartesian_layout,

    # Networks
    technology_network,
    read_technology_network,
    dynamic_truth_table,

    # Technology
    area,

    dependent_cell_mode,
    energy_calculation,
    charge_distribution_mode,
    charge_index_mode,
    charge_distribution_surface,
    charge_distribution_surface_100,
    charge_distribution_surface_111,

    sidb_charge_state,
    charge_state_to_sign,
    sign_to_charge_state,
    charge_configuration_to_string,

    sidb_defect_type,
    sidb_defect,
    is_charged_defect_type,
    is_neutral_defect_type,
    is_positively_charged_defect,
    is_negatively_charged_defect,
    is_neutrally_charged_defect,
    defect_extent,

    sidb_100_lattice,
    sidb_111_lattice,

    sidb_nm_distance_100,
    sidb_nm_distance_111,

    sidb_nm_position,

    # Utils
    num_adjacent_coordinates,
    normalize_layout_coordinates,
    convert_layout_to_siqad_coordinates,
    random_coordinate,

    get_name,
    set_name,

    high_degree_fanin_exception,
    has_high_degree_fanin_nodes,

    reserve_input_nodes,
    place,

    is_crossable_wire,
    route_path,
    extract_routing_objectives,
    clear_routing,

    create_id_tt,
    create_not_tt,
    create_and_tt,
    create_or_tt,
    create_nand_tt,
    create_nor_tt,
    create_xor_tt,
    create_xnor_tt,
    create_lt_tt,
    create_gt_tt,
    create_le_tt,
    create_ge_tt,
    create_and3_tt,
    create_xor_and_tt,
    create_or_and_tt,
    create_onehot_tt,
    create_maj_tt,
    create_gamble_tt,
    create_dot_tt,
    create_ite_tt,
    create_and_xor_tt,
    create_xor3_tt,
    create_double_wire_tt,
    create_crossing_wire_tt,
    create_fan_out_tt,
    create_half_adder_tt,

    __version__,
    __repo__,
    __compiled_date__,
    __compiled_time__
)

__all__ = [
    # Algorithms
    ## iter
    "input_bdl_configuration",
    "bdl_input_iterator_params",
    "bdl_input_iterator_100",
    "bdl_input_iterator_111",

    ## network_transformation
    "substitution_strategy",
    "fanout_substitution_params",
    "fanout_substitution",
    "is_fanout_substituted",

    "network_balancing_params",
    "network_balancing",
    "is_balanced",

    "technology_mapping_params",
    "technology_mapping_stats",
    "and_or_not",
    "and_or_not_maj",
    "all_standard_2_input_functions",
    "all_standard_3_input_functions",
    "all_supported_standard_functions",
    "technology_mapping",

    ## path_finding
    "a_star_params",
    "a_star",
    "a_star_distance",

    "manhattan_distance",
    "euclidean_distance",
    "squared_euclidean_distance",
    "twoddwave_distance",
    "chebyshev_distance",

    "enumerate_all_paths_params",
    "enumerate_all_paths",

    "yen_k_shortest_paths_params",
    "yen_k_shortest_paths",

    ## physical_design
    "apply_qca_one_library",
    "apply_topolinano_library",
    "apply_bestagon_library",

    "graph_coloring_engine",
    "color_routing_params",
    "color_routing",

    "design_sidb_gates_mode",
    "design_sidb_gates_params",
    "design_sidb_gates",

    # "technology_constraints",
    # "exact_params",
    # "exact_stats",
    # "exact_cartesian",
    # "exact_shifted_cartesian",
    # "exact_hexagonal",

    "gold_effort_mode",
    "gold_cost_objective",
    "graph_oriented_layout_design_params",
    "graph_oriented_layout_design_stats",
    "graph_oriented_layout_design",

    "hexagonalization",

    "orthogonal_params",
    "orthogonal_stats",
    "orthogonal",

    "post_layout_optimization_params",
    "post_layout_optimization_stats",
    "post_layout_optimization",

    "wiring_reduction_params",
    "wiring_reduction_stats",
    "wiring_reduction",

    ## properties
    "critical_path_length_and_throughput",

    ## simulation
    ### sidb
    "transition_type",
    "population_stability_information",
    "assess_physical_population_stability_params",
    "assess_physical_population_stability",

    "calculate_energy_and_state_type_with_kinks_accepted",
    "calculate_energy_and_state_type_with_kinks_rejected",
    "can_positive_charges_occur",
    "check_simulation_results_for_equivalence",

    "compute_operational_ratio_params",
    "compute_operational_ratio",

    "convert_potential_to_distance",

    "critical_temperature_stats",
    "critical_temperature_params",
    "critical_temperature_gate_based",
    "critical_temperature_non_gate_based",

    "bdl_pair",
    "detect_bdl_pairs_params",
    "detect_bdl_pairs",

    "bdl_wire_selection",
    "detect_bdl_wires_params",
    "bdl_wire_100",
    "bdl_wire_111",
    "detect_bdl_wires_100",
    "detect_bdl_wires_111",

    "determine_groundstate_from_simulation_results",

    "physically_valid_parameters_domain",
    "determine_physically_valid_parameters",

    "dimer_displacement_policy",
    "displacement_analysis_mode",
    "displacement_robustness_domain_params",
    "displacement_robustness_domain_stats",
    "displacement_robustness_domain_100",
    "displacement_robustness_domain_111",
    "determine_displacement_robustness_domain_100",
    "determine_displacement_robustness_domain_111",

    "energy_distribution",

    "exhaustive_ground_state_simulation",

    "is_ground_state",

    "operational_status",
    "operational_condition",
    "is_operational_params",
    "is_operational",
    "operational_input_patterns",

    # "maximum_defect_influence_distance_params",
    # "maximum_defect_influence_position_and_distance",

    "minimum_energy",

    "occupation_probability_gate_based",
    "occupation_probability_non_gate_based",

    "parameter_point",
    "sweep_parameter",
    "operational_domain",
    "operational_domain_value_range",
    "operational_domain_params",
    "operational_domain_stats",
    "operational_domain_grid_search",
    "operational_domain_random_sampling",
    "operational_domain_flood_fill",
    "operational_domain_contour_tracing",

    "automatic_base_number_detection",
    "quickexact_params",
    "quickexact",

    "quicksim_params",
    "quicksim",

    "positive_charges",
    "generate_random_sidb_layout_params",
    "generate_random_sidb_layout",
    "generate_multiple_random_sidb_layouts",

    "sidb_simulation_engine",
    "exact_sidb_simulation_engine",

    "sidb_simulation_parameters",

    "sidb_simulation_result_100",
    "sidb_simulation_result_111",

    "time_to_solution_params",
    "time_to_solution_stats",
    "time_to_solution",
    "time_to_solution_for_given_simulation_results",

    ### logic
    "simulate",

    ## verfication
    "gate_level_drv_params",
    "gate_level_drvs",

    "eq_type",
    "equivalence_checking_stats",
    "equivalence_checking",

    # Inout
    "fgl_parsing_error",
    "read_cartesian_fgl_layout",
    "read_shifted_cartesian_fgl_layout",
    "read_hexagonal_fgl_layout",

    "unsupported_character_exception",
    "undefined_cell_label_exception",
    "unrecognized_cell_definition_exception",
    "read_fqca_layout",

    "sqd_parsing_error",
    "read_sqd_layout_100",
    "read_sqd_layout_111",

    "write_dot_network",
    "write_dot_layout",

    "write_fgl_layout",

    "out_of_cell_names_exception",
    "write_fqca_layout_params",
    "write_fqca_layout",

    # "write_location_and_ground_state",

    "sample_writing_mode",
    "write_operational_domain_params",
    "write_operational_domain",

    "write_qca_layout_params",
    "write_qca_layout",

    "write_qcc_layout",

    "write_qll_layout",

    "write_sqd_layout",

    "write_sqd_sim_result",

    "write_qca_layout_svg_params",
    "write_qca_layout_svg",
    "color_mode",
    "sidb_lattice_mode",
    "write_sidb_layout_svg_params",
    "write_sidb_layout_svg",
    "write_sidb_layout_svg_to_string",

    # Layouts
    "cartesian_layout",

    "qca_technology",
    "inml_technology",
    "sidb_technology",
    "qca_layout",
    "inml_layout",
    "sidb_layout",

    "clocked_cartesian_layout",
    "clocked_shifted_cartesian_layout",
    "clocked_hexagonal_layout",

    "offset_coordinate",
    "cube_coordinate",
    "siqad_coordinate",
    "offset_area",
    "cube_area",
    "siqad_area",
    "offset_volume",
    "cube_volume",
    "siqad_volume",
    "to_offset_coord",
    "to_cube_coord",
    "to_siqad_coord",

    "cartesian_gate_layout",
    "shifted_cartesian_gate_layout",
    "hexagonal_gate_layout",

    "hexagonal_layout",

    "cartesian_obstruction_layout",
    "shifted_cartesian_obstruction_layout",
    "hexagonal_obstruction_layout",

    "shifted_cartesian_layout",

    # Networks
    "technology_network",
    "read_technology_network",
    "dynamic_truth_table",

    # Technology
    "area",

    "dependent_cell_mode",
    "energy_calculation",
    "charge_distribution_mode",
    "charge_index_mode",
    "charge_distribution_surface",
    "charge_distribution_surface_100",
    "charge_distribution_surface_111",

    "sidb_charge_state",
    "charge_state_to_sign",
    "sign_to_charge_state",
    "charge_configuration_to_string",

    "sidb_defect_type",
    "sidb_defect",
    "is_charged_defect_type",
    "is_neutral_defect_type",
    "is_positively_charged_defect",
    "is_negatively_charged_defect",
    "is_neutrally_charged_defect",
    "defect_extent",

    "sidb_100_lattice",
    "sidb_111_lattice",

    "sidb_nm_distance_100",
    "sidb_nm_distance_111",

    "sidb_nm_position",

    # Utils
    "num_adjacent_coordinates",
    "normalize_layout_coordinates",
    "convert_layout_to_siqad_coordinates",
    "random_coordinate",

    "get_name",
    "set_name",

    "high_degree_fanin_exception",
    "has_high_degree_fanin_nodes",

    "reserve_input_nodes",
    "place",

    "is_crossable_wire",
    "route_path",
    "extract_routing_objectives",
    "clear_routing",

    "create_id_tt",
    "create_not_tt",
    "create_and_tt",
    "create_or_tt",
    "create_nand_tt",
    "create_nor_tt",
    "create_xor_tt",
    "create_xnor_tt",
    "create_lt_tt",
    "create_gt_tt",
    "create_le_tt",
    "create_ge_tt",
    "create_and3_tt",
    "create_xor_and_tt",
    "create_or_and_tt",
    "create_onehot_tt",
    "create_maj_tt",
    "create_gamble_tt",
    "create_dot_tt",
    "create_ite_tt",
    "create_and_xor_tt",
    "create_xor3_tt",
    "create_double_wire_tt",
    "create_crossing_wire_tt",
    "create_fan_out_tt",
    "create_half_adder_tt",

    "__version__",
    "__repo__",
    "__compiled_date__",
    "__compiled_time__",
]
