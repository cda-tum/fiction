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
 * @brief Python bindings for `fiction/technology/sidb/generators/random_layout_generator.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/simulation_parameters.hpp"

#include <fiction/technology/sidb/generators/random_layout_generator.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/** @brief Native options constructed for each Python call. */
using generate_random_layout_options = fiction::sidb::generators::generate_random_layout_params;

/**
 * @brief Binds random layout generator helpers.
 * @param m Python module.
 */
void register_random_layout_generator(nb::module_& m)
{
    using fiction::sidb::layout;

    nb::enum_<generate_random_layout_options::positive_charges>(
        m, "positive_charges", DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges))
        .value("ALLOWED", generate_random_layout_options::positive_charges::ALLOWED,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_ALLOWED))
        .value("FORBIDDEN", generate_random_layout_options::positive_charges::FORBIDDEN,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_FORBIDDEN))
        .value("MAY_OCCUR", generate_random_layout_options::positive_charges::MAY_OCCUR,
               DOC(fiction_sidb_generators_generate_random_layout_params_positive_charges_MAY_OCCUR));
    m.def(
        "generate_random_sidb_layout",
        [](const std::optional<layout>&                                          skeleton,
           const decltype(generate_random_layout_options{}.coordinate_pair)      coordinate_pair,
           const decltype(generate_random_layout_options{}.number_of_sidbs)      number_of_sidbs,
           const decltype(generate_random_layout_options{}.positive_sidbs)       positive_sidbs,
           const decltype(generate_random_layout_options{}.sim_params.base)      base,
           const decltype(generate_random_layout_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(generate_random_layout_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(generate_random_layout_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(generate_random_layout_options{}.maximal_attempts)     maximal_attempts,
           const decltype(generate_random_layout_options{}
                              .number_of_unique_generated_layouts)               number_of_unique_generated_layouts,
           const decltype(generate_random_layout_options{}
                              .maximal_attempts_for_multiple_layouts)            maximal_attempts_for_multiple_layouts,
           const decltype(generate_random_layout_options{}.on_progress)&         on_progress)
        {
            generate_random_layout_options params{};
            params.sim_params       = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.coordinate_pair  = coordinate_pair;
            params.number_of_sidbs  = number_of_sidbs;
            params.positive_sidbs   = positive_sidbs;
            params.maximal_attempts = maximal_attempts;
            params.number_of_unique_generated_layouts    = number_of_unique_generated_layouts;
            params.maximal_attempts_for_multiple_layouts = maximal_attempts_for_multiple_layouts;
            params.on_progress                           = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::generators::generate_random_layout(params, skeleton);
        },
        nb::arg("lyt_skeleton")     = std::nullopt, nb::kw_only(),
        nb::arg("coordinate_pair")  = generate_random_layout_options{}.coordinate_pair,
        nb::arg("number_of_sidbs")  = generate_random_layout_options{}.number_of_sidbs,
        nb::arg("positive_sidbs")   = generate_random_layout_options{}.positive_sidbs,
        nb::arg("base")             = generate_random_layout_options{}.sim_params.base,
        nb::arg("mu_minus")         = generate_random_layout_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")        = generate_random_layout_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")        = generate_random_layout_options{}.sim_params.lambda_tf,
        nb::arg("maximal_attempts") = generate_random_layout_options{}.maximal_attempts,
        nb::arg("number_of_unique_generated_layouts") =
            generate_random_layout_options{}.number_of_unique_generated_layouts,
        nb::arg("maximal_attempts_for_multiple_layouts") =
            generate_random_layout_options{}.maximal_attempts_for_multiple_layouts,
        nb::arg("on_progress").none() = nb::none(),
        R"doc(Generate a random SiDB layout within coordinate_pair, optionally extending lyt_skeleton.
The keyword options select physical values, charge restrictions, and attempt limits.
Returns None when the attempt limit does not produce a layout.
)doc");

    m.def(
        "generate_multiple_random_sidb_layouts",
        [](const std::optional<layout>&                                          skeleton,
           const decltype(generate_random_layout_options{}.coordinate_pair)      coordinate_pair,
           const decltype(generate_random_layout_options{}.number_of_sidbs)      number_of_sidbs,
           const decltype(generate_random_layout_options{}.positive_sidbs)       positive_sidbs,
           const decltype(generate_random_layout_options{}.sim_params.base)      base,
           const decltype(generate_random_layout_options{}.sim_params.mu_minus)  mu_minus,
           const decltype(generate_random_layout_options{}.sim_params.epsilon_r) epsilon_r,
           const decltype(generate_random_layout_options{}.sim_params.lambda_tf) lambda_tf,
           const decltype(generate_random_layout_options{}.maximal_attempts)     maximal_attempts,
           const decltype(generate_random_layout_options{}
                              .number_of_unique_generated_layouts)               number_of_unique_generated_layouts,
           const decltype(generate_random_layout_options{}
                              .maximal_attempts_for_multiple_layouts)            maximal_attempts_for_multiple_layouts,
           const decltype(generate_random_layout_options{}.on_progress)&         on_progress)
        {
            generate_random_layout_options params{};
            params.sim_params       = make_simulation_parameters(base, mu_minus, epsilon_r, lambda_tf);
            params.coordinate_pair  = coordinate_pair;
            params.number_of_sidbs  = number_of_sidbs;
            params.positive_sidbs   = positive_sidbs;
            params.maximal_attempts = maximal_attempts;
            params.number_of_unique_generated_layouts    = number_of_unique_generated_layouts;
            params.maximal_attempts_for_multiple_layouts = maximal_attempts_for_multiple_layouts;
            params.on_progress                           = on_progress;
            const nb::gil_scoped_release release{};
            return fiction::sidb::generators::generate_multiple_random_layouts(params, skeleton);
        },
        nb::arg("lyt_skeleton")     = std::nullopt, nb::kw_only(),
        nb::arg("coordinate_pair")  = generate_random_layout_options{}.coordinate_pair,
        nb::arg("number_of_sidbs")  = generate_random_layout_options{}.number_of_sidbs,
        nb::arg("positive_sidbs")   = generate_random_layout_options{}.positive_sidbs,
        nb::arg("base")             = generate_random_layout_options{}.sim_params.base,
        nb::arg("mu_minus")         = generate_random_layout_options{}.sim_params.mu_minus,
        nb::arg("epsilon_r")        = generate_random_layout_options{}.sim_params.epsilon_r,
        nb::arg("lambda_tf")        = generate_random_layout_options{}.sim_params.lambda_tf,
        nb::arg("maximal_attempts") = generate_random_layout_options{}.maximal_attempts,
        nb::arg("number_of_unique_generated_layouts") =
            generate_random_layout_options{}.number_of_unique_generated_layouts,
        nb::arg("maximal_attempts_for_multiple_layouts") =
            generate_random_layout_options{}.maximal_attempts_for_multiple_layouts,
        nb::arg("on_progress").none() = nb::none(),
        R"doc(Generate distinct random SiDB layouts, optionally extending lyt_skeleton.
The keyword options select the area, physical values, requested count, and attempt limits.
Returns None when the attempt limit does not produce the requested layouts.
)doc");
}

}  // namespace pyfiction
