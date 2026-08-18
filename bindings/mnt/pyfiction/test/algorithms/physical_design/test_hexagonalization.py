from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_params,
    hexagonalization_stats,
    orthogonal,
)


def test_hexagonalization_default(mux21):
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    hex_layout = hexagonalization(cart_layout)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG


def test_hexagonalization_with_parameters(mux21):
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    params = hexagonalization_params()
    hex_layout = hexagonalization(cart_layout, params)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG


def test_hexagonalization_with_stats(mux21):
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, statistics=stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0


def test_hexagonalization_with_stats_and_parameters(mux21):
    cart_layout = orthogonal(mux21)
    assert equivalence_checking(mux21, cart_layout) == eq_type.STRONG

    params = hexagonalization_params()
    params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.NONE
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0

    params.input_pin_extension = hexagonalization_io_pin_extension_mode.NONE
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.NONE
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0

    params.input_pin_extension = hexagonalization_io_pin_extension_mode.NONE
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()

    params.input_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    params.output_pin_extension = hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
    stats = hexagonalization_stats()
    hex_layout = hexagonalization(cart_layout, params, stats)
    assert equivalence_checking(mux21, hex_layout) == eq_type.STRONG
    assert equivalence_checking(cart_layout, hex_layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    for pi in hex_layout.pis():
        assert pi.y == 0
    for po in hex_layout.pos():
        assert po.y == hex_layout.y()
