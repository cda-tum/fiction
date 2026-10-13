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
#include <type_traits>

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

namespace detail
{

/**
 * @brief Binds `has_high_degree_fanin_nodes`.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void register_has_high_degree_fanin_nodes(nb::module_& m)
{
    m.def("has_high_degree_fanin_nodes", &fiction::networks::has_high_degree_fanin_nodes<Ntk>, nb::arg("ntk"),
          nb::arg("threshold") = 2, DOC(fiction_networks_has_high_degree_fanin_nodes));
}

/**
 * @brief Bind random generation for the selected registered network class.
 * @tparam Ntk Network representation.
 * @param m Networks module.
 * @param signature Python signature including the selected return type.
 */
template <typename Ntk>
void register_random_network(nb::module_& m, const char* signature)
{
    const auto generate =
        [](const uint32_t inputs, const uint32_t gates, const uint64_t seed, const nb::handle network_type)
    {
        if (!network_type.is(nb::type<Ntk>()))
        {
            throw nb::next_overload();
        }
        if (inputs < 2u || gates == 0u)
        {
            throw std::invalid_argument("random generation requires at least two inputs and one gate");
        }
        const nb::gil_scoped_release                           release{};
        const mockturtle::random_network_generator_params_size params{.seed      = seed,
                                                                      .num_pis   = inputs,
                                                                      .num_gates = gates};
        auto                                                   generator = [&params]()
        {
            if constexpr (std::is_same_v<Ntk, py_aig_network>)
            {
                return mockturtle::random_aig_generator(params);
            }
            else if constexpr (std::is_same_v<Ntk, py_xag_network>)
            {
                return mockturtle::random_xag_generator(params);
            }
            else if constexpr (std::is_same_v<Ntk, py_mig_network>)
            {
                return mockturtle::random_mig_generator(params);
            }
            else
            {
                return mockturtle::mixed_random_mig_generator(params);
            }
        }();
        auto network = fiction::synthesis::convert_network<Ntk>(generator.generate());
        network.set_network_name(std::to_string(seed));
        return network;
    };
    if constexpr (std::is_same_v<Ntk, py_tec_network>)
    {
        m.def("random_network", generate, nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), nb::kw_only(),
              nb::arg("network_type") = nb::type<Ntk>(), nb::sig(signature),
              "Generate directly in the selected network representation. network_type defaults to technology_network. "
              "Require at least two inputs and one gate; use the seed as the network name.");
    }
    else
    {
        m.def("random_network", generate, nb::arg("inputs"), nb::arg("gates"), nb::arg("seed"), nb::kw_only(),
              nb::arg("network_type"), nb::sig(signature));
    }
}

}  // namespace detail

/**
 * @brief Binds `network_utils`.
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

    detail::register_has_high_degree_fanin_nodes<py_tec_network>(m);
    detail::register_random_network<py_tec_network>(
        m, "def random_network(inputs: int, gates: int, seed: int, *, network_type: "
           "type[mnt.pyfiction.networks.technology_network] = mnt.pyfiction.networks.technology_network) -> "
           "mnt.pyfiction.networks.technology_network");
    detail::register_random_network<py_aig_network>(
        m, "def random_network(inputs: int, gates: int, seed: int, *, network_type: "
           "type[mnt.pyfiction.networks.aig_network]) -> mnt.pyfiction.networks.aig_network");
    detail::register_random_network<py_xag_network>(
        m, "def random_network(inputs: int, gates: int, seed: int, *, network_type: "
           "type[mnt.pyfiction.networks.xag_network]) -> mnt.pyfiction.networks.xag_network");
    detail::register_random_network<py_mig_network>(
        m, "def random_network(inputs: int, gates: int, seed: int, *, network_type: "
           "type[mnt.pyfiction.networks.mig_network]) -> mnt.pyfiction.networks.mig_network");
    m.def(
        "random_network", [](const uint32_t, const uint32_t, const uint64_t, const nb::handle) -> nb::object
        { throw nb::type_error("network_type must be a registered network class"); }, nb::arg("inputs"),
        nb::arg("gates"), nb::arg("seed"), nb::kw_only(), nb::arg("network_type"),
        nb::sig("def random_network(inputs: int, gates: int, seed: int, *, network_type: "
                "type[mnt.pyfiction.networks.technology_network | mnt.pyfiction.networks.aig_network | "
                "mnt.pyfiction.networks.xag_network | mnt.pyfiction.networks.mig_network]) -> "
                "mnt.pyfiction.networks.technology_network | mnt.pyfiction.networks.aig_network | "
                "mnt.pyfiction.networks.xag_network | mnt.pyfiction.networks.mig_network"));
}

}  // namespace pyfiction
