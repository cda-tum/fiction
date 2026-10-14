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
 * @brief Python bindings for `fiction/networks/network_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/network_utils.hpp>
#include <fiction/synthesis/network_conversion.hpp>

#include <mockturtle/generators/random_network.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds network utils helpers.
 * @param m Python module.
 */
void register_network_utils(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::networks::high_degree_fanin_exception>(
        m, "high_degree_fanin_exception",
        PyExc_ValueError);  // NOLINT(misc-include-cleaner): included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    m.def("has_high_degree_fanin_nodes", &fiction::networks::has_high_degree_fanin_nodes<py_tec_network>,
          nb::arg("ntk"), nb::arg("threshold") = 2, DOC(fiction_networks_has_high_degree_fanin_nodes));
    m.def(
        "random_aig_network",
        [](const uint32_t inputs, const uint32_t gates, const uint64_t seed)
        {
            if (inputs < 2u || gates == 0u)
            {
                throw std::invalid_argument("random generation requires at least two inputs and one gate");
            }
            const mockturtle::random_network_generator_params_size params{.seed      = seed,
                                                                          .num_pis   = inputs,
                                                                          .num_gates = gates};
            auto                                                   generator = mockturtle::random_aig_generator(params);
            auto network = fiction::synthesis::convert_network<py_aig_network>(generator.generate());
            network.set_network_name(std::to_string(seed));
            return network;
        },
        nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), "Generate a native random AIG network.");

    m.def(
        "random_xag_network",
        [](const uint32_t inputs, const uint32_t gates, const uint64_t seed)
        {
            if (inputs < 2u || gates == 0u)
            {
                throw std::invalid_argument("random generation requires at least two inputs and one gate");
            }
            const mockturtle::random_network_generator_params_size params{.seed      = seed,
                                                                          .num_pis   = inputs,
                                                                          .num_gates = gates};
            auto                                                   generator = mockturtle::random_xag_generator(params);
            auto network = fiction::synthesis::convert_network<py_xag_network>(generator.generate());
            network.set_network_name(std::to_string(seed));
            return network;
        },
        nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), "Generate a native random XAG network.");

    m.def(
        "random_mig_network",
        [](const uint32_t inputs, const uint32_t gates, const uint64_t seed)
        {
            if (inputs < 2u || gates == 0u)
            {
                throw std::invalid_argument("random generation requires at least two inputs and one gate");
            }
            const mockturtle::random_network_generator_params_size params{.seed      = seed,
                                                                          .num_pis   = inputs,
                                                                          .num_gates = gates};
            auto                                                   generator = mockturtle::random_mig_generator(params);
            auto network = fiction::synthesis::convert_network<py_mig_network>(generator.generate());
            network.set_network_name(std::to_string(seed));
            return network;
        },
        nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), "Generate a native random MIG network.");

    m.def(
        "random_tec_network",
        [](const uint32_t inputs, const uint32_t gates, const uint64_t seed)
        {
            if (inputs < 2u || gates == 0u)
            {
                throw std::invalid_argument("random generation requires at least two inputs and one gate");
            }
            const mockturtle::random_network_generator_params_size params{.seed      = seed,
                                                                          .num_pis   = inputs,
                                                                          .num_gates = gates};
            auto generator = mockturtle::mixed_random_mig_generator(params);
            auto network   = fiction::synthesis::convert_network<py_tec_network>(generator.generate());
            network.set_network_name(std::to_string(seed));
            return network;
        },
        nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), "Generate a native random TEC network.");
}

}  // namespace pyfiction
