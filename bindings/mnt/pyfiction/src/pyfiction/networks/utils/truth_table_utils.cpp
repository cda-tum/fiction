//
// Created by marcel on 15.11.23.
//

#include "pyfiction/documentation.hpp"

#include <fiction/networks/utils/truth_table_utils.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void truth_table_utils(nanobind::module_& m)
{
    m.def("create_id_tt", &fiction::networks::utils::create_id_tt, DOC(fiction_networks_utils_create_id_tt));
    m.def("create_not_tt", &fiction::networks::utils::create_not_tt, DOC(fiction_networks_utils_create_not_tt));
    m.def("create_and_tt", &fiction::networks::utils::create_and_tt, DOC(fiction_networks_utils_create_and_tt));
    m.def("create_or_tt", &fiction::networks::utils::create_or_tt, DOC(fiction_networks_utils_create_or_tt));
    m.def("create_nand_tt", &fiction::networks::utils::create_nand_tt, DOC(fiction_networks_utils_create_nand_tt));
    m.def("create_nor_tt", &fiction::networks::utils::create_nor_tt, DOC(fiction_networks_utils_create_nor_tt));
    m.def("create_xor_tt", &fiction::networks::utils::create_xor_tt, DOC(fiction_networks_utils_create_xor_tt));
    m.def("create_xnor_tt", &fiction::networks::utils::create_xnor_tt, DOC(fiction_networks_utils_create_xnor_tt));
    m.def("create_lt_tt", &fiction::networks::utils::create_lt_tt, DOC(fiction_networks_utils_create_lt_tt));
    m.def("create_gt_tt", &fiction::networks::utils::create_gt_tt, DOC(fiction_networks_utils_create_gt_tt));
    m.def("create_le_tt", &fiction::networks::utils::create_le_tt, DOC(fiction_networks_utils_create_le_tt));
    m.def("create_ge_tt", &fiction::networks::utils::create_ge_tt, DOC(fiction_networks_utils_create_ge_tt));
    m.def("create_and3_tt", &fiction::networks::utils::create_and3_tt, DOC(fiction_networks_utils_create_and3_tt));
    m.def("create_xor_and_tt", &fiction::networks::utils::create_xor_and_tt,
          DOC(fiction_networks_utils_create_xor_and_tt));
    m.def("create_or_and_tt", &fiction::networks::utils::create_or_and_tt,
          DOC(fiction_networks_utils_create_or_and_tt));
    m.def("create_onehot_tt", &fiction::networks::utils::create_onehot_tt,
          DOC(fiction_networks_utils_create_onehot_tt));
    m.def("create_maj_tt", &fiction::networks::utils::create_maj_tt, DOC(fiction_networks_utils_create_maj_tt));
    m.def("create_gamble_tt", &fiction::networks::utils::create_gamble_tt,
          DOC(fiction_networks_utils_create_gamble_tt));
    m.def("create_dot_tt", &fiction::networks::utils::create_dot_tt, DOC(fiction_networks_utils_create_dot_tt));
    m.def("create_ite_tt", &fiction::networks::utils::create_ite_tt, DOC(fiction_networks_utils_create_ite_tt));
    m.def("create_and_xor_tt", &fiction::networks::utils::create_and_xor_tt,
          DOC(fiction_networks_utils_create_and_xor_tt));
    m.def("create_xor3_tt", &fiction::networks::utils::create_xor3_tt, DOC(fiction_networks_utils_create_xor3_tt));
    m.def("create_double_wire_tt", &fiction::networks::utils::create_double_wire_tt,
          DOC(fiction_networks_utils_create_double_wire_tt));
    m.def("create_crossing_wire_tt", &fiction::networks::utils::create_crossing_wire_tt,
          DOC(fiction_networks_utils_create_crossing_wire_tt));
    m.def("create_fan_out_tt", &fiction::networks::utils::create_fan_out_tt,
          DOC(fiction_networks_utils_create_fan_out_tt));
    m.def("create_half_adder_tt", &fiction::networks::utils::create_half_adder_tt,
          DOC(fiction_networks_utils_create_half_adder_tt));
}

}  // namespace pyfiction
