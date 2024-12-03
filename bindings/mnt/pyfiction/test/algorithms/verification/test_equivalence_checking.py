import os
import unittest

from mnt.pyfiction import eq_type, equivalence_checking, equivalence_checking_stats, read_technology_network

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestEquivalenceChecking(unittest.TestCase):
    def test_non_eq(self):
        xor2_net = read_technology_network(dir_path + "/../../resources/xor2.v")
        xnor2_net = read_technology_network(dir_path + "/../../resources/xnor2.v")

        stats = equivalence_checking_stats()
        self.assertEqual(stats.counter_example, [])

        eq = equivalence_checking(xor2_net, xnor2_net, stats)
        self.assertEqual(eq, eq_type.NO)
        self.assertEqual(stats.counter_example, [True, False])


if __name__ == "__main__":
    unittest.main()
