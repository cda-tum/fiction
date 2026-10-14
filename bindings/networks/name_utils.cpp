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
 * @brief Python bindings for `fiction/networks/name_utils.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/networks/name_utils.hpp>
#include <fiction/technology/inml/layout.hpp>
#include <fiction/technology/mol_qca/layout.hpp>
#include <fiction/technology/qca/layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Registers name access for a network or layout type.
 * @tparam NtkOrLyt Network or layout type.
 * @param m Python module.
 */
template <typename NtkOrLyt>
void register_name_utils(nb::module_& m)
{
    // Keep the GIL for these short name lookups and updates; a release adds overhead.
    m.def("get_name", &fiction::networks::get_name<NtkOrLyt>, nb::arg("ntk_or_lyt"), DOC(fiction_networks_get_name));
    m.def("set_name", &fiction::networks::set_name<NtkOrLyt>, nb::arg("ntk_or_lyt"), nb::arg("name"),
          DOC(fiction_networks_set_name));
}

}  // namespace detail

/**
 * @brief Binds name utils helpers.
 * @param m Python module.
 */
void register_name_utils(nb::module_& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::register_name_utils<py_cartesian_gate_layout>(m);
    detail::register_name_utils<py_shifted_cartesian_gate_layout>(m);
    detail::register_name_utils<py_hexagonal_gate_layout>(m);
    detail::register_name_utils<fiction::qca::layout>(m);
    detail::register_name_utils<fiction::mol_qca::layout>(m);
    detail::register_name_utils<fiction::inml::layout>(m);
    detail::register_name_utils<fiction::sidb::layout>(m);
    detail::register_name_utils<py_tec_network>(m);
    detail::register_name_utils<py_aig_network>(m);
    detail::register_name_utils<py_xag_network>(m);
    detail::register_name_utils<py_mig_network>(m);
}

}  // namespace pyfiction
