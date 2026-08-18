from mnt.pyfiction import eq_type, equivalence_checking, equivalence_checking_stats, read_technology_network


def test_non_eq(resources_dir):
    xor2_net = read_technology_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_technology_network(str(resources_dir / "xnor2.v"))

    stats = equivalence_checking_stats()
    assert stats.counter_example == []

    eq = equivalence_checking(xor2_net, xnor2_net, stats)
    assert eq == eq_type.NO
    assert stats.counter_example == [True, False]
