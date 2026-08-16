import os
import unittest

from mnt.pyfiction import (
    bdl_input_iterator_params,
    bdl_wire_selection,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    create_and_tt,
    create_not_tt,
    create_xor_tt,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_params,
    critical_temperature_stats,
    detect_bdl_pairs,
    detect_bdl_pairs_params,
    detect_bdl_wires_100,
    detect_bdl_wires_params,
    generate_bdl_input_pattern_layouts,
    read_sqd_layout_100,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_simulation_engine,
    sidb_simulation_parameters,
    sidb_technology,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestCriticalTemperature(unittest.TestCase):
    def test_perturber_and_DB_pair_100(self):
        layout = sidb_100_lattice((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)

        self.assertEqual(critical_temperature_non_gate_based(cds, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickExact")
        self.assertEqual(stats.num_valid_lyt, 1)

    def test_perturber_and_DB_pair_111(self):
        layout = sidb_111_lattice((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_111(layout)

        self.assertEqual(critical_temperature_non_gate_based(cds, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickExact")
        self.assertEqual(stats.num_valid_lyt, 1)

    def test_gate_based_simulation(self):
        layout = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd", "xor_gate")
        params = critical_temperature_params()

        params.operational_params.simulation_parameters.base = 2

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_xor_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 200)

        self.assertEqual(stats.algorithm_name, "QuickExact")

    def test_bestagon_inv(self):
        layout = read_sqd_layout_100(
            dir_path + "/../../../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd",
            "inverter_input_0",
        )

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKSIM

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_not_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickSim")
        self.assertGreater(stats.num_valid_lyt, 1)

    def test_bestagon_inv_with_different_mu(self):
        layout = read_sqd_layout_100(
            dir_path + "/../../../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd",
            "inverter_input_0",
        )

        params = critical_temperature_params()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.2

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_not_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 5)

        self.assertEqual(stats.algorithm_name, "QuickExact")

    def test_critical_temperature_with_input_pattern_layouts(self):
        lyt = sidb_100_lattice()

        lyt.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((19, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((10, 12, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((10, 14, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

        params = critical_temperature_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

        input_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT)
        output_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT)
        output_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.OUTPUT, detect_bdl_pairs_params())

        input_pattern_layouts = generate_bdl_input_pattern_layouts(
            lyt,
            bdl_input_iterator_params(),
            input_bdl_wires,
        )

        # a 2-input gate has 4 input patterns
        self.assertEqual(len(input_pattern_layouts), 4)

        reference_stats = critical_temperature_stats()
        reference_ct = critical_temperature_gate_based(lyt, [create_and_tt()], params, reference_stats)

        stats = critical_temperature_stats()
        ct = critical_temperature_gate_based(
            input_pattern_layouts,
            [create_and_tt()],
            params,
            output_bdl_pairs,
            input_bdl_wires,
            output_bdl_wires,
            stats,
        )

        # the two overloads run the same computation, so the results must be identical
        self.assertEqual(ct, reference_ct)
        self.assertEqual(stats.num_valid_lyt, reference_stats.num_valid_lyt)
        self.assertEqual(stats.algorithm_name, reference_stats.algorithm_name)

        # a layout list that does not match the specification is rejected
        with self.assertRaises(ValueError):
            critical_temperature_gate_based(
                input_pattern_layouts[:1],
                [create_and_tt()],
                params,
                output_bdl_pairs,
                input_bdl_wires,
                output_bdl_wires,
            )

        # more output BDL pairs than truth tables is rejected
        with self.assertRaises(ValueError):
            critical_temperature_gate_based(
                input_pattern_layouts,
                [create_and_tt()],
                params,
                [*output_bdl_pairs, output_bdl_pairs[0]],
                input_bdl_wires,
                output_bdl_wires,
            )


if __name__ == "__main__":
    unittest.main()
