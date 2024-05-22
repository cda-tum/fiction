from mnt.pyfiction import *
import unittest


class TestBDLInputIterator(unittest.TestCase):
    def test_empty_layout(self):
        layout = sidb_100_lattice((0,0))

        bii = bdl_input_iterator_100(layout)

        self.assertEqual(bii.num_input_pairs(), 0)
        self.assertEqual(bii, 0)
        self.assertNotEqual(bii, 1)
        self.assertLess(bii, 1)
        self.assertLessEqual(bii, 1)
        self.assertGreaterEqual(bii, 0)


if __name__ == '__main__':
    unittest.main()
