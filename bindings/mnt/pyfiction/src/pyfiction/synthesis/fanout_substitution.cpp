/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/synthesis/fanout_substitution.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void fanout_substitution_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("fanout_substitution", &fiction::synthesis::fanout_substitution<py_logic_network, Ntk>, py::arg("network"),
          py::arg("params") = fiction::synthesis::fanout_substitution_params{},
          DOC(fiction_synthesis_fanout_substitution));

    m.def("is_fanout_substituted", &fiction::synthesis::is_fanout_substituted<Ntk>, py::arg("network"),
          py::arg("params") = fiction::synthesis::fanout_substitution_params{},
          DOC(fiction_synthesis_is_fanout_substituted));
}

}  // namespace detail

void fanout_substitution(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::synthesis::fanout_substitution_params::substitution_strategy>(
        m, "substitution_strategy", DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy))
        .value("BREADTH", fiction::synthesis::fanout_substitution_params::substitution_strategy::BREADTH,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_BREADTH))
        .value("DEPTH", fiction::synthesis::fanout_substitution_params::substitution_strategy::DEPTH,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_DEPTH))
        .value("RANDOM", fiction::synthesis::fanout_substitution_params::substitution_strategy::RANDOM,
               DOC(fiction_synthesis_fanout_substitution_params_substitution_strategy_RANDOM))

        ;

    py::class_<fiction::synthesis::fanout_substitution_params>(m, "fanout_substitution_params",
                                                               DOC(fiction_synthesis_fanout_substitution_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("strategy", &fiction::synthesis::fanout_substitution_params::strategy,
                DOC(fiction_synthesis_fanout_substitution_params_strategy))
        .def_rw("degree", &fiction::synthesis::fanout_substitution_params::degree,
                DOC(fiction_synthesis_fanout_substitution_params_degree))
        .def_rw("threshold", &fiction::synthesis::fanout_substitution_params::threshold,
                DOC(fiction_synthesis_fanout_substitution_params_threshold))
        .def_rw("seed", &fiction::synthesis::fanout_substitution_params::seed,
                DOC(fiction_synthesis_fanout_substitution_params_seed))

        ;

    detail::fanout_substitution_impl<py_logic_network>(m);
}

}  // namespace pyfiction
