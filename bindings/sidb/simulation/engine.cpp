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
 * @brief Python bindings for `fiction/technology/sidb/simulation/engine.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Willem Lambooy (wlambooy)
 */

#include "pyfiction/documentation.hpp"

#include <fiction/technology/sidb/simulation/engine.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
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
 * @brief Binds `sidb_simulation_engine_name_impl`.
 * @param m Python module.
 * @tparam EngineType Layout type.
 */
template <typename EngineType>
void bind_sidb_simulation_engine_name_impl(nb::module_& m)
{
    m.def(
        "sidb_simulation_engine_name",
        [](const EngineType& engine) -> std::string_view { return fiction::sidb::simulation::engine_name(engine); },
        nb::arg("engine"), DOC(fiction_sidb_simulation_engine_name));
}

}  // namespace detail

/**
 * @brief Binds `engine`.
 * @param m Python module.
 */
void bind_engine(nb::module_& m)
{
    nb::enum_<fiction::sidb::simulation::engine>(m, "sidb_simulation_engine", DOC(fiction_sidb_simulation_engine))
        .value("EXGS", fiction::sidb::simulation::engine::EXGS, DOC(fiction_sidb_simulation_engine_EXGS))
        .value("QUICKSIM", fiction::sidb::simulation::engine::QUICKSIM, DOC(fiction_sidb_simulation_engine_QUICKSIM))
        .value("QUICKEXACT", fiction::sidb::simulation::engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::sidb::simulation::engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    nb::enum_<fiction::sidb::simulation::exact_engine>(m, "exact_sidb_simulation_engine",
                                                       DOC(fiction_sidb_simulation_exact_engine))
        .value("EXGS", fiction::sidb::simulation::exact_engine::EXGS, DOC(fiction_sidb_simulation_exact_engine_EXGS))
        .value("QUICKEXACT", fiction::sidb::simulation::exact_engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_exact_engine_QUICKEXACT))
#if (FICTION_ALGLIB_ENABLED)
        .value("CLUSTERCOMPLETE", fiction::sidb::simulation::exact_engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_exact_engine_CLUSTERCOMPLETE))
#endif  // FICTION_ALGLIB_ENABLED

        ;

    nb::enum_<fiction::sidb::simulation::heuristic_engine>(m, "heuristic_sidb_simulation_engine",
                                                           DOC(fiction_sidb_simulation_heuristic_engine))
        .value("QUICKSIM", fiction::sidb::simulation::heuristic_engine::QUICKSIM,
               DOC(fiction_sidb_simulation_heuristic_engine_QUICKSIM))

        ;

    detail::bind_sidb_simulation_engine_name_impl<fiction::sidb::simulation::engine>(m);
    detail::bind_sidb_simulation_engine_name_impl<fiction::sidb::simulation::exact_engine>(m);
    detail::bind_sidb_simulation_engine_name_impl<fiction::sidb::simulation::heuristic_engine>(m);
}

}  // namespace pyfiction
