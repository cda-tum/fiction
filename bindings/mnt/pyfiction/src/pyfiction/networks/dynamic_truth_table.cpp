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
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>  // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

/**
 * Rejects a string that holds a character outside the alphabet of the respective truth table constructor.
 *
 * `kitty` reads every character it is given: a binary character other than `1` becomes a `0`, and an invalid
 * hexadecimal character becomes `-1`, which sets every bit of its block. Both produce a wrong truth table
 * instead of an error, so the characters are checked here.
 *
 * @param text The string to check.
 * @param alphabet The characters the constructor accepts.
 * @param what The name of the string in the error message.
 * @throws std::invalid_argument If `text` holds a character outside `alphabet`.
 */
inline void check_alphabet(const std::string& text, const std::string_view alphabet, const std::string_view what)
{
    const auto pos = text.find_first_not_of(alphabet);

    if (pos != std::string::npos)
    {
        throw std::invalid_argument(fmt::format("'{}' is not a {} character", text.at(pos), what));
    }
}

}  // namespace detail

void dynamic_truth_table(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<py_tt>(m, "dynamic_truth_table")
        .def(
            "__init__", [](py_tt* tt) { new (tt) py_tt{0u}; }, "Constructs the constant-zero truth table.")
        .def(
            "__init__",
            [](py_tt* tt, const uint32_t num_vars)
            {
                if (num_vars >= 38u)
                {
                    throw std::invalid_argument("truth tables require fewer than 38 variables");
                }
                new (tt) py_tt{num_vars};
            },
            py::arg("num_vars"), "Constructs a truth table with fewer than 38 variables.")

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

                detail::check_alphabet(binary, "01", "binary");

                kitty::create_from_binary_string(tt, binary);
            },
            py::arg("binary"),
            "Sets the bits from a string of `0` and `1` characters, most significant bit first. The string must "
            "hold exactly `num_bits()` characters. Any other character raises a `ValueError`.")
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

                detail::check_alphabet(hex, "0123456789abcdefABCDEF", "hexadecimal");

                kitty::create_from_hex_string(tt, hex);
            },
            py::arg("hex"),
            "Sets the bits from a hexadecimal string, most significant digit first. The string must hold exactly "
            "`num_bits() / 4` digits, or one digit for fewer than two variables. Any non-hexadecimal character "
            "raises a `ValueError`.")
        .def(
            "create_from_expression",
            [](py_tt& tt, const std::string& expression)
            {
                for (const auto variable : expression)
                {
                    if (variable >= 'a' && variable <= 'p' && static_cast<uint32_t>(variable - 'a') >= tt.num_vars())
                    {
                        throw std::invalid_argument(
                            fmt::format("variable '{}' exceeds the table's {} variables", variable, tt.num_vars()));
                    }
                }
                py_tt parsed{tt.num_vars()};
                if (!kitty::create_from_expression(parsed, expression))
                {
                    throw std::invalid_argument(fmt::format("could not parse expression '{}'", expression));
                }
                tt = std::move(parsed);
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
