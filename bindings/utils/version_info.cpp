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
 * @brief Python bindings for the version and repository metadata CMake generates.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/utils/version_info.hpp>

#include <nanobind/nanobind.h>

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds version info helpers.
 * @param m Python module.
 */
void register_version_info(nb::module_& m)
{
    m.attr("__version__")       = fiction::FICTION_VERSION;
    m.attr("__repo__")          = fiction::FICTION_REPO;
    m.attr("__compiled_date__") = fiction::COMPILED_DATE;
    m.attr("__compiled_time__") = fiction::COMPILED_TIME;
}

}  // namespace pyfiction
