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
 * @brief Python bindings for `fiction/synthesis/network_balancing.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/network_balancing.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds network balancing helpers.
 * @param m Python module.
 */
void register_network_balancing(nb::module_& m)
{
    nb::class_<fiction::synthesis::network_balancing_params>(
        m, "network_balancing_params", pyfiction::progress_type_slots<fiction::synthesis::network_balancing_params>(),
        DOC(fiction_synthesis_network_balancing_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("on_progress", &fiction::synthesis::network_balancing_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                "Receives completed work and the phase total.")
        .def_rw("unify_outputs", &fiction::synthesis::network_balancing_params::unify_outputs,
                DOC(fiction_synthesis_network_balancing_params_unify_outputs))

        ;

    m.def("network_balancing", &fiction::synthesis::network_balancing<py_tec_network, py_tec_network>,
          nb::arg("network"), nb::arg("params") = fiction::synthesis::network_balancing_params{},
          DOC(fiction_synthesis_network_balancing), nb::call_guard<nb::gil_scoped_release>());

    m.def("is_balanced", &fiction::synthesis::is_balanced<py_tec_network>, nb::arg("network"),
          nb::arg("params") = fiction::synthesis::network_balancing_params{}, DOC(fiction_synthesis_is_balanced),
          nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace pyfiction
