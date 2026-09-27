/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/synthesis/truth_tables.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fiction/synthesis/truth_tables.hpp>

#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{
/**
 * @brief Binds named single-output and multi-output Boolean specifications.
 * @param m Python synthesis module.
 */
void truth_tables(nanobind::module_& m)
{
    std::map<std::string, std::vector<py_tt>> functions{
        {"id", {fiction::synthesis::create_id_tt()}},
        {"not", {fiction::synthesis::create_not_tt()}},
        {"and", {fiction::synthesis::create_and_tt()}},
        {"or", {fiction::synthesis::create_or_tt()}},
        {"nand", {fiction::synthesis::create_nand_tt()}},
        {"nor", {fiction::synthesis::create_nor_tt()}},
        {"xor", {fiction::synthesis::create_xor_tt()}},
        {"xnor", {fiction::synthesis::create_xnor_tt()}},
        {"lt", {fiction::synthesis::create_lt_tt()}},
        {"gt", {fiction::synthesis::create_gt_tt()}},
        {"le", {fiction::synthesis::create_le_tt()}},
        {"ge", {fiction::synthesis::create_ge_tt()}},
        {"and3", {fiction::synthesis::create_and3_tt()}},
        {"xor_and", {fiction::synthesis::create_xor_and_tt()}},
        {"or_and", {fiction::synthesis::create_or_and_tt()}},
        {"onehot", {fiction::synthesis::create_onehot_tt()}},
        {"maj", {fiction::synthesis::create_maj_tt()}},
        {"gamble", {fiction::synthesis::create_gamble_tt()}},
        {"dot", {fiction::synthesis::create_dot_tt()}},
        {"ite", {fiction::synthesis::create_ite_tt()}},
        {"and_xor", {fiction::synthesis::create_and_xor_tt()}},
        {"xor3", {fiction::synthesis::create_xor3_tt()}},
        {"double_wire", fiction::synthesis::create_double_wire_tt()},
        {"crossing_wire", fiction::synthesis::create_crossing_wire_tt()},
        {"fan_out", fiction::synthesis::create_fan_out_tt()},
        {"half_adder", fiction::synthesis::create_half_adder_tt()},
    };
    m.def(
        "standard_functions", [functions]() { return functions; },
        "Returns fresh truth tables for every named standard function. Each value lists the outputs in specification "
        "order.");
    m.def(
        "standard_functions",
        [functions = std::move(functions)](const std::string& name)
        {
            const auto found = functions.find(name);
            if (found == functions.end())
            {
                throw std::invalid_argument("unknown standard function: " + name);
            }
            return found->second;
        },
        nanobind::arg("name"),
        "Returns fresh truth tables for the named function, in specification output order. Unknown names raise "
        "ValueError.");
}
}  // namespace pyfiction
