from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestAStarPhysicalDesign(unittest.TestCase):

    def test_a_star_pr(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = a_star_pr_params()
        params.return_first = True

        stats = a_star_pr_stats()

        layout = a_star_pr(network, params, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
