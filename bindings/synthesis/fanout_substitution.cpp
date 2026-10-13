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
void register_fanout_substitution_impl(nb::module_& m)
{
    m.def(
        "fanout_substitution",
        [](const Ntk& network, const decltype(fiction::synthesis::fanout_substitution_params{}.on_progress) on_progress,
           const decltype(fiction::synthesis::fanout_substitution_params{}.strategy)  strategy,
           const decltype(fiction::synthesis::fanout_substitution_params{}.degree)    degree,
           const decltype(fiction::synthesis::fanout_substitution_params{}.threshold) threshold,
           const decltype(fiction::synthesis::fanout_substitution_params{}.seed)      seed)
        {
            fiction::synthesis::fanout_substitution_params params{};
            params.on_progress = on_progress;
            params.strategy    = strategy;
            params.degree      = degree;
            params.threshold   = threshold;
            params.seed        = seed;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::fanout_substitution<py_tec_network, Ntk>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress") = nb::none(),
        nb::arg("strategy")  = fiction::synthesis::fanout_substitution_params{}.strategy,
        nb::arg("degree")    = fiction::synthesis::fanout_substitution_params{}.degree,
        nb::arg("threshold") = fiction::synthesis::fanout_substitution_params{}.threshold,
        nb::arg("seed")      = fiction::synthesis::fanout_substitution_params{}.seed,
        R"doc(Substitutes high-output degrees in a logic network with fanout nodes
that compute the identity function. For this purpose, `create_buf` is
utilized. Therefore, `NtkDest` should support identity nodes. If it
does not, no new nodes will in fact be created. A destination that
elides buffer nodes may exceed the requested fanout degrees. The
returned network is logically equivalent to the input network.

The algorithm builds breadth-first, depth-first, or random fanout
trees. Parameters set the maximum output degrees of ordinary nodes and
fanout nodes.

The returned network is newly created from scratch because its type
`NtkDest` may differ from `NtkSrc`. Repeated nonconstant inputs use
distinct routing branches when `NtkDest` preserves buffer nodes.

Args:
    ntk_src: The input logic network.

Returns:
    A fanout-substituted logic network of type `NtkDest` that is
    logically equivalent to `ntk_src`.

Raises:
    std::invalid_argument: If `degree` is less than two or
                           `threshold` is zero.

Note:
    The physical design algorithms natively provided in fiction do not
    require their input networks to be fanout-substituted. If that is
    necessary, they will do it themselves. Providing already
    substituted networks does however allow for the control over
    maximum output degrees.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");

    m.def(
        "is_fanout_substituted",
        [](const Ntk& network, const decltype(fiction::synthesis::fanout_substitution_params{}.on_progress) on_progress,
           const decltype(fiction::synthesis::fanout_substitution_params{}.strategy)  strategy,
           const decltype(fiction::synthesis::fanout_substitution_params{}.degree)    degree,
           const decltype(fiction::synthesis::fanout_substitution_params{}.threshold) threshold,
           const decltype(fiction::synthesis::fanout_substitution_params{}.seed)      seed)
        {
            fiction::synthesis::fanout_substitution_params params{};
            params.on_progress = on_progress;
            params.strategy    = strategy;
            params.degree      = degree;
            params.threshold   = threshold;
            params.seed        = seed;
            const nb::gil_scoped_release release{};
            return fiction::synthesis::is_fanout_substituted<Ntk>(network, params);
        },
        nb::arg("network"), nb::kw_only(), nb::arg("on_progress") = nb::none(),
        nb::arg("strategy")  = fiction::synthesis::fanout_substitution_params{}.strategy,
        nb::arg("degree")    = fiction::synthesis::fanout_substitution_params{}.degree,
        nb::arg("threshold") = fiction::synthesis::fanout_substitution_params{}.threshold,
        nb::arg("seed")      = fiction::synthesis::fanout_substitution_params{}.seed,
        R"doc(Checks if a logic network is properly fanout-substituted with regard
to the keyword options, i.e., if no node exceeds the specified
fanout limits.

Args:
    ntk: The logic network to check.

Returns:
    `true` iff `ntk` is properly fanout-substituted with regard to
    the keyword options.

Algorithm options are keyword-only; the signature lists their defaults.
)doc");
}

}  // namespace detail

/**
 * @brief Binds fanout substitution helpers.
 * @param m Python module.
 */
void register_fanout_substitution(nb::module_& m)
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

    detail::register_fanout_substitution_impl<py_tec_network>(m);
}

}  // namespace pyfiction
