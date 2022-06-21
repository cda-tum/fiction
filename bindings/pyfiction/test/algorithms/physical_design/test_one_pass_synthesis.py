# from fiction import pyfiction
# import unittest
# import os
#
# dir_path = os.path.dirname(os.path.realpath(__file__))
#
#
# class TestOnePassSynthesisPhysicalDesign(unittest.TestCase):
#
#     def test_one_pass_synthesis_default(self):
#         network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
#         layout = pyfiction.one_pass_synthesis(network)
#
#     def test_one_pass_synthesis_with_parameters(self):
#         network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
#
#         params = pyfiction.one_pass_synthesis_params()
#         params.crossings = True
#         params.scheme = "ESP"
#
#         layout = pyfiction.one_pass_synthesis(network, params)
#
#     def test_one_pass_synthesis_with_stats(self):
#         network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
#
#         stats = pyfiction.one_pass_synthesis_stats()
#
#         layout = pyfiction.one_pass_synthesis(network, statistics=stats)
