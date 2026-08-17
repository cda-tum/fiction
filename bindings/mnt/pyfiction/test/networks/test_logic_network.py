import os

import pytest

from mnt.pyfiction import read_technology_network

dir_path = os.path.dirname(os.path.realpath(__file__))


def test_read_technology_network():
    network = read_technology_network(dir_path + "/../resources/mux21.v")

    assert network.size() == 10
    assert network.nodes() == list(range(10))
    assert network.num_gates() == 5
    assert network.gates() == [5, 6, 7, 8, 9]
    assert network.is_inv(5)
    assert network.is_and(6)
    assert network.is_and(7)
    assert network.is_or(8)

    assert network.is_constant(0)
    assert network.is_constant(1)

    assert network.num_pis() == 3
    assert network.pis() == [2, 3, 4]
    assert network.is_pi(2)
    assert network.is_pi(3)
    assert network.is_pi(4)
    assert network.has_name(2)
    assert network.has_name(3)
    assert network.has_name(4)
    assert network.get_name(2) == "in0"
    assert network.get_name(3) == "in1"
    assert network.get_name(4) == "in2"

    assert network.num_pos() == 1
    assert network.pos() == [9]
    assert network.is_po(9)
    assert network.po_index(9) == 0
    assert network.po_at(0) == 9
    assert network.has_output_name(network.po_index(9))
    assert network.get_output_name(network.po_index(9)) == "out"

    assert network.fanins(0) == []
    assert network.fanins(1) == []
    assert network.fanins(2) == []
    assert network.fanins(3) == []
    assert network.fanins(4) == []
    assert network.fanins(5) == [4]
    assert network.fanins(6) == [2, 5]
    assert network.fanins(7) == [3, 4]
    assert network.fanins(8) == [6, 7]
    assert network.fanins(9) == [8]

    with pytest.raises(RuntimeError):
        read_technology_network(dir_path + "/mux41.v")


def test_is_gate_functions(resources_dir):
    network = read_technology_network(os.path.join(resources_dir, "mux21.v"))

    for i in network.nodes():
        assert not network.is_maj(i)
        assert not network.is_xor(i)
        assert not network.is_xnor(i)
        assert not network.is_nand(i)
        assert not network.is_nor(i)

    network = read_technology_network(os.path.join(resources_dir, "FA.v"))

    assert network.is_xor(6)

    for i in network.nodes():
        assert not network.is_fanout(i)
        assert not network.is_maj(i)
        assert not network.is_xnor(i)
        assert not network.is_nand(i)
        assert not network.is_nor(i)
