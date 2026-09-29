# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Placement and routing of logic networks into gate-level layouts."""

from __future__ import annotations

from mnt.pyfiction._native.physical_design import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    clear_routing,
    color_routing,
    color_routing_params,
    extract_routing_objectives,
    gold_cost_objective,
    gold_effort_mode,
    graph_coloring_engine,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_io_pin_routing_error,
    hexagonalization_params,
    hexagonalization_stats,
    is_crossable_wire,
    orthogonal,
    orthogonal_even_column_hex,
    orthogonal_hexagonal,
    orthogonal_odd_column_hex,
    orthogonal_odd_row_hex,
    orthogonal_params,
    orthogonal_stats,
    place,
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    reserve_input_nodes,
    route_path,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)

from . import path_finding

__all__ = [
    "apply_bestagon_library",
    "apply_qca_one_library",
    "apply_sim7_mol_library",
    "apply_topolinano_library",
    "clear_routing",
    "color_routing",
    "color_routing_params",
    "extract_routing_objectives",
    "gold_cost_objective",
    "gold_effort_mode",
    "graph_coloring_engine",
    "graph_oriented_layout_design",
    "graph_oriented_layout_design_params",
    "graph_oriented_layout_design_stats",
    "hexagonalization",
    "hexagonalization_io_pin_extension_mode",
    "hexagonalization_io_pin_routing_error",
    "hexagonalization_params",
    "hexagonalization_stats",
    "is_crossable_wire",
    "orthogonal",
    "orthogonal_even_column_hex",
    "orthogonal_hexagonal",
    "orthogonal_odd_column_hex",
    "orthogonal_odd_row_hex",
    "orthogonal_params",
    "orthogonal_stats",
    "path_finding",
    "place",
    "post_layout_optimization",
    "post_layout_optimization_params",
    "post_layout_optimization_stats",
    "reserve_input_nodes",
    "route_path",
    "wiring_reduction",
    "wiring_reduction_params",
    "wiring_reduction_stats",
]

try:
    from mnt.pyfiction._native.physical_design import (
        exact_cartesian,
        exact_even_column_cartesian,
        exact_even_column_hex,
        exact_even_row_cartesian,
        exact_hexagonal,
        exact_odd_column_hex,
        exact_odd_row_cartesian,
        exact_odd_row_hex,
        exact_params,
        exact_shifted_cartesian,
        exact_stats,
        num_clks,
        technology_constraints,
    )
except ImportError:
    pass
else:
    __all__ += [
        "exact_cartesian",
        "exact_even_column_cartesian",
        "exact_even_column_hex",
        "exact_even_row_cartesian",
        "exact_hexagonal",
        "exact_odd_column_hex",
        "exact_odd_row_cartesian",
        "exact_odd_row_hex",
        "exact_params",
        "exact_shifted_cartesian",
        "exact_stats",
        "num_clks",
        "technology_constraints",
    ]
