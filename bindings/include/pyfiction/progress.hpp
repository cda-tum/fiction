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
 * @brief Property annotations for the progress callbacks of algorithm parameters.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <nanobind/nanobind.h>

namespace pyfiction
{

/**
 * @brief Types the getter of an `on_progress` member, which reads `None` while no callback is set.
 */
inline const auto on_progress_getter = nanobind::for_getter(
    nanobind::sig("def on_progress(self, /) -> collections.abc.Callable[[str, int, int], None] | None"));
/**
 * @brief Types the getter of an `on_worker_progress` member, which reads `None` while no callback is set.
 */
inline const auto on_worker_progress_getter = nanobind::for_getter(nanobind::sig(
    "def on_worker_progress(self, /) -> collections.abc.Callable[[int, int, str, int, int, bool], None] | None"));
/**
 * @brief Lets the setter of a callback member take `None`, which clears the callback.
 */
inline const auto callback_setter = nanobind::for_setter(nanobind::arg("value").none());

}  // namespace pyfiction
