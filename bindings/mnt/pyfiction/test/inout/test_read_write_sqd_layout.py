from mnt.pyfiction import read_sqd_layout_100, read_sqd_layout_111


def test_read_write(resources_dir):
    read_sqd_layout_100(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    read_sqd_layout_111(str(resources_dir / "AND_mu_032_111_surface.sqd"))
