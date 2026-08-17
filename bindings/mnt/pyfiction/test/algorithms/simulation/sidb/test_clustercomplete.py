import pytest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    clustercomplete,
    clustercomplete_params,
    ground_state_space_reporting,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_technology,
)


def test_three_sidbs():
    layout = sidb_100_lattice((2, 1))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

    params = clustercomplete_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.25
    params.validity_witness_partitioning_max_cluster_size_gss = 15
    params.num_overlapping_witnesses_limit_gss = 8
    params.available_threads = 4
    params.report_gss_stats = ground_state_space_reporting.ON
    assert params.simulation_parameters.base == 2
    assert params.simulation_parameters.mu_minus == pytest.approx(-0.25)
    assert params.validity_witness_partitioning_max_cluster_size_gss == 15
    assert params.num_overlapping_witnesses_limit_gss == 8
    assert params.available_threads == 4
    assert params.report_gss_stats == ground_state_space_reporting.ON

    cds = charge_distribution_surface_100(layout)

    result = clustercomplete(cds, params)

    assert result.algorithm_name == "ClusterComplete"
    assert len(result.charge_distributions) <= 3

    params.simulation_parameters.base = 3
    assert params.simulation_parameters.base == 3

    result = clustercomplete(cds, params)
    assert len(result.charge_distributions) <= 4

    params.simulation_parameters.epsilon_r = 2
    params.simulation_parameters.lambda_tf = 2
    result = clustercomplete(cds, params)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111():
    layout = sidb_111_lattice((4, 1))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((3, 0), sidb_technology.cell_type.NORMAL)

    params = clustercomplete_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.32
    assert params.simulation_parameters.mu_minus == pytest.approx(-0.32)

    cds = charge_distribution_surface_111(layout)

    result = clustercomplete(cds, params)

    assert result.algorithm_name == "ClusterComplete"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state((0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state((1, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((2, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((3, 0)) == sidb_charge_state.NEGATIVE
