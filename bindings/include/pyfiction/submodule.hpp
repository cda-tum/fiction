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
 * @brief Nested `mnt.pyfiction` submodules that the import system resolves.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Creates the submodule `name` of `parent` and registers it in `sys.modules`.
 *
 * `nanobind::module_::def_submodule` makes the submodule reachable as an attribute only.
 * The `sys.modules` entry lets `import` and `from ... import` statements resolve the full
 * dotted name, such as `mnt.pyfiction.sidb.simulation.engines`.
 *
 * @param parent Module that holds the submodule.
 * @param name Unqualified submodule name.
 * @param doc Submodule docstring.
 * @return The submodule.
 */
inline nanobind::module_ def_submodule(nanobind::module_& parent, const char* name, const char* doc)
{
    auto submodule = parent.def_submodule(name, doc);

    nanobind::module_::import_("sys").attr("modules")[submodule.attr("__name__")] = submodule;

    return submodule;
}

}  // namespace pyfiction
