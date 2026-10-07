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
 * @brief Maps execution timeouts to Python's built-in TimeoutError.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include <fiction/utils/execution_timeout.hpp>

#include <exception>

#include <nanobind/nanobind.h>
#include <pyerrors.h>

namespace pyfiction
{

/**
 * @brief Registers the Python translator for execution timeouts.
 *
 * Every extension module that can throw `fiction::utils::timeout_error` calls this function. A
 * translator catches the exception only in the module whose code threw it: the type information of
 * the exception is local to each module, and macOS does not match it across modules.
 */
inline void register_execution_timeout()
{
    nanobind::register_exception_translator(
        [](const std::exception_ptr& exception, void* /* unused */)
        {
            try
            {
                std::rethrow_exception(exception);
            }
            catch (const fiction::utils::timeout_error& error)
            {
                PyErr_SetString(PyExc_TimeoutError, error.what());
            }
        });
}

}  // namespace pyfiction
