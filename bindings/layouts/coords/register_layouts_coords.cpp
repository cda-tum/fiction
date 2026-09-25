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
 * @brief Registers the bindings of the `mnt.pyfiction.layouts.coords` submodule.
 * @author OpenAI Codex
 */

#include <nanobind/nanobind.h>

namespace pyfiction
{

/** @brief Registers offset coordinates. @param m Python coordinate submodule. */
void offset_coordinate(nanobind::module_& m);
/** @brief Registers cube coordinates. @param m Python coordinate submodule. */
void cube_coordinate(nanobind::module_& m);
/** @brief Registers coordinate area and volume functions. @param m Python coordinate submodule. */
void coordinate_utility(nanobind::module_& m);

/**
 * @brief Registers coordinate types and their area and volume functions.
 * @param m Python coordinate submodule.
 */
void register_layouts_coords(nanobind::module_& m)
{
    offset_coordinate(m);
    cube_coordinate(m);
    coordinate_utility(m);
}

}  // namespace pyfiction
