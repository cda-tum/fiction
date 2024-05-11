//
// Created by marcel on 15.11.23.
//

#ifndef PYFICTION_TRUTH_TABLE_UTILS_HPP
#define PYFICTION_TRUTH_TABLE_UTILS_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/truth_table_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

inline void truth_table_utils(pybind11::module& m)
{
    m.def("create_id_tt", &fiction::create_id_tt, DOC(fiction_create_id_tt));
    m.def("create_not_tt", &fiction::create_not_tt, DOC(fiction_create_not_tt));
    m.def("create_and_tt", &fiction::create_and_tt, DOC(fiction_create_and_tt));
    m.def("create_or_tt", &fiction::create_or_tt, DOC(fiction_create_or_tt));
    m.def("create_nand_tt", &fiction::create_nand_tt, DOC(fiction_create_nand_tt));
    m.def("create_nor_tt", &fiction::create_nor_tt, DOC(fiction_create_nor_tt));
    m.def("create_xor_tt", &fiction::create_xor_tt, DOC(fiction_create_xor_tt));
    m.def("create_xnor_tt", &fiction::create_xnor_tt, DOC(fiction_create_xnor_tt));
    m.def("create_lt_tt", &fiction::create_lt_tt, DOC(fiction_create_lt_tt));
    m.def("create_gt_tt", &fiction::create_gt_tt, DOC(fiction_create_gt_tt));
    m.def("create_le_tt", &fiction::create_le_tt, DOC(fiction_create_le_tt));
    m.def("create_ge_tt", &fiction::create_ge_tt, DOC(fiction_create_ge_tt));
    m.def("create_and3_tt", &fiction::create_and3_tt, DOC(fiction_create_and3_tt));
    m.def("create_xor_and_tt", &fiction::create_xor_and_tt, DOC(fiction_create_xor_and_tt));
    m.def("create_or_and_tt", &fiction::create_or_and_tt, DOC(fiction_create_or_and_tt));
    m.def("create_onehot_tt", &fiction::create_onehot_tt, DOC(fiction_create_onehot_tt));
    m.def("create_maj_tt", &fiction::create_maj_tt, DOC(fiction_create_maj_tt));
    m.def("create_gamble_tt", &fiction::create_gamble_tt, DOC(fiction_create_gamble_tt));
    m.def("create_dot_tt", &fiction::create_dot_tt, DOC(fiction_create_dot_tt));
    m.def("create_ite_tt", &fiction::create_ite_tt, DOC(fiction_create_ite_tt));
    m.def("create_and_xor_tt", &fiction::create_and_xor_tt, DOC(fiction_create_and_xor_tt));
    m.def("create_xor3_tt", &fiction::create_xor3_tt, DOC(fiction_create_xor3_tt));
    m.def("create_double_wire_tt", &fiction::create_double_wire_tt, DOC(fiction_create_double_wire_tt));
    m.def("create_crossing_wire_tt", &fiction::create_crossing_wire_tt, DOC(fiction_create_crossing_wire_tt));
    m.def("create_fan_out_tt", &fiction::create_fan_out_tt, DOC(fiction_create_fan_out_tt));
    m.def("create_half_adder_tt", &fiction::create_half_adder_tt, DOC(fiction_create_half_adder_tt));
}

}  // namespace pyfiction

#endif  // PYFICTION_TRUTH_TABLE_UTILS_HPP
