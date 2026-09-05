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
 * @brief Python bindings for `kitty`'s dynamic truth tables.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/types.hpp"

#include <fmt/format.h>
#include <kitty/constructors.hpp>
#include <kitty/print.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void dynamic_truth_table(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<py_tt>(m, "dynamic_truth_table")
        .def(py::init<>(), "Default constructor. Constructs a truth table of 0 variables.")
        .def(py::init<uint32_t>(), py::arg("num_vars"), "Constructs a truth table of the given number of variables.")

        .def("num_vars", &py_tt::num_vars)
        .def("num_blocks", &py_tt::num_blocks)
        .def("num_bits", &py_tt::num_bits)

        .def(
            "create_from_binary_string",
            [](py_tt& tt, const std::string& binary)
            {
                if (binary.size() != tt.num_bits())
                {
                    throw std::invalid_argument(fmt::format("a truth table of {} variables needs {} bits, got {}",
                                                            tt.num_vars(), tt.num_bits(), binary.size()));
                }

                kitty::create_from_binary_string(tt, binary);
            },
            py::arg("binary"),
            "Sets the bits from a string of `0` and `1` characters, most significant bit first. The string must "
            "hold exactly `num_bits()` characters.")
        .def(
            "create_from_hex_string",
            [](py_tt& tt, const std::string& hex)
            {
                // truth tables of fewer than two variables occupy a single hex digit
                const auto expected = tt.num_vars() < 2 ? 1u : tt.num_bits() / 4u;

                if (hex.size() != expected)
                {
                    throw std::invalid_argument(fmt::format("a truth table of {} variables needs {} hex digits, got {}",
                                                            tt.num_vars(), expected, hex.size()));
                }

                kitty::create_from_hex_string(tt, hex);
            },
            py::arg("hex"),
            "Sets the bits from a hexadecimal string, most significant digit first. The string must hold exactly "
            "`num_bits() / 4` digits, or one digit for fewer than two variables.")
        .def(
            "create_from_expression",
            [](py_tt& tt, const std::string& expression)
            {
                if (!kitty::create_from_expression(tt, expression))
                {
                    throw std::invalid_argument(fmt::format("could not parse expression '{}'", expression));
                }
            },
            py::arg("expression"),
            "Sets the bits from a Boolean expression over the variables `a` to `p`: constants `0` and `1`, negation "
            "`!E`, conjunction `(E...E)`, disjunction `{E...E}`, exclusive OR `[E...E]`, and majority `<EEE>`. The "
            "truth table must have at least as many variables as the largest one the expression uses.")
        .def(
            "create_random", [](py_tt& tt) { kitty::create_random(tt); }, "Sets every bit to a random value.")

        .def(
            "to_binary", [](const py_tt& tt) { return kitty::to_binary(tt); },
            "Returns the bits as a string of `0` and `1` characters, most significant bit first.")
        .def(
            "to_hex", [](const py_tt& tt) { return kitty::to_hex(tt); },
            "Returns the bits as a hexadecimal string, most significant digit first.")

        ;
}

}  // namespace pyfiction
