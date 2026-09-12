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
 */

#include <fiction/utils/execution_timeout.hpp>

#include <exception>

#include <nanobind/nanobind.h>
#include <pyerrors.h>

namespace pyfiction
{

/**
 * @brief Registers the Python translator for execution timeouts.
 *
 * @param m Python module.
 */
void execution_timeout(nanobind::module_& /* m */)
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
