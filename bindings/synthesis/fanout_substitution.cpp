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
 * @brief Python bindings for `fiction/synthesis/fanout_substitution.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/progress.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/fanout_substitution.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{

/**
 * @brief Binds fanout substitution helpers.
 * @param m Python module.
 * @tparam Ntk Network type.
 */
template <typename Ntk>
void bind_fanout_substitution_impl(nb::module_& m)
{
    m.def("fanout_substitution", &fiction::synthesis::fanout_substitution<py_tec_network, Ntk>, nb::arg("network"),
          nb::arg("params") = fiction::synthesis::fanout_substitution_params{},
          DOC(fiction_synthesis_fanout_substitution), nb::call_guard<nb::gil_scoped_release>());

    m.def("is_fanout_substituted", &fiction::synthesis::is_fanout_substituted<Ntk>, nb::arg("network"),
          nb::arg("params") = fiction::synthesis::fanout_substitution_params{},
          DOC(fiction_synthesis_is_fanout_substituted), nb::call_guard<nb::gil_scoped_release>());
}

}  // namespace detail

/**
 * @brief Binds fanout substitution helpers.
 * @param m Python module.
 */
void bind_fanout_substitution(nb::module_& m)
{
    nb::enum_<fiction::synthesis::fanout_substitution_params::substitution_strategy>(
        m, "substitution_strategy", DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy))
        .value("BREADTH", fiction::synthesis::fanout_substitution_params::substitution_strategy::BREADTH,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_BREADTH))
        .value("DEPTH", fiction::synthesis::fanout_substitution_params::substitution_strategy::DEPTH,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_DEPTH))
        .value("RANDOM", fiction::synthesis::fanout_substitution_params::substitution_strategy::RANDOM,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_RANDOM))

        ;

    nb::class_<fiction::synthesis::fanout_substitution_params>(
        m, "fanout_substitution_params",
        pyfiction::progress_type_slots<fiction::synthesis::fanout_substitution_params>(),
        DOC(fiction_synthesis_fanout_substitution_params))
        .def(nb::init<>(), "Default constructor.")
        .def_rw("on_progress", &fiction::synthesis::fanout_substitution_params::on_progress,
                pyfiction::ON_PROGRESS_GETTER, pyfiction::CALLBACK_SETTER,
                "Receives completed work and the phase total.")
        .def_rw("strategy", &fiction::synthesis::fanout_substitution_params::strategy,
                DOC(fiction_synthesis_fanout_substitution_params_strategy))
        .def_rw("degree", &fiction::synthesis::fanout_substitution_params::degree,
                DOC(fiction_synthesis_fanout_substitution_params_degree))
        .def_rw("threshold", &fiction::synthesis::fanout_substitution_params::threshold,
                DOC(fiction_synthesis_fanout_substitution_params_threshold))
        .def_rw("seed", &fiction::synthesis::fanout_substitution_params::seed,
                DOC(fiction_synthesis_fanout_substitution_params_seed))

        ;

    detail::bind_fanout_substitution_impl<py_tec_network>(m);
}

}  // namespace pyfiction
